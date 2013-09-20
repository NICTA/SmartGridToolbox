#include "MatpowerParser.h"
#include "Network.h"

#include <fstream>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_function.hpp>
#include <boost/spirit/include/phoenix_statement.hpp>
#include <boost/spirit/include/phoenix_bind.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/include/support_multi_pass.hpp>

namespace Qi = boost::spirit::qi;
namespace Ascii = boost::spirit::ascii;
namespace Phoenix = boost::phoenix;

using Ascii::blank;
using Phoenix::bind;
using Qi::char_;
using Qi::double_;
using Qi::eol;
using Qi::eps;
using Qi::grammar;
using Qi::lexeme;
using Qi::lit;
using Qi::phrase_parse;
using Qi::rule;

typedef std::istreambuf_iterator<char> BaseIterator;
typedef boost::spirit::multi_pass<BaseIterator> ForwardIterator;

namespace SmartGridToolbox
{
   template<typename Iterator, typename SpaceType> struct Gram : grammar<Iterator, SpaceType>
   {
      rule<Iterator, SpaceType> statementTerm_;
      rule<Iterator, SpaceType> colSep_;
      rule<Iterator, SpaceType> rowSep_;
      rule<Iterator, std::vector<double>, SpaceType> row_;
      rule<Iterator, std::vector<std::vector<double>>(), SpaceType> matrix_;
      rule<Iterator, SpaceType> topFunction_;
      rule<Iterator, SpaceType> comment_;
      rule<Iterator, double(), SpaceType> baseMVA_;
      rule<Iterator, SpaceType> ignore_;
      rule<Iterator, SpaceType> start_;

      double baseMVAVal_;

      Gram() : Gram::base_type(start_)
      {
         statementTerm_ = (eol | lit(';'));
         colSep_ = lexeme[*blank >> +(lit(',') | blank) >> *blank]; 
         rowSep_ = eol | lit(';');
         row_ = double_ % colSep_;
         matrix_ = lit('[') >> *(row_ % rowSep_) >> lit(']'); 
         topFunction_ = lit("function") >> lit("mpc") >> lit("=") >> double_ >> statementTerm_;
         comment_ = lit('%') >> *char_ >> eol;
         baseMVA_ = lit("mpc.baseMVA") >> lit('=') >> double_ >> statementTerm_;
         ignore_ = *char_ >> statementTerm_;
         start_ = eps[bind(&Gram::init, this)] >> *comment_ >> topFunction_ >> *(comment_ | baseMVA_ | ignore_);
      }

      void init() {baseMVAVal_ = 0.0;}
      void setBaseMVA(double val) 
      {
         SGT_DEBUG(debug() << "Matpower file : baseMVA = " << baseMVA_ << std::endl);
         baseMVAVal_ = val;
      }
   };

   void MatpowerParser::parse(const YAML::Node & nd, Model & mod, const ParserState & state) const
   {
      SGT_DEBUG(debug() << "Matpower : parse." << std::endl);

      assertFieldPresent(nd, "input_file");
      assertFieldPresent(nd, "network_name");

      string inputName = state.expandName(nd["input_file"].as<std::string>());
      string networkName = state.expandName(nd["network_name"].as<std::string>());

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

      Network & comp = mod.newComponent<Network>(networkName);

      while (fwdBegin != fwdEnd)
      {
         Gram<ForwardIterator, decltype(blank)> gram;
         bool ok = phrase_parse(fwdBegin, fwdEnd, gram, blank);
      }
   }

   void MatpowerParser::postParse(const YAML::Node & nd, Model & mod, const ParserState & state) const
   {

   }

}
