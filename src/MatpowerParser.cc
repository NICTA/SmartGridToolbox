#include "MatpowerParser.h"
#include <smartgridtoolbox/Bus.h>
#include <smartgridtoolbox/Network.h>
#include <smartgridtoolbox/PowerFlow.h>
#include <smartgridtoolbox/ZipToGround.h>

#include <fstream>
#include <map>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_function.hpp>
#include <boost/spirit/include/phoenix_statement.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/include/support_multi_pass.hpp>

namespace Qi = boost::spirit::qi;
namespace Ascii = boost::spirit::ascii;
namespace Phoenix = boost::phoenix;

typedef std::istreambuf_iterator<char> BaseIterator;
typedef boost::spirit::multi_pass<BaseIterator> ForwardIterator;

namespace SmartGridToolbox
{
   typedef ForwardIterator Iterator;
   typedef decltype(Ascii::blank) SpaceType;

   struct Gram : Qi::grammar<Iterator, SpaceType>
   {
      Gram(double & MVABaseVal,
           std::vector<double> & busMatrixVal,
           std::vector<double> & genMatrixVal,
           std::vector<double> & branchMatrixVal)
         : Gram::base_type(start_)
      {
         statementTerm_ = (Qi::eol | Qi::lit(';'));

         ignore_ =   (Qi::eol) | (Qi::lit('%') >> *(Qi::char_-Qi::eol) >> Qi::eol);
         other_ = (*(Qi::char_-statementTerm_) >> statementTerm_);

         matrix_ = Qi::lit('[') >> -Qi::eol >> Qi::skip(Ascii::blank|Qi::eol|Qi::char_(",;"))[*Qi::double_] >> -Qi::eol
            >> Qi::lit(']');
         // TODO: having struggled with Qi, I am quitting while ahead. No attempt is made to verify that there are
         // the right number of rows and columns in the matrix; it is just treated as a one-dimensional array of
         // elements.

         topFunction_ = Qi::lit("function") >> Qi::lit("mpc") >> Qi::lit("=") >> *(Qi::char_-statementTerm_)
            >> statementTerm_;
         MVABase_ = (Qi::lit("mpc.baseMVA") >> Qi::lit('=') >> Qi::double_ >> statementTerm_)
                    [Phoenix::ref(MVABaseVal) = Qi::_1];
         busMatrix_ = (Qi::lit("mpc.bus") >> Qi::lit('=') >> matrix_ >> statementTerm_)
            [Phoenix::ref(busMatrixVal) = Qi::_1];
         genMatrix_ = (Qi::lit("mpc.gen") >> Qi::lit('=') >> matrix_ >> statementTerm_)
            [Phoenix::ref(genMatrixVal) = Qi::_1];
         branchMatrix_ = (Qi::lit("mpc.branch") >> Qi::lit('=') >> matrix_ >> statementTerm_)
            [Phoenix::ref(branchMatrixVal) = Qi::_1];

         start_ =  *ignore_ >> topFunction_ >> *(ignore_ | MVABase_ | busMatrix_ | genMatrix_ | branchMatrix_ | other_);

         // To debug: e.g.
         // BOOST_SPIRIT_DEBUG_NODE(MVABase_); 
         // debug(MVABase_); 
      }

      Qi::rule<Iterator, SpaceType> statementTerm_;

      Qi::rule<Iterator, SpaceType> ignore_;
      Qi::rule<Iterator, SpaceType> other_;

      Qi::rule<Iterator, std::vector<double>(), SpaceType> matrix_;

      Qi::rule<Iterator, SpaceType> topFunction_;
      Qi::rule<Iterator, SpaceType> MVABase_;
      Qi::rule<Iterator, SpaceType> busMatrix_;
      Qi::rule<Iterator, SpaceType> genMatrix_;
      Qi::rule<Iterator, SpaceType> branchMatrix_;

      Qi::rule<Iterator, SpaceType> start_;
   };

   // All quantities expressed as injections:
   struct BusInfo
   {
      std::string name;
      int mPType;
      double VBase;
      Complex V;
      Complex SLoad;
      Complex Sg;
      Complex Ys;
   };

   static std::string busName(const std::string & prefix, int id)
   {
      return prefix + "_bus_" + std::to_string(id);
   }

   static std::string zipName(const std::string & prefix, int id)
   {
      return prefix + "_zip_" + std::to_string(id);
   }

   static std::string branchName(const std::string & prefix, int nBranch, int id1, int id2)
   {
      return prefix + "_branch_" + std::to_string(nBranch) + "_" + std::to_string(id1) + "_" + std::to_string(id2);
   }

   void MatpowerParser::parse(const YAML::Node & nd, Model & mod, const ParserState & state) const
   {
      // Matpower input units:
      // Power is in MW.
      // Voltage is in p.u., with the base being specified in KV by the KVBase field.
      // Branch admittance is in p.u., that is, MVABase / (KVBase)^2.
      // Shunt admittance is in MW / (KVBase)^2.
      
      // Power in SI = input * 1e6.
      // Voltage in SI = input * 1000 * KVBase.
      // Branch admittance in SI = input * 1e6 * MVABase / KVBase^2.
      // Shunt admittance in SI = input * 1e6 / KVBase^2.
      
      // Matpower input to internal units (p.u.):
      // Power is p.u. : input value divided by MVABase.
      // Voltage remains in p.u. : input value.
      // Branch admittance remains in p.u. : input value.
      // Shunt admittance is p.u. : input value divided by MVABase.
      
      // SI to matpower internal:
      // P_mp_intl = P_SI / (1e6 * MVABase)
      // V_mp_intl = V_SI / (1000 * KVBase)
      // y_mp_intl = y_SI * MVABase / KVBase^2 

      // TODO: Investigate what effect the generator base power field has on matpower by looking at the code.

      SGT_DEBUG(debug() << "Matpower : parse." << std::endl);

      assertFieldPresent(nd, "input_file");
      assertFieldPresent(nd, "network_name");
      assertFieldPresent(nd, "default_V_base");

      std::string inputName = state.expandName(nd["input_file"].as<std::string>());
      std::string networkName = state.expandName(nd["network_name"].as<std::string>());

      Phases phases;
      const YAML::Node ndPhases = nd["phases"];
      if (ndPhases)
      {
         phases = ndPhases.as<Phases>();
      }
      else
      {
         phases = Phase::BAL;
      }

      const YAML::Node ndPerUnit = nd["use_per_unit"];
      bool usePerUnit = ndPerUnit ? ndPerUnit.as<bool>() : false;
      
      double defaultVBase = nd["default_V_base"].as<double>();

      const YAML::Node ndFreq = nd["freq_Hz"];
      double freq = ndFreq ? ndFreq.as<double>() : 50.0;

      // Parse in the raw matpower data.
      std::vector<double> busMatrix;
      std::vector<double> genMatrix;
      std::vector<double> branchMatrix;
      double MVABase;
      {
         Gram gram(MVABase, busMatrix, genMatrix, branchMatrix);
         std::fstream infile(inputName);
         if (!infile.is_open())
         {
            error() << "Could not open the matpower input file " << inputName << "." << std::endl;
            abort();
         }

         // Iterate over stream input:
         BaseIterator inBegin(infile);

         ForwardIterator fwdBegin = boost::spirit::make_default_multi_pass(inBegin);
         ForwardIterator fwdEnd;

         while (fwdBegin != fwdEnd)
         {
            bool ok = Qi::phrase_parse(fwdBegin, fwdEnd, gram, Ascii::blank);
         }
      }
      double SBase = 1e6 * MVABase;

      const int busCols = 13;
      const int genCols = 21;
      const int branchCols = 13;

      int nBus = busMatrix.size() / busCols;
      int nGen = genMatrix.size() / genCols;
      int nBranch = branchMatrix.size() / branchCols;

      SGT_DEBUG(debug() << "Matpower: Bus matrix size = " << busCols << std::endl);
      SGT_DEBUG(debug() << "Matpower: Gen matrix size = " << genCols << std::endl);
      SGT_DEBUG(debug() << "Matpower: Branch matrix size = " << branchCols << std::endl);

      // Organize and scale the data:
      std::map<int, BusInfo> busMap;
      {
         for (int i = 0; i < nBus; ++i)
         {
            int busId = busMatrix[busCols * i];
            BusInfo & busInfo = busMap[busId];

            double KVBase = busMatrix[busCols * i + 9];
            busInfo.VBase = KVBase == 0 ? defaultVBase : KVBase * 1000;

            busInfo.mPType = busMatrix[busCols * i + 1];

            double Pd = busMatrix[busCols * i + 2];
            double Qd = busMatrix[busCols * i + 3];
            busInfo.SLoad = -Complex(Pd, Qd) * 1e6; // Injection is -ve load.
            if (usePerUnit)
            {
               busInfo.SLoad /= SBase;
            }

            double Gs = busMatrix[busCols * i + 4];
            double Bs = busMatrix[busCols * i + 5];
            busInfo.Ys = Complex(Gs, Bs) * 1e6 / (busInfo.VBase * busInfo.VBase);
            if (usePerUnit)
            {
               busInfo.Ys *= (busInfo.VBase * busInfo.VBase) / SBase;
            }

            double Vm = busMatrix[busCols * i + 7];
            double Va = busMatrix[busCols * i + 8];
            busInfo.V = polar(Vm, Va * pi/180);
            if (!usePerUnit)
            {
               busInfo.V *= busInfo.VBase;
            }
         }

         for (int i = 0; i < nGen; ++i)
         {
            int busId  = genMatrix[genCols * i];
            BusInfo & busInfo = busMap[busId];

            double Pg  = genMatrix[genCols * i + 1];
            double Qg  = genMatrix[genCols * i + 2];
            double Vg  = genMatrix[genCols * i + 5];
            if (!usePerUnit)
            {
               Vg *= busInfo.VBase;
            }

            busInfo.Sg = Complex(Pg, Qg) * 1e6; // Assuming 1 generator max per bus.
            if (usePerUnit)
            {
               busInfo.Sg /= SBase;
            }

            busInfo.V *= Vg / abs(busInfo.V); // Scale V of bus to match specified generator V.
         }
      }

      // Add new components.
      {
         Network & netw = mod.newComponent<Network>(networkName, freq);
         for (const auto pair : busMap)
         {
            const int & busId = pair.first;
            const BusInfo & info = pair.second;
            BusType type = BusType::BAD;
            switch (info.mPType)
            {
               case 1 :
                  type = BusType::PQ;
                  break;
               case 2 :
                  type = BusType::PV;
                  break;
               case 3 :
                  type = BusType::SL;
                  break;
               case 4:
                  error() << "Matpower isolated bus type not supported." << std::endl; 
                  abort();
                  break;
               default:
                  error() << "Bad matpower bus type (type = " << info.mPType << ") encountered." << std::endl;
                  abort();
                  break;
            }

            ublas::vector<Complex> VVec(phases.size(), info.V);
            ublas::vector<Complex> SLoadVec(phases.size(), info.SLoad);
            ublas::vector<Complex> SgVec(phases.size(), info.Sg);
            ublas::vector<Complex> YsVec(phases.size(), info.Ys);

            Bus & bus = mod.newComponent<Bus>(busName(networkName, busId), type, phases, VVec, VVec, SgVec);

            ZipToGround & zip = mod.newComponent<ZipToGround>(zipName(networkName, busId), phases);
            zip.S() = SLoadVec;
            zip.Y() = YsVec;
            bus.addZipToGround(zip);

            netw.addBus(bus);
         }

         for (int i = 0; i < nBranch; ++i)
         {
            int bus0Id = branchMatrix[branchCols * i];
            int bus1Id = branchMatrix[branchCols * i + 1];

            const BusInfo & busInfo0 = busMap[bus0Id];
            const BusInfo & busInfo1 = busMap[bus1Id];

            double VBase0 = busInfo0.VBase;
            double VBase1 = busInfo1.VBase;

            double Rs = branchMatrix[branchCols * i + 2];
            double Xs = branchMatrix[branchCols * i + 3];
            double Bc = branchMatrix[branchCols * i + 4];
            double tap = branchMatrix[branchCols * i + 8];
            double theta = branchMatrix[branchCols * i + 9]; // Matpower format is in deg, convert to rad.

            if (tap == 0.0)
            {
               tap = 1.0; // 0 means "default", i.e. 1.
            }

            Complex cTap = polar(tap, theta * pi / 180);

            Bus * bus0 = mod.componentNamed<Bus>(busName(networkName, bus0Id));
            if (bus0 == nullptr)
            {
               error() << "Matpower: for branch " << i << ", from bus " << bus0Id << " was not found." << std::endl;
               abort();
            }
            Bus * bus1 = mod.componentNamed<Bus>(busName(networkName, bus1Id));
            if (bus1 == nullptr)
            {
               error() << "Matpower: for branch " << i << ", to bus " << bus1Id << " was not found." << std::endl;
               abort();
            }

            Complex ys = 1.0 / Complex{Rs, Xs};

            Complex Y11 = (ys + Complex{0, 0.5 * Bc});
            Complex Y00 = Y11 / (tap * tap);
            Complex Y01 = -(ys / conj(cTap));
            Complex Y10 = -(ys / cTap);

            if (!usePerUnit)
            {
               // TODO: scaling across a transformer, looks fishy. Careful.
               Y00 *= SBase / (VBase0 * VBase0);
               Y01 *= SBase / (VBase0 * VBase1);
               Y10 *= SBase / (VBase1 * VBase0);
               Y11 *= SBase / (VBase1 * VBase1);
            }

            Branch & branch = mod.newComponent<Branch>(branchName(networkName, i, bus0Id, bus1Id), phases, phases);

            branch.setBus0(*bus0);
            branch.setBus1(*bus1);

            for (int k = 0; k < phases.size(); ++k)
            {
               branch.Y()(k, k) = Y00;
               branch.Y()(k + phases.size(), k + phases.size()) = Y11;
               branch.Y()(k, k + phases.size()) = Y01;
               branch.Y()(k + phases.size(), k) = Y10;
            }

            netw.addBranch(branch);
         }
      }
   }
}
