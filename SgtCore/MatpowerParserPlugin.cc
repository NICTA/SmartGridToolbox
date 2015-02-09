#include "MatpowerParserPlugin.h"

#include "Bus.h"
#include "Branch.h"
#include "CommonBranch.h"
#include "Gen.h"
#include "Network.h"
#include "Zip.h"
#include "YamlSupport.h"

#include <fstream>
#include <map>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_function.hpp>
#include <boost/spirit/include/phoenix_statement.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/include/support_multi_pass.hpp>

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
namespace phoenix = boost::phoenix;

typedef std::istreambuf_iterator<char> BaseIterator;
typedef boost::spirit::multi_pass<BaseIterator> ForwardIterator;

using namespace SmartGridToolbox;

namespace
{
   typedef ForwardIterator Iterator;
   typedef decltype(ascii::blank) SpaceType;
   typedef std::vector<std::vector<double>> Matrix;
   typedef Matrix::value_type Row;

   struct MpData
   {
      double MVABase;
      Matrix bus;
      Matrix gen;
      Matrix branch;
      Matrix genCost;
   };

   struct Gram : qi::grammar<Iterator, SpaceType>
   {
      Gram(MpData& data) : Gram::base_type(start_)
      {
         statementTerm_ = (qi::eol | qi::lit(';'));

         eol_ = -(qi::lit('%') >> *(qi::char_-qi::eol)) >> qi::eol;
         ignore_ = -(qi::lit('%') >> *(qi::char_-qi::eol)) >> qi::eol;
         other_ = (*(qi::char_-statementTerm_) >> statementTerm_);
      
         rowSep_ = (qi::lit(';') >> -eol_) | eol_;
         row_ = +(qi::double_ >> -qi::lit(','));
         matrix_ = "[" >> -eol_ >> (row_ % rowSep_) >> -(eol_ ^ qi::lit(';')) >> "]";

         topFunction_ = qi::lit("function") >> qi::lit("mpc") >> qi::lit("=") >> *(qi::char_-statementTerm_)
            >> statementTerm_;
         MVABase_ = (qi::lit("mpc.baseMVA") >> qi::lit('=') >> qi::double_ >> statementTerm_)
            [phoenix::ref(data.MVABase) = qi::_1];
         busMatrix_ = (qi::lit("mpc.bus") >> qi::lit('=') >> matrix_ >> statementTerm_)
            [phoenix::ref(data.bus) = qi::_1];
         genMatrix_ = (qi::lit("mpc.gen") >> qi::lit('=') >> matrix_ >> statementTerm_)
            [phoenix::ref(data.gen) = qi::_1];
         branchMatrix_ = (qi::lit("mpc.branch") >> qi::lit('=') >> matrix_ >> statementTerm_)
            [phoenix::ref(data.branch) = qi::_1];
         genCostMatrix_ = (qi::lit("mpc.gencost") >> qi::lit('=') >> matrix_ >> statementTerm_)
            [phoenix::ref(data.genCost) = qi::_1];

         start_ = *ignore_ >> topFunction_ >> *(ignore_ | MVABase_ | busMatrix_ | genMatrix_ | branchMatrix_ |
               genCostMatrix_ | other_);

         // To debug: e.g.
         // BOOST_SPIRIT_DEBUG_NODE(busMatrix_); debug(busMatrix_);
      }

      qi::rule<Iterator, SpaceType> statementTerm_;

      qi::rule<Iterator, SpaceType> eol_;
      qi::rule<Iterator, SpaceType> ignore_;
      qi::rule<Iterator, SpaceType> other_;

      // Matrix parser.
      qi::rule<Iterator, SpaceType> rowSep_;
      qi::rule<Iterator, Row(), SpaceType> row_;
      qi::rule<Iterator, Matrix(), SpaceType> matrix_;

      qi::rule<Iterator, SpaceType> topFunction_;
      qi::rule<Iterator, SpaceType> MVABase_;
      qi::rule<Iterator, SpaceType> busMatrix_;
      qi::rule<Iterator, SpaceType> genMatrix_;
      qi::rule<Iterator, SpaceType> branchMatrix_;
      qi::rule<Iterator, SpaceType> genCostMatrix_;

      qi::rule<Iterator, SpaceType> start_;
   };
   
   // Voltage is in p.u., with the base being specified in KV by the KVBase field.
   template<typename T> T pu2kV(T V, double kVBase)
   {
      return V * kVBase;
   }
   
   // Shunt admittance is in MW @ 1 p.u. voltage.
   template<typename T> T YBusShunt2Siemens(T Y, double kVBase)
   {
      return Y / (kVBase * kVBase); 
   }
   
   // Branch admittance is in p.u., that is, KVBase^2/MVABase.
   // KVBase is referenced to the "from" bus, the second bus, bus1.
   template<typename T> T YBranch2Siemens(T Y, double kVBase, double MVABase)
   {
      return Y * MVABase / (kVBase * kVBase); 
   }
  
   double deg2Rad(double deg)
   {
      return pi * deg / 180.0;
   }
}

namespace SmartGridToolbox
{

   struct MpBusInfo
   {
      int id; // C1, Not nec. consecutive.
      int type; // C2, 1 = PQ, 2 = PV, 3 = SL, 4 = isolated.
      double Pd; // C3, MW
      double Qd; // C4, MVAr
      double Gs; // C5, MW demand @ V = 1 pu
      double Bs; // C6, MVAr demand @ V = 1 pu
      int busArea; // C7
      double VMag; // C8, pu
      double VAngDeg; // C9, pu
      double kVBase; // C10, kV
      int zone; // C11
      double VMagMax; // C12, pu
      double VMagMin; // C13, pu
      double lam_P; // C14, u/pu
      double lam_Q; // C15, u/pu
      double mu_VMax; // C16, u/pu
      double mu_VMin; // C17, u/pu
   };

   struct MpBranchInfo
   {
      int busIdF;
      int busIdT;
      double R;
      double X;
      double b;
      double rateA;
      double rateB;
      double rateC;
      double tap;
      double shiftDeg;
      int status;
      double angMinDeg;
      double angMaxDeg;
   };

   struct MpGenInfo
   {
      int busId;
      double Pg; // MW
      double Qg; // MVAr
      double QMax; // MVAr
      double QMin; // MVAr
      double Vg; // pu
      double MVABase; // MVA
      int status;
      double PMax; // MVA
      double PMin; // MVA
   };

   struct MpGenCostInfo
   {
      int model;
      double startup;
      double shutdown;
      std::vector<double> costs;
   };

   namespace
   {
      /*
      std::string num2PaddedString(int num)
      {
         std::ostringstream ss;
         ss << std::setfill('0') << std::setw(5) << num;
         return ss.str();
      }
      */

      std::string getBusId(int id)
      {
         // return "bus_" + num2PaddedString(id);
         return "bus_" + std::to_string(id);
      }

      std::string getZipId(int iZip, int iBus)
      {
         // return "zip_" + num2PaddedString(iZip) + "_" + num2PaddedString(iBus);
         return "zip_" + std::to_string(iZip) + "_" + std::to_string(iBus);
      }
      
      std::string getGenId(int iGen, int iBus)
      {
         // return "gen_" + num2PaddedString(iGen) + "_" + num2PaddedString(iBus);
         return "gen_" + std::to_string(iGen) + "_" + std::to_string(iBus);
      }

      std::string getBranchId(int iBranch, int iBus0, int iBus1)
      {
         // return "branch_" + num2PaddedString(iBranch) + "_" + num2PaddedString(iBus0)
         //   + "_" + num2PaddedString(iBus1);
         return "branch_" + std::to_string(iBranch) + "_" + std::to_string(iBus0)
            + "_" + std::to_string(iBus1);
      }
   }

   void MatpowerParserPlugin::parse(const YAML::Node& nd, Network& netw, const ParserBase& parser) const
   {
      assertFieldPresent(nd, "input_file");
      assertFieldPresent(nd, "default_kV_base");

      std::string inputName = parser.expand<std::string>(nd["input_file"]);

      double default_kVBase = parser.expand<double>(nd["default_kV_base"]);
     
      double VScale = 1.0; 
      auto ndVScale = nd["scale_V_by"];
      if (ndVScale)
      {
         VScale = ndVScale.as<double>();
      }
      double PScale = 1.0; 
      auto ndPScale = nd["scale_P_by"];
      if (ndPScale)
      {
         PScale = ndPScale.as<double>();
      }
      double RScale = VScale * VScale / PScale;
      double GScale = 1.0 / RScale;

      // Parse in the raw matpower data.
      MpData data;
      
      {
         Gram gram(data);

         std::fstream infile(inputName);
         if (!infile.is_open())
         {
            Log().fatal() << "Could not open the matpower input file " << inputName << "." << std::endl;
         }
         else
         {
            Log().message() << "Parsing matpower input file " << inputName << "." << std::endl;
         }

         // Iterate over stream input:
         BaseIterator inBegin(infile);

         ForwardIterator fwdBegin = boost::spirit::make_default_multi_pass(inBegin);
         ForwardIterator fwdEnd;

         while (fwdBegin != fwdEnd)
         {
            qi::phrase_parse(fwdBegin, fwdEnd, gram, ascii::blank);
         }
      }

      Log().message() << "Parsed " << data.bus.size() << " busses, " << data.gen.size() << " generators and "
         << data.branch.size() << " branches." << std::endl;

      // Extract the bus data.
      std::vector<MpBusInfo> busVec;
      {
         busVec.reserve(data.bus.size());
         for (const auto& row : data.bus)
         {
            busVec.push_back(MpBusInfo{});
            MpBusInfo& busInfo = busVec.back();

            busInfo.id = row[0];
            busInfo.type = row[1];
            busInfo.Pd = row[2];
            busInfo.Qd = row[3];
            busInfo.Gs = row[4];
            busInfo.Bs = row[5];
            busInfo.busArea = row[6];
            busInfo.VMag = row[7];
            busInfo.VAngDeg = row[8];
            busInfo.kVBase = row[9] > 1e-6 ? row[9] : default_kVBase;
            busInfo.zone = row[10];
            busInfo.VMagMax = row[11];
            busInfo.VMagMin = row[12];
            if (data.bus[0].size() > 13)
            {
               busInfo.lam_P = row[13];
               busInfo.lam_Q = row[14];
               busInfo.mu_VMax = row[15];
               busInfo.mu_VMin = row[16];
            }
         }
      }

      // Extract the gen data.
      std::vector<MpGenInfo> genVec;
      {
         genVec.reserve(data.gen.size());
         for (const auto& row : data.gen)
         {
            genVec.push_back(MpGenInfo{});
            MpGenInfo& genInfo = genVec.back();

            genInfo.busId = row[0];
            genInfo.Pg = row[1];
            genInfo.Qg = row[2];
            genInfo.QMax = row[3];
            genInfo.QMin = row[4];
            genInfo.Vg = row[5];
            genInfo.MVABase = row[6];
            genInfo.status = row[7];
            genInfo.PMax = row[8];
            genInfo.PMin = row[9];
            // There are other fields, but at present we are ignoring them.
         }
      }
      
      // Extract the branch data.
      std::vector<MpBranchInfo> branchVec;
      {
         branchVec.reserve(data.branch.size());
         for (const auto& row : data.branch)
         {
            branchVec.push_back(MpBranchInfo{});
            MpBranchInfo& branchInfo = branchVec.back();

            branchInfo.busIdF = row[0];
            branchInfo.busIdT = row[1];
            branchInfo.R = row[2];
            branchInfo.X = row[3];
            branchInfo.b = row[4];
            branchInfo.rateA = row[5];
            branchInfo.rateB = row[6];
            branchInfo.rateC = row[7];
            branchInfo.tap = row[8];
            branchInfo.shiftDeg = row[9];
            branchInfo.status = row[10];
            branchInfo.angMinDeg = row[11];
            branchInfo.angMaxDeg = row[12];
         }
      }
      
      // Extract the genCost data.
      if (data.genCost.size() > 0)
      {
         if (data.genCost.size() == 2 * data.gen.size())
         {
            Log().warning() << "Reactive generator costs not yet implemented. Ignoring them." << std::endl;
         }
         else if (data.genCost.size() != data.gen.size())
         {
            Log().fatal() << "There are a different number of generators to generator costs." << std::endl;
         }
      }

      std::vector<MpGenCostInfo> genCostVec;
      {
         genCostVec.reserve(data.gen.size());
         if (data.genCost.size() > 0)
         {
            for (int i = 0; i < data.gen.size(); ++i)
            {
               auto row = data.genCost[i];
               genCostVec.push_back(MpGenCostInfo{});
               MpGenCostInfo& genCostInfo = genCostVec.back();

               genCostInfo.model = row[0];
               genCostInfo.startup = row[1];
               genCostInfo.shutdown = row[2];
               int nCost = row[3];
               genCostInfo.costs.reserve(nCost);
               for (int i = 0; i < nCost; ++i)
               {
                  genCostInfo.costs.push_back(row[4 + i]);
               }
            }
         }
      }
      
      // Network:
      netw.setPBase(data.MVABase);

      // Busses:
      int nZip = 0;
      for (const auto& busInfo : busVec)
      {
         std::string busId = getBusId(busInfo.id);
         std::unique_ptr<Bus> bus(
               new Bus(busId, Phase::BAL, {VScale * Complex(busInfo.kVBase, 0.0)}, PScale * busInfo.kVBase));
         BusType type = BusType::BAD;
         switch (busInfo.type)
         {
            case 1:
               type = BusType::PQ;
               break;
            case 2:
               type = BusType::PV;
               break;
            case 3:
               type = BusType::SL;
               break;
            default:
               break;
         }
         bus->setType(type);
         
         Complex SZip = -Complex(busInfo.Pd, busInfo.Qd); // Already in MW.
         Complex YZip = YBusShunt2Siemens(Complex(busInfo.Gs, busInfo.Bs), busInfo.kVBase);
         std::string zipId = getZipId(nZip++, busInfo.id);
         std::unique_ptr<GenericZip> zip(new GenericZip(zipId, Phase::BAL));
         zip->setYConst({GScale * YZip});
         zip->setSConst({PScale * SZip});
         bus->setVMagMin(busInfo.VMagMin == -infinity ? -infinity : VScale * pu2kV(busInfo.VMagMin, busInfo.kVBase));
         bus->setVMagMax(busInfo.VMagMax == infinity ? infinity : VScale * pu2kV(busInfo.VMagMax, busInfo.kVBase));

         bus->setIsInService(true);

         double VMag = pu2kV(busInfo.VMag, busInfo.kVBase);
         double VAng = deg2Rad(busInfo.VAngDeg);
         bus->setV({VScale * std::polar(VMag, VAng)});
         
         netw.addBus(std::move(bus));
         netw.addZip(std::move(zip), busId);
      } // Busses

      // If there is no slack bus, Matpower assigns the first PV bus as slack. We need to do the same.
      auto it = std::find_if(netw.busses().cbegin(), netw.busses().cend(), 
            [](const BusPtr& bus)->bool{return bus->type() == BusType::SL;});
      if (it == netw.busses().cend())
      {
         Log().warning() 
            << "There is no slack bus defined on the network. Setting the type of the first PV bus to SL." 
            << std::endl;
         auto itb = std::find_if(netw.busses().cbegin(), netw.busses().cend(), 
               [](const BusPtr& bus)->bool{return bus->type() == BusType::PV;});
         assert(itb != netw.busses().cend());
         (**itb).setType(BusType::SL);
      }
      else
      {
         Log().message() << "The slack bus is " << (**it).id() << std::endl;
      }

      // Gens:
      std::vector<GenericGen*> genCompVec;
      genCompVec.reserve(genVec.size());
      for (int i = 0; i < genVec.size(); ++i) 
      {
         MpGenInfo& genInfo = genVec[i];
         std::string genId = getGenId(i, genInfo.busId);
         std::unique_ptr<GenericGen> gen(new GenericGen(genId, Phase::BAL));
         genCompVec.push_back(gen.get());
         
         std::string busId = getBusId(genInfo.busId);

         gen->setIsInService(genInfo.status);

         gen->setInServiceS({PScale * Complex(genInfo.Pg, genInfo.Qg)});

         gen->setPMin(genInfo.PMin == -infinity ? -infinity : PScale * genInfo.PMin);
         gen->setPMax(genInfo.PMax == infinity ? infinity : PScale * genInfo.PMax);
         gen->setQMin(genInfo.QMin == -infinity ? -infinity : PScale * genInfo.QMin);
         gen->setQMax(genInfo.QMax == infinity ? infinity : PScale * genInfo.QMax);

         gen->setC0(0.0);
         gen->setC1(0.0);
         gen->setC2(0.0);

         auto bus = netw.bus(busId);
         bus->setVMagSetpoint({VScale * pu2kV(genInfo.Vg, bus->VBase())});

         netw.addGen(std::move(gen), busId);
      } // Gens

      // Branches:
      for (int i = 0; i < branchVec.size(); ++i)
      {
         const MpBranchInfo& branchInfo = branchVec[i];

         std::string branchId = getBranchId(i, branchInfo.busIdF, branchInfo.busIdT);
         std::unique_ptr<CommonBranch> branch(new CommonBranch(branchId));

         branch->setIsInService(branchInfo.status);

         std::string bus0Id = getBusId(branchInfo.busIdF);
         std::string bus1Id = getBusId(branchInfo.busIdT);

         auto bus0 = netw.bus(bus0Id);
         auto bus1 = netw.bus(bus1Id);

         double tap = (std::abs(branchInfo.tap) < 1e-6 ? 1.0 : branchInfo.tap) * bus0->VBase() / bus1->VBase();
         branch->setTapRatio(std::polar(tap, deg2Rad(branchInfo.shiftDeg)));
         branch->setYSeries(GScale * YBranch2Siemens(1.0 / Complex(branchInfo.R, branchInfo.X), bus1->VBase(),
                  data.MVABase));
         branch->setYShunt(GScale * YBranch2Siemens(Complex(0.0, branchInfo.b), bus1->VBase(), data.MVABase));
         branch->setRateA(PScale * branchInfo.rateA);
         branch->setRateB(PScale * branchInfo.rateB);
         branch->setRateC(PScale * branchInfo.rateC);

         netw.addBranch(std::move(branch), bus0Id, bus1Id);
      }

      // Add generator costs, if they exist.
      for (int i = 0; i < genCostVec.size(); ++i)
      {
         const MpGenCostInfo& genCostInfo = genCostVec[i];

         bool isBad = false;

         if (genCostInfo.costs.size() > 3)
         {
            Log().warning() << "Can't have more than three costs for generator. Ignoring costs." << std::endl;
            isBad = true;
         }

         if (genCostInfo.model != 2)
         {
            Log().warning() << "Can only use model 2 for generator costs. Ignoring costs." << std::endl;
            isBad = true;
         }

         if (!isBad)
         {
            GenericGen& gen = *genCompVec[i];
            gen.setCStartup(genCostInfo.startup);
            gen.setCShutdown(genCostInfo.shutdown);
            int nCost = genCostInfo.costs.size();
            if (nCost >= 1)
            {
               gen.setC0(genCostInfo.costs[2]);
            }
            if (nCost >= 2)
            {
               gen.setC1(genCostInfo.costs[1]);
            }
            if (nCost >= 3)
            {
               gen.setC2(genCostInfo.costs[0]);
            }
         }
      }
   } // parse(...)
}
