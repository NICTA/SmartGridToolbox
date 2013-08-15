#include <Common.h>
#include <sstream>
#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>

namespace SmartGridToolbox
{

   template <typename Iterator> bool parse_complex(Iterator first, Iterator last, std::complex<double>& c)
   {
      // TODO: I'm sure this can be done better if I learn something about boost spirit.
      using boost::spirit::qi::double_;
      using boost::spirit::qi::_1;
      using boost::spirit::qi::phrase_parse;
      using boost::spirit::ascii::space;
      using boost::phoenix::ref;

      double rN = 0.0;
      double iN = 0.0;
      bool r = phrase_parse(first, last,
            //  Begin grammar
            (
               '(' >> double_[ref(rN) = _1] >> -(',' >> double_[ref(iN) = _1]) >> ')' |
               double_[ref(rN) = _1] >> -('+' >> double_[ref(iN) = _1]) >> 'i' |
               double_[ref(rN) = _1] >> -('+' >> double_[ref(iN) = _1]) >> 'j' |
               double_[ref(rN) = _1] >> -('-' >> double_[ref(iN) = -_1]) >> 'i' |
               double_[ref(rN) = _1] >> -('-' >> double_[ref(iN) = -_1]) >> 'j' |
               double_[ref(rN) = _1] |
               double_[ref(iN) = _1] >> 'i' |
               double_[ref(iN) = _1] >> 'j'
            ),
            //  End grammar
            space);

      if (!r || first != last) // fail if we did not get a full match
         return false;
      c = std::complex<double>(rN, iN);
      return r;
   }

   std::ostream & operator<<(std::ostream & os, const Complex & c)
   {
      return os << complex2String(c);
      // Doing it this way avoids hassles with stream manipulators, since there is only one insertion operator.
   }

   Complex string2Complex(const std::string & s)
   {
      Complex c;
      if (!parse_complex(s.begin(), s.end(), c))
      {
         c = {NAN, NAN};
      }
      return c;
   }

   std::string complex2String(const Complex & c)
   {
      std::ostringstream ss;
      char reSgn = c.real() >= 0.0 ? ' ' : '-';
      char imSgn = c.imag() >= 0.0 ? '+' : '-';
      ss << reSgn << std::abs(c.real()) << imSgn << std::abs(c.imag()) << "j";
      return ss.str();
   }
}
