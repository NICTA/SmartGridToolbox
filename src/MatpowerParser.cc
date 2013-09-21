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
using Qi::_1;
using Qi::_2;

typedef std::istreambuf_iterator<char> BaseIterator;
typedef boost::spirit::multi_pass<BaseIterator> ForwardIterator;
typedef std::vector<double> Row;
typedef std::vector<Row> Matrix;

void showRow(const Row & v)
{
   std::cout << "show vec v " << v.size() << " " << v.front() << " ... " << v.back() << std::endl;
}
void showMat(const Matrix & m)
{
   std::cout << "show mat m " << m.size() << " " << m.front().front() << " ... " << m.back().back() << std::endl;
}

namespace SmartGridToolbox
{
   typedef ForwardIterator Iterator;
   typedef decltype(blank) SpaceType;

   struct Gram : grammar<Iterator, SpaceType>
   {
      rule<Iterator, SpaceType> statementTerm_;
      rule<Iterator, SpaceType> colSep_;
      rule<Iterator, SpaceType> rowSep_;

      rule<Iterator, SpaceType> blankLine_;
      rule<Iterator, SpaceType> comment_;
      rule<Iterator, SpaceType> ignore_;

      rule<Iterator, Row(), SpaceType> row_;
      rule<Iterator, Matrix(), SpaceType> matrix_;

      rule<Iterator, SpaceType> topFunction_;
      rule<Iterator, SpaceType> baseMVA_;
      rule<Iterator, SpaceType> busMatrix_;

      rule<Iterator, SpaceType> start_;

      double baseMVAVal_;
      Matrix busMatrixVal_;

      Gram() : Gram::base_type(start_)
      {
         statementTerm_ = (eol | lit(';'));
         colSep_ = lexeme[*blank >> +(lit(',') | blank) >> *blank]; 
         rowSep_ = eol | (lit(';') >> -eol);

         blankLine_ = eol;
         comment_ = lit('%') >> *(char_-eol) >> eol;
         ignore_ = *(char_-statementTerm_) >> statementTerm_;

         row_ = (double_ % *lit(','));//[&showVec];
         matrix_ = lit('[') >> -eol >> *(row_ % rowSep_) >> -rowSep_ >> lit(']');//[&showMat];

         topFunction_ = lit("function") >> lit("mpc") >> lit("=") >> *(char_-statementTerm_) >> statementTerm_;
         baseMVA_ = (lit("mpc.baseMVA") >> lit('=') >> double_ >> statementTerm_)
                    [bind(&Gram::setBaseMVA, this, _1)];
         busMatrix_ = (lit("mpc.bus") >> lit('=') >> matrix_ >> statementTerm_);
                      //[bind(&Gram::setBusMatrix, this, _1)];

         start_ = eps[bind(&Gram::init, this)] >> *comment_ >> topFunction_ >> *(
                  comment_ |
                  baseMVA_ |
                  busMatrix_ |
                  ignore_);

         BOOST_SPIRIT_DEBUG_NODE(row_); 
         BOOST_SPIRIT_DEBUG_NODE(topFunction_); 
         BOOST_SPIRIT_DEBUG_NODE(baseMVA_); 
         BOOST_SPIRIT_DEBUG_NODE(busMatrix_); 
         BOOST_SPIRIT_DEBUG_NODE(matrix_); 
         debug(row_); 
         debug(topFunction_); 
         debug(baseMVA_); 
         debug(busMatrix_); 
         debug(matrix_); 
      }

      void init() {baseMVAVal_ = 0.0;}

      void setBaseMVA(double val) 
      {
         baseMVAVal_ = val;
      }

      void setBusMatrix(const Matrix & val) 
      {
         busMatrixVal_ = val;
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

      Gram gram;
      while (fwdBegin != fwdEnd)
      {
         bool ok = phrase_parse(fwdBegin, fwdEnd, gram, blank);
      }
   }

   void MatpowerParser::postParse(const YAML::Node & nd, Model & mod, const ParserState & state) const
   {

   }

}
