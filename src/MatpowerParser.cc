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
using Phoenix::ref;
using Qi::char_;
using Qi::double_;
using Qi::eol;
using Qi::eps;
using Qi::grammar;
using Qi::lexeme;
using Qi::lit;
using Qi::phrase_parse;
using Qi::rule;
using Qi::skip;
using Qi::_val;
using Qi::_1;
using Qi::_2;

typedef std::istreambuf_iterator<char> BaseIterator;
typedef boost::spirit::multi_pass<BaseIterator> ForwardIterator;

namespace SmartGridToolbox
{
   typedef ForwardIterator Iterator;
   typedef decltype(blank) SpaceType;

   void showMat(const std::vector<double> & from)
   {
      std::cout << "showMat " << from.size() << std::endl;
   }

   struct Gram : grammar<Iterator, SpaceType>
   {
      Gram(double & baseMVAVal, std::vector<double> & busMatrixVal) : Gram::base_type(start_)
      {
         statementTerm_ = (eol | lit(';'));

         ignore_ =   (eol) | (lit('%') >> *(char_-eol) >> eol);
         other_ = (*(char_-statementTerm_) >> statementTerm_);

         matrix_ = lit('[') >> -eol >> skip(blank|eol|char_(",;"))[*double_] >> -eol >> lit(']');

         topFunction_ = lit("function") >> lit("mpc") >> lit("=") >> *(char_-statementTerm_) >> statementTerm_;
         baseMVA_ = (lit("mpc.baseMVA") >> lit('=') >> double_ >> statementTerm_)
                    [Phoenix::ref(baseMVAVal) = _1];
         busMatrix_ = (lit("mpc.bus") >> lit('=') >> matrix_ >> statementTerm_)
                    [Phoenix::ref(busMatrixVal) = _1];

         start_ =  *ignore_ >> topFunction_ >> *(ignore_ | baseMVA_ | busMatrix_ | other_);

         // To debug: e.g.
         // BOOST_SPIRIT_DEBUG_NODE(baseMVA_); 
         // debug(baseMVA_); 
      }

      rule<Iterator, SpaceType> statementTerm_;
      rule<Iterator, SpaceType> colSep_;
      rule<Iterator, SpaceType> rowSep_;
      rule<Iterator, SpaceType> matSep_;

      rule<Iterator, SpaceType> ignore_;
      rule<Iterator, SpaceType> other_;

      rule<Iterator, std::vector<double>(), SpaceType> matrix_;

      rule<Iterator, SpaceType> topFunction_;
      rule<Iterator, SpaceType> baseMVA_;
      rule<Iterator, SpaceType> busMatrix_;

      rule<Iterator, SpaceType> start_;
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

      double baseMVAVal;
      std::vector<double> busMatrixVal;
      Gram gram(baseMVAVal, busMatrixVal);
      while (fwdBegin != fwdEnd)
      {
         bool ok = phrase_parse(fwdBegin, fwdEnd, gram, blank);
      }
      std::cout << "Bus matrix size = " << busMatrixVal.size() << std::endl;
   }

   void MatpowerParser::postParse(const YAML::Node & nd, Model & mod, const ParserState & state) const
   {

   }

}
