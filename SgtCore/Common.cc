// Copyright 2015 National ICT Australia Limited (NICTA)
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "Common.h"

#include <sstream>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_function.hpp>
#include <boost/spirit/include/phoenix_statement.hpp>
#include <boost/spirit/include/phoenix_bind.hpp>
#include <boost/spirit/include/phoenix.hpp>

namespace Qi = boost::spirit::qi;
namespace Ascii = boost::spirit::ascii;
namespace Phoenix = boost::phoenix;

using namespace arma;

namespace Sgt
{
    StreamIndent::StreamIndent(std::ostream& strm) :
        strm_(&strm),
        destBuf_(strm.rdbuf()),
        isFirst_(true),
        isNewline_(true),
        ind1_("    "),
        ind2_("    ")
    {
        strm_->rdbuf(this);
    }

    StreamIndent::~StreamIndent()
    {
        if (strm_ != NULL)
        {
            strm_->rdbuf(destBuf_);
        }
    }

    void StreamIndent::reset(const std::string& ind1, const std::string& ind2)
    {
        ind1_ = ind1;
        ind2_ = ind2;
        isFirst_ = true;
    }

    int StreamIndent::overflow(int ch)
    {
        if (isNewline_ && ch != '\n')
        {
            if (isFirst_)
            {
                destBuf_->sputn(ind1_.c_str(), static_cast<std::streamsize>(ind1_.size()));
            }
            else
            {
                destBuf_->sputn(ind2_.c_str(), static_cast<std::streamsize>(ind2_.size()));
            }
        }
        isNewline_ = ch == '\n';
        isFirst_ = false;
        return destBuf_->sputc(static_cast<char_type>(ch));
    }

    LogLevel& messageLogLevel()
    {
        static LogLevel level = LogLevel::NORMAL;
        return level;
    }

    LogLevel& warningLogLevel()
    {
        static LogLevel level = LogLevel::NORMAL;
        return level;
    }

    LogLevel& errorLogLevel()
    {
        static LogLevel level = LogLevel::NORMAL;
        return level;
    }

    LogLevel& debugLogLevel()
    {
        static LogLevel level = LogLevel::NONE;
        return level;
    }

    std::ostream& Log::message()
    {
        coutBuf_.reset(
                std::string("MESSAGE: ") + std::string(indentLevel_, ' '),
                std::string("         ") + std::string(indentLevel_, ' '));
        return std::cout;
    }

    std::ostream& Log::warning()
    {
        cerrBuf_.reset(
                std::string("WARNING: ") + std::string(indentLevel_, ' '),
                std::string("         ") + std::string(indentLevel_, ' '));
        return std::cerr;
    }

    std::ostream& Log::error()
    {
        cerrBuf_.reset(
                std::string("ERROR  : ") + std::string(indentLevel_, ' '),
                std::string("         ") + std::string(indentLevel_, ' '));
        return std::cerr;
    }

    std::ostream& Log::debug()
    {
        coutBuf_.reset(
                std::string("DEBUG  : ") + std::string(indentLevel_, ' '),
                std::string("         ") + std::string(indentLevel_, ' '));
        return std::cout;
    }

    unsigned int Log::indentLevel_ = 0;

    struct CGram : Qi::grammar<std::string::const_iterator, Complex(), Ascii::space_type>
    {
        CGram() : CGram::base_type(start_)
        {
            phaseRad_ = Qi::lit('R') >> Qi::double_[Phoenix::bind(&CGram::setAngRad, this, Qi::_1)];
            phaseDeg_ = Qi::lit('D') >> Qi::double_[Phoenix::bind(&CGram::setAngDeg, this, Qi::_1)];

            bracketedBoth_ = (Qi::lit('(') >> Qi::double_ >> Qi::lit(',') >> Qi::double_ >> Qi::lit(')'))
                [Phoenix::bind(&CGram::setResult, this, Qi::_1, Qi::_2)];
            bracketedIm_ = (Qi::lit('(') >> Qi::lit(',') >> Qi::double_ >> Qi::lit(')'))
                [Phoenix::bind(&CGram::setResult, this, 0.0, Qi::_1)];
            bracketedRe_ = (Qi::lit('(') >> Qi::double_ >> -Qi::lit(',') >> Qi::lit(')'))
                [Phoenix::bind(&CGram::setResult, this, Qi::_1, 0.0)];
            rePlusIm_ = (Qi::double_ >> Qi::lit('+') >> Qi::double_ >> Qi::char_("ij"))
                [Phoenix::bind(&CGram::setResult, this, Qi::_1, Qi::_2)];
            reMinusIm_ = (Qi::double_ >> Qi::lit('-') >> Qi::double_ >> Qi::char_("ij"))
                [Phoenix::bind(&CGram::setResult, this, Qi::_1, -Qi::_2)];
            im_ = (Qi::double_ >> Qi::char_("ij"))
                [Phoenix::bind(&CGram::setResult, this, 0.0, Qi::_1)];
            re_ = (Qi::double_)
                [Phoenix::bind(&CGram::setResult, this, Qi::_1, 0.0)];

            start_ = Qi::eps[Phoenix::bind(&CGram::init, this)] >>
                ((bracketedBoth_ | bracketedIm_ | bracketedRe_ | rePlusIm_ | reMinusIm_ | im_ | re_) >>
                 -(phaseRad_ | phaseDeg_))
                [Qi::_val = Phoenix::construct<Complex>(Phoenix::bind(&CGram::result, this))];
        }

        void init()
        {
            result_ = {0.0, 0.0};
        }
        void setResult(double re, double im)
        {
            result_ = {re, im};
        }
        void setAngRad(double ang)
        {
            result_ *= polar(1.0, ang);
        }
        void setAngDeg(double ang)
        {
            result_ *= polar(1.0, ang * pi / 180.0);
        }
        Complex result()
        {
            return result_;
        }

        Qi::rule<std::string::const_iterator, Complex(), Ascii::space_type> start_;
        Qi::rule<std::string::const_iterator, Complex(), Ascii::space_type> phaseRad_;
        Qi::rule<std::string::const_iterator, Complex(), Ascii::space_type> phaseDeg_;
        Qi::rule<std::string::const_iterator, Complex(), Ascii::space_type> bracketedBoth_;
        Qi::rule<std::string::const_iterator, Complex(), Ascii::space_type> bracketedIm_;
        Qi::rule<std::string::const_iterator, Complex(), Ascii::space_type> bracketedRe_;
        Qi::rule<std::string::const_iterator, Complex(), Ascii::space_type> rePlusIm_;
        Qi::rule<std::string::const_iterator, Complex(), Ascii::space_type> reMinusIm_;
        Qi::rule<std::string::const_iterator, Complex(), Ascii::space_type> im_;
        Qi::rule<std::string::const_iterator, Complex(), Ascii::space_type> re_;
        Complex result_;
    };

    std::ostream& operator<<(std::ostream& os, const Complex& c)
    {
        return os << to_string(c);
        // Doing it this way avoids hassles with stream manipulators, since there is only one insertion operator.
    }

    template<> Complex from_string<Complex>(const std::string& s)
    {
        Complex c;
        std::string::const_iterator iter = s.begin();
        std::string::const_iterator end = s.end();
        bool ok = Qi::phrase_parse(iter, end, CGram(), Ascii::space, c);
        sgtAssert(ok, "Bad complex number string: \"" << s << "\": came unstuck at substring: \"" << *iter << "\".");
        return c;
    }

    std::string to_string(const Complex& c)
    {
        std::ostringstream ss;
        float re = static_cast<float>(c.real());
        float im = static_cast<float>(c.imag());
        std::string reSgn = re >= 0.0 ? "" : "-";
        std::string imSgn = im >= 0.0 ? "+" : "-";
        if (im == 0.0)
        {
            ss << reSgn << std::abs(re);
        }
        else
        {
            ss << reSgn << std::abs(re) << imSgn << std::abs(im) << "j";
        }
        return ss.str();
    }
    
    template std::ostream& operator<< <double>(std::ostream& os, const Col<double>& v);
    template std::ostream& operator<< <float>(std::ostream& os, const Col<float>& v);
    template std::ostream& operator<< <int>(std::ostream& os, const Col<int>& v);
    template std::ostream& operator<< <uword>(std::ostream& os, const Col<uword>& v);
    template std::ostream& operator<< <Complex>(std::ostream& os, const Col<Complex>& v);

    template std::ostream& operator<< <double>(std::ostream& os, const Mat<double>& v);
    template std::ostream& operator<< <float>(std::ostream& os, const Mat<float>& v);
    template std::ostream& operator<< <int>(std::ostream& os, const Mat<int>& v);
    template std::ostream& operator<< <uword>(std::ostream& os, const Mat<uword>& v);
    template std::ostream& operator<< <Complex>(std::ostream& os, const Mat<Complex>& v);
    
    const posix_time::ptime epoch(gregorian::date(1970,1,1));

    Time timeFromDSeconds(double dSeconds)
    {
        long wholeSecs = long(dSeconds);
        double fracSecs = dSeconds - wholeSecs;
        return Time(0, 0, Time::sec_type(wholeSecs), 
                    Time::fractional_seconds_type(fracSecs * Time::ticks_per_second()));
    }

    posix_time::ptime utcTimeFromLocalTime(posix_time::ptime localTime, const local_time::time_zone_ptr localTz)
    {
        using namespace boost::local_time;
        if (localTime.is_not_a_date_time())
        {
            return localTime;
        }
        local_date_time ldt(localTime.date(), localTime.time_of_day(), localTz,
                local_date_time::NOT_DATE_TIME_ON_ERROR);
        return ldt.utc_time();
    }
    
    json JsonConvert<Complex>::toJson(const Complex& c)
    {
        return {c.real(), c.imag()};
    }
}
