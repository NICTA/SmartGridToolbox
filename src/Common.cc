#include <Common.h>
#include <sstream>
#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>

namespace SmartGridToolbox
{

   template <typename Iterator> bool parse_complex(Iterator first, Iterator last, Complex & c)
   {
      // TODO: I'm sure this can be done better if I learn something about boost spirit.
      using boost::spirit::qi::double_;
      using boost::spirit::qi::char_;
      using boost::spirit::qi::string;
      using boost::spirit::qi::_1;
      using boost::spirit::qi::phrase_parse;
      using boost::spirit::ascii::space;
      using boost::phoenix::ref;
      using Expression = boost::spirit::qi::rule<Iterator, int(), boost::spirit::ascii::space_type>; 

      double re = 0.0;
      double im = 0.0;
      double ang = 0.0;
      bool hasDeg = false;
      bool hasRad = false;

      Expression cplxRect(
            '(' >> double_[ref(re) = _1] >> -(',' >> double_[ref(im) = _1]) >> ')' |
            double_[ref(re) = _1] >> -('+' >> double_[ref(im) = _1]) >> char_("ij") |
            double_[ref(re) = _1] >> -('-' >> double_[ref(im) = -_1]) >> char_("ij") |
            double_[ref(im) = _1] >> char_("ij") |
            char_("ij")[ref(im) = 1] |
            '-' >> char_("ij")[ref(im) = -1] |
            double_[ref(re) = _1]);

      Expression cplxRectPlusAng(
            (cplxRect >> char_('R') >> double_[ref(ang) = _1])[ref(hasRad) = true] |
            (cplxRect >> char_('D') >> double_[ref(ang) = _1])[ref(hasDeg) = true]);

      Expression cplx(cplxRectPlusAng | cplxRect);

      bool r = phrase_parse(first, last, cplx, space);

      if (!r || first != last) // fail if we did not get a full match
      {
         return false;
      }

      c = Complex(re, im);
      if (hasDeg)
      {
         c *= polar(1.0, ang * pi / 180.0);
      }
      else if (hasRad)
      {
         c *= polar(1.0, ang);
      }
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
