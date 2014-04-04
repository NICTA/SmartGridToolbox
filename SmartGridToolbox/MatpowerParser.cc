#include "MatpowerParser.h"

#include <SmartGridToolbox/Bus.h>
#include <SmartGridToolbox/Network.h>
#include <SmartGridToolbox/PowerFlow.h>
#include <SmartGridToolbox/ZipToGround.h>

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
   struct MpBusInfo
   {
      int mpType;

      double VBase;  // SI.
      Complex VPu;   // Per unit.
      Complex SLoad; // SI.
      Complex Sg;    // SI.
      Complex YsPu;  // Per unit.
   };

   static std::string num2PaddedString(int num)
   {
      std::ostringstream ss;
      ss << std::setfill('0') << std::setw(5) << num;
      return ss.str();
   }

   static std::string busName(const std::string & prefix, int id)
   {
      return prefix + "_bus_" + num2PaddedString(id);
   }

   static std::string zipName(const std::string & prefix, int id)
   {
      return prefix + "_zip_" + num2PaddedString(id);
   }

   static std::string branchName(const std::string & prefix, int nBranch, int id1, int id2)
   {
      return prefix + "_branch_" + num2PaddedString(nBranch) + "_" + num2PaddedString(id1)
                    + "_" + num2PaddedString(id2);
   }

   void MatpowerParser::parse(const YAML::Node & nd, Model & mod, const ParserState & state) const
   {
      // Matpower input units:
      // Power is in MW.
      // Voltage is in p.u., with the base being specified in KV by the KVBase field.
      // Branch admittance is in p.u., that is, MVABase/(KVBase)^2.
      // Shunt admittance is in MW/(KVBase)^2.

      // Power in SI = input*1e6.
      // Voltage in SI = input*1000*KVBase.
      // Branch admittance in SI = input*1e6*MVABase/KVBase^2.
      // Shunt admittance in SI = input*1e6/KVBase^2.

      // Matpower input to internal units (p.u.):
      // Power is p.u. : input value divided by MVABase.
      // Voltage remains in p.u. : input value.
      // Branch admittance remains in p.u. : input value.
      // Shunt admittance is p.u. : input value divided by MVABase.

      // SI to matpower internal:
      // P_mp_intl = P_SI/(1e6*MVABase)
      // V_mp_intl = V_SI/(1000*KVBase)
      // Y_mp_intl = Y_SI*MVABase/KVBase^2

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
      double SBase = 1e6*MVABase;

      const int busCols = 13;
      const int genCols = 21;
      const int branchCols = 13;

      int nBus = busMatrix.size()/busCols;
      int nGen = genMatrix.size()/genCols;
      int nBranch = branchMatrix.size()/branchCols;

      SGT_DEBUG(debug() << "Matpower: Bus matrix size = " << busCols << std::endl);
      SGT_DEBUG(debug() << "Matpower: Gen matrix size = " << genCols << std::endl);
      SGT_DEBUG(debug() << "Matpower: Branch matrix size = " << branchCols << std::endl);

      // Organize and scale the data:
      std::map<int, MpBusInfo> busMap;
      {
         for (int i = 0; i < nBus; ++i)
         {
            int busId = busMatrix[busCols*i];
            MpBusInfo & busInfo = busMap[busId];

            busInfo.mpType = busMatrix[busCols*i + 1];

            double KVBase = busMatrix[busCols*i + 9];
            busInfo.VBase = KVBase == 0 ? defaultVBase : KVBase*1000;

            double Vm = busMatrix[busCols*i + 7];
            double Va = busMatrix[busCols*i + 8];
            busInfo.VPu = polar(Vm, Va*pi/180);

            double Pd = busMatrix[busCols*i + 2];
            double Qd = busMatrix[busCols*i + 3];
            busInfo.SLoad = -Complex(Pd, Qd)*1e6; // Injection is -ve load.

            double Gs = busMatrix[busCols*i + 4];
            double Bs = busMatrix[busCols*i + 5];
            busInfo.YsPu = Complex(Gs, Bs)*1e6/SBase; // Matpower has Ys in MW/VBase^2
         }

         for (int i = 0; i < nGen; ++i)
         {
            int busId  = genMatrix[genCols*i];
            MpBusInfo & busInfo = busMap[busId];

            double Pg  = genMatrix[genCols*i + 1];
            double Qg  = genMatrix[genCols*i + 2];
            double Vg  = genMatrix[genCols*i + 5];

            busInfo.Sg = Complex(Pg, Qg)*1e6; // Assuming 1 generator max per bus.

            busInfo.VPu *= Vg/abs(busInfo.VPu); // Scale V of bus to match specified generator V.
         }
      }

      // Add new components.
      {
         Network & netw = mod.newComponent<Network>(networkName, freq);
         for (const auto pair : busMap)
         {
            const int & busId = pair.first;
            const MpBusInfo & info = pair.second;
            BusType type = BusType::BAD;
            switch (info.mpType)
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
                  error() << "Bad matpower bus type (type = " << info.mpType << ") encountered." << std::endl;
                  abort();
                  break;
            }

            ublas::vector<Complex> VNomVec(phases.size(), info.VBase);

            Complex V = usePerUnit ? info.VPu : info.VPu*info.VBase;
            ublas::vector<Complex> VVec(phases.size(), V);

            double VMag = usePerUnit ? abs(info.VPu) : abs(info.VPu*info.VBase);
            ublas::vector<double> VMagVec(phases.size(), VMag);

            double VAng = usePerUnit ? arg(info.VPu) : arg(info.VPu*info.VBase);
            ublas::vector<double> VAngVec(phases.size(), VAng);

            Complex SLoad = usePerUnit ? info.SLoad/SBase : info.SLoad;
            ublas::vector<Complex> SLoadVec(phases.size(), SLoad);

            Complex Sg = usePerUnit ? info.Sg/SBase : info.Sg;
            ublas::vector<Complex> SgVec(phases.size(), Sg);

            Complex Ys = usePerUnit ? info.YsPu : info.YsPu*SBase/(info.VBase*info.VBase);
            ublas::vector<Complex> YsVec(phases.size(), Ys);

            // TODO: bounds on values.

            Bus & bus = mod.newComponent<Bus>(busName(networkName, busId), type, phases, VNomVec);

            bus.setPgSetpoint(real(SgVec));
            bus.setQgSetpoint(imag(SgVec));
            bus.setVMagSetpoint(VMagVec);
            bus.setVAngSetpoint(VAngVec);

            bus.setV(VVec);
            bus.setSg(SgVec);

            ZipToGround & zip = mod.newComponent<ZipToGround>(zipName(networkName, busId), phases);
            zip.S() = SLoadVec;
            zip.Y() = YsVec;
            bus.addZipToGround(zip);

            netw.addBus(bus);
         }

         for (int i = 0; i < nBranch; ++i)
         {
            int bus0Id = branchMatrix[branchCols*i];
            int bus1Id = branchMatrix[branchCols*i + 1];

            const MpBusInfo & busInfo0 = busMap[bus0Id];
            const MpBusInfo & busInfo1 = busMap[bus1Id];

            double VBase0 = busInfo0.VBase;
            double VBase1 = busInfo1.VBase;

            double Rs = branchMatrix[branchCols*i + 2];
            double Xs = branchMatrix[branchCols*i + 3];
            double Bc = branchMatrix[branchCols*i + 4];
            double tap = branchMatrix[branchCols*i + 8];
            double theta = branchMatrix[branchCols*i + 9]; // Matpower format is in deg, convert to rad.

            if (tap == 0.0)
            {
               tap = 1.0; // 0 means "default", i.e. 1.
            }

            Complex cTap = polar(tap, theta*pi/180);

            Bus* bus0 = mod.component<Bus>(busName(networkName, bus0Id));
            if (bus0 == nullptr)
            {
               error() << "Matpower: for branch " << i << ", from bus " << bus0Id << " was not found." << std::endl;
               abort();
            }
            Bus* bus1 = mod.component<Bus>(busName(networkName, bus1Id));
            if (bus1 == nullptr)
            {
               error() << "Matpower: for branch " << i << ", to bus " << bus1Id << " was not found." << std::endl;
               abort();
            }

            Complex Ys = 1.0/Complex{Rs, Xs};

            Complex Y11 = (Ys + Complex{0, 0.5*Bc});
            Complex Y00 = Y11/(tap*tap);
            Complex Y01 = -(Ys/conj(cTap));
            Complex Y10 = -(Ys/cTap);
            if (!usePerUnit)
            {
               // TODO: scaling across a transformer, looks fishy. Careful.
               Y00 *= SBase/(VBase0*VBase0);
               Y01 *= SBase/(VBase0*VBase1);
               Y10 *= SBase/(VBase1*VBase0);
               Y11 *= SBase/(VBase1*VBase1);
            }

            Branch & branch = mod.newComponent<Branch>(branchName(networkName, i, bus0Id, bus1Id), phases, phases);

            branch.setBus0(*bus0);
            branch.setBus1(*bus1);

            ublas::matrix<Complex> Y(2*phases.size(), 2*phases.size());
            for (int k = 0; k < phases.size(); ++k)
            {
               Y(k, k) = Y00;
               Y(k + phases.size(), k + phases.size()) = Y11;
               Y(k, k + phases.size()) = Y01;
               Y(k + phases.size(), k) = Y10;
            }
            branch.setY(Y);

            netw.addBranch(branch);
         }
      }
   }
}
