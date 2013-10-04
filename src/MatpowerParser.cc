#include "MatpowerParser.h"
#include "Bus.h"
#include "Network.h"
#include "PowerFlow.h"
#include "ZipToGround.h"

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
      Complex S;
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

   static std::string branchName(const std::string & prefix, int id1, int id2)
   {
      return prefix + "_branch_" + std::to_string(id1) + "_" + std::to_string(id2);
   }

   void MatpowerParser::parse(const YAML::Node & nd, Model & mod, const ParserState & state) const
   {
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

      double defaultVBase = nd["default_V_base"].as<double>();

      const YAML::Node ndScaleV = nd["scale_V"];
      double scaleV = ndScaleV ? ndScaleV.as<double>() : 1;
      
      const YAML::Node ndScaleP = nd["scale_P"];
      double scaleS = ndScaleP ? ndScaleP.as<double>() : 1;

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

      const int busCols = 13;
      const int genCols = 21;
      const int branchCols = 13;

      int nBus = busMatrix.size() / busCols;
      int nGen = genMatrix.size() / genCols;
      int nBranch = branchMatrix.size() / branchCols;

      message() << "Matpower: Bus matrix size = " << busCols << std::endl;
      message() << "Matpower: Gen matrix size = " << genCols << std::endl;
      message() << "Matpower: Branch matrix size = " << branchCols << std::endl;

      // Organize and scale the data:
      std::map<int, BusInfo> busMap;
      {
         for (int i = 0; i < nBus; ++i)
         {
            int busId      = busMatrix[busCols * i];
            BusInfo & busInfo = busMap[busId];

            double KVBase  = busMatrix[busCols * i + 9];
            busInfo.VBase  = KVBase == 0 ? defaultVBase : KVBase * 1000;

            double SBase   = MVABase * 1e6;
            double yBase   = SBase / (busInfo.VBase * busInfo.VBase);

            busInfo.mPType = busMatrix[busCols * i + 1];

            double Pd      = busMatrix[busCols * i + 2];
            double Qd      = busMatrix[busCols * i + 3];
            busInfo.S      = -Complex(Pd, Qd) * SBase; // Injection is -ve load.

            double Gs      = busMatrix[busCols * i + 4];
            double Bs      = busMatrix[busCols * i + 5];
            busInfo.Ys     = Complex(Gs, Bs) * yBase;

            double Vm      = busMatrix[busCols * i + 7];
            double Va      = busMatrix[busCols * i + 8];
            busInfo.V      = polar(Vm * busInfo.VBase, Va * pi/180);
         }

         for (int i = 0; i < nGen; ++i)
         {
            int busId  = genMatrix[genCols * i];
            BusInfo & busInfo = busMap[busId];

            double MVABaseGen  = genMatrix[genCols * i + 6];
            double SBase = (MVABaseGen == 0 ? MVABase : MVABaseGen) * 1e6;

            double Pg  = genMatrix[genCols * i + 1] * SBase;
            double Qg  = genMatrix[genCols * i + 2] * SBase;
            double Vg  = genMatrix[genCols * i + 5] * busInfo.VBase;

            busInfo.S += Complex(Pg, Qg) / SBase;
            busInfo.V *= Vg / abs(busInfo.V);
         }
      }

      // Add new components.
      {
         double scaleY = scaleS / (scaleV * scaleV);
         Network & netw = mod.newComponent<Network>(networkName);
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

            UblasVector<Complex> VVec(phases.size(), info.V);
            UblasVector<Complex> SVec(phases.size(), info.S);
            UblasVector<Complex> YsVec(phases.size(), info.Ys);

            Bus & bus = mod.newComponent<Bus>(busName(networkName, busId), type, phases, VVec / scaleV, VVec / scaleV);

            ZipToGround & zip = mod.newComponent<ZipToGround>(zipName(networkName, busId), phases);
            zip.S() = SVec / scaleS;
            zip.Y() = YsVec / scaleY;
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
            double SBase = MVABase * 1e6;
            double yBase0 = SBase / (VBase0 * VBase0);
            double yBase1 = SBase / (VBase1 * VBase1);

            double Rs = branchMatrix[branchCols * i + 2];
            double Xs = branchMatrix[branchCols * i + 3];
            double Bc = branchMatrix[branchCols * i + 4];
            double tap = branchMatrix[branchCols * i + 8];
            double theta = branchMatrix[branchCols * i + 9] * pi / 180.0; // Matpower format is in deg, convert to rad.

            if (tap == 0.0)
            {
               tap = 1.0; // 0 means "default", i.e. 1.
            }

            if (tap != 1.0 || theta != 0.0)
            {
               warning() << "Matpower: the tap and theta branch parameters have yet to be tested, " 
                  "and are likely to be wrong." << std::endl;
            }

            Complex cTap = polar(tap, theta * pi / 180);

            Bus * bus0 = mod.componentNamed<Bus>(busName(networkName, bus0Id));
            if (bus0 == nullptr)
            {
               error() << "Matpower: for generator " << i << ", from bus " << bus0Id << " was not found." << std::endl;
               abort();
            }
            Bus * bus1 = mod.componentNamed<Bus>(busName(networkName, bus1Id));
            if (bus1 == nullptr)
            {
               error() << "Matpower: for generator " << i << ", to bus " << bus1Id << " was not found." << std::endl;
               abort();
            }

            Complex ys = 1.0 / Complex{Rs, Xs};

            Complex Y11 = yBase1 * ((ys + Complex{0, 0.5 * Bc}));
            Complex Y00 = yBase0 * (Y11 / (tap * tap));
            Complex Y01 = yBase1 * (-(ys / conj(cTap)));
            Complex Y10 = yBase0 * (-(ys / cTap));

            Branch & branch = mod.newComponent<Branch>(branchName(networkName, bus0Id, bus1Id), phases, phases);

            branch.setBus0(*bus0);
            branch.setBus1(*bus1);

            for (int k = 0; k < phases.size(); ++k)
            {
               branch.Y()(k, k) = Y00;
               branch.Y()(k + phases.size(), k + phases.size()) = Y11;
               branch.Y()(k, k + phases.size()) = Y01;
               branch.Y()(k + phases.size(), k) = Y10;
            }
            branch.Y() /= scaleY;

            netw.addBranch(branch);
         }
      }
   }
}
