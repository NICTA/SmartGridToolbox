#include "MatpowerParser.h"
#include "Bus.h"
#include "Network.h"
#include "PowerFlow.h"
#include "ZipToGround.h"

#include <fstream>
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
      Gram(double & baseMVAVal,
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
         baseMVA_ = (Qi::lit("mpc.baseMVA") >> Qi::lit('=') >> Qi::double_ >> statementTerm_)
                    [Phoenix::ref(baseMVAVal) = Qi::_1];
         busMatrix_ = (Qi::lit("mpc.bus") >> Qi::lit('=') >> matrix_ >> statementTerm_)
            [Phoenix::ref(busMatrixVal) = Qi::_1];
         genMatrix_ = (Qi::lit("mpc.gen") >> Qi::lit('=') >> matrix_ >> statementTerm_)
            [Phoenix::ref(genMatrixVal) = Qi::_1];
         branchMatrix_ = (Qi::lit("mpc.branch") >> Qi::lit('=') >> matrix_ >> statementTerm_)
            [Phoenix::ref(branchMatrixVal) = Qi::_1];

         start_ =  *ignore_ >> topFunction_ >> *(ignore_ | baseMVA_ | busMatrix_ | genMatrix_ | branchMatrix_ | other_);

         // To debug: e.g.
         // BOOST_SPIRIT_DEBUG_NODE(baseMVA_); 
         // debug(baseMVA_); 
      }

      Qi::rule<Iterator, SpaceType> statementTerm_;

      Qi::rule<Iterator, SpaceType> ignore_;
      Qi::rule<Iterator, SpaceType> other_;

      Qi::rule<Iterator, std::vector<double>(), SpaceType> matrix_;

      Qi::rule<Iterator, SpaceType> topFunction_;
      Qi::rule<Iterator, SpaceType> baseMVA_;
      Qi::rule<Iterator, SpaceType> busMatrix_;
      Qi::rule<Iterator, SpaceType> genMatrix_;
      Qi::rule<Iterator, SpaceType> branchMatrix_;

      Qi::rule<Iterator, SpaceType> start_;
   };

   static std::string busName(const std::string & prefix, int id)
   {
      return prefix + "_bus_" + std::to_string(id);
   }

   static std::string loadName(const std::string & prefix, int id)
   {
      return prefix + "_load_" + std::to_string(id);
   }

   static std::string genName(const std::string & prefix, int id)
   {
      return prefix + "_gen_" + std::to_string(id);
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
      assertFieldPresent(nd, "area_base_V");

      std::string inputName = state.expandName(nd["input_file"].as<std::string>());
      std::string networkName = state.expandName(nd["network_name"].as<std::string>());

      std::map<int, double> areaBaseV;
      YAML::Node ndBaseV = nd["area_base_V"];
      for (int i = 0; i < ndBaseV.size(); ++i)
      {
         areaBaseV[ndBaseV[0][0].as<int>()] = ndBaseV[0][1].as<double>();
      }

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

      double baseMVA;
      std::vector<double> busMatrix;
      std::vector<double> genMatrix;
      std::vector<double> branchMatrix;
      {
         Gram gram(baseMVA, busMatrix, genMatrix, branchMatrix);
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

      double baseS = 1e6 * baseMVA;

      Network & netw = mod.newComponent<Network>(networkName);

      for (int i = 0; i < nBus; ++i)
      {
         int busId      = busMatrix[busCols * i];
         int mpType     = busMatrix[busCols * i + 1];
         double Pd      = busMatrix[busCols * i + 2];
         double Qd      = busMatrix[busCols * i + 3];
         double Gs      = busMatrix[busCols * i + 4];
         double Bs      = busMatrix[busCols * i + 5];
         int area       = busMatrix[busCols * i + 6];
         double baseKV  = busMatrix[busCols * i + 9];

         double baseV = (baseKV == 0) ? areaBaseV[area] : 1e3 * baseKV;
         double basey = baseS / (baseV * baseV);

         Complex Sd = Complex{Pd, Qd} * baseS;
         Complex Ys = Complex{Gs, Bs} * basey; 

         BusType type = BusType::BAD;
         switch (mpType)
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
               error() << "Bad matpower bus type (type = " << mpType << ") encountered." << std::endl;
               abort();
               break;
         }

         UblasVector<Complex> nomVVec(phases.size(), baseV);
         UblasVector<Complex> SVec(phases.size(), -Sd); // Note: load = -ve injection.
         UblasVector<Complex> YsVec(phases.size(), Ys);

         Bus & bus = mod.newComponent<Bus>(busName(networkName, busId), type, phases, nomVVec);

         ZipToGround & load = mod.newComponent<ZipToGround>(loadName(networkName, busId), phases);
         load.S() = SVec;
         load.Y() = YsVec;
         bus.addZipToGround(load);

         netw.addBus(bus);
      }

      for (int i = 0; i < nGen; ++i)
      {
         int busId  = genMatrix[genCols * i];
         double Pg  = genMatrix[genCols * i + 1];
         double Qg  = genMatrix[genCols * i + 2];
         double baseMVA  = genMatrix[genCols * i + 6];

         double baseS = 1e6 * baseMVA;

         Complex Sg = Complex{Pg, Qg} * baseS;
         UblasVector<Complex> SVec(phases.size(), Sg);

         Bus * bus = mod.componentNamed<Bus>(busName(networkName, busId));
         if (bus == nullptr)
         {
            error() << "Matpower: for generator " << i << ", bus " << busId << " was not found." << std::endl;
            abort();
         }

         ZipToGround & gen = mod.newComponent<ZipToGround>(genName(networkName, busId), phases);
         gen.S() = SVec;
         bus->addZipToGround(gen);
      }

      for (int i = 0; i < nBranch; ++i)
      {
         int bus0Id  = branchMatrix[branchCols * i];
         int bus1Id  = branchMatrix[branchCols * i + 1];
         double Rs  = branchMatrix[branchCols * i + 2];
         double Xs  = branchMatrix[branchCols * i + 3];
         double Bc  = branchMatrix[branchCols * i + 4];
         double tau  = branchMatrix[branchCols * i + 8];
         double theta  = branchMatrix[branchCols * i + 9] * pi / 180.0; // Matpower format is in deg, convert to rad.

         if (tau == 0.0)
         {
            tau = 1.0; // I think 0 means "default".
         }

         if (tau != 1.0 || theta != 0.0)
         {
            warning() << "Matpower: the tau and theta branch parameters have yet to be tested, " 
                         "and are likely to be wrong." << std::endl;
         }

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

         double baseV = bus0->nominalV()(0).real();
         assert(baseV = bus1->nominalV()(0).real()); // TODO: Otherwise, how is p.u. defined?
         double basey = baseS / (baseV * baseV);

         Complex ys = basey / Complex{Rs, Xs};

         Complex Y11 = (ys + Complex{0, 0.5 * basey * Bc});
         Complex Y00 = Y11 / (tau * tau);
         Complex Y01 = -(ys / tau) * Complex{cos(theta), sin(theta)};
         Complex Y10 = -(ys / tau) * Complex{cos(theta), -sin(theta)};

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

         netw.addBranch(branch);
      }

   }

   void MatpowerParser::postParse(const YAML::Node & nd, Model & mod, const ParserState & state) const
   {

   }

}
