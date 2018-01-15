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

#ifndef COMMON_DOT_H
#define COMMON_DOT_H

#include <SgtCore/ComponentCollection.h>
#include <SgtCore/json.h>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/local_time/local_time.hpp>

#include <armadillo>

#include <algorithm>
#include <complex>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <mutex>

/// @name Utility macros - not normally for public consumption.
/// @{

// Macro symbol concatenation.
#define SGT_DO_CAT(A, B) A ## B
#define SGT_CAT(A, B) SGT_DO_CAT(A, B)

// Unique name (within file scope). 
#define SGT_UNIQUE_NAME(PREFIX) SGT_CAT(PREFIX, __LINE__)

// For overloaded variadic function-like macros:
#define SGT_COMPOSE(NAME, ARGS) NAME ARGS
#define SGT_GET_COUNT(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, COUNT, ...) COUNT
#define SGT_EXPAND() ,,,,,,,,,, // 10 commas (or 10 empty tokens)
#define SGT_VA_SIZE(...) SGT_COMPOSE(SGT_GET_COUNT, (SGT_EXPAND __VA_ARGS__ (), 0, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0))
#define SGT_VA_SELECT(NAME, ...) SGT_CAT(NAME, SGT_VA_SIZE(__VA_ARGS__))(__VA_ARGS__)

/// @}

namespace Sgt
{
    /// @name Reporting and errors.
    /// @{

    class StreamIndent : public std::streambuf
    {
        public:

        explicit StreamIndent(std::ostream& strm);

        virtual ~StreamIndent();

        void reset(const std::string& ind1, const std::string& ind2);

        protected:

        virtual int overflow(int ch) override;

        private:

        std::ostream* strm_;
        std::streambuf* destBuf_;
        bool isFirst_;
        bool isNewline_;
        std::string ind1_;
        std::string ind2_;
    };

    /// @brief Logging object.
    ///
    /// Use e.g. Log().message() << "this is a message" << std::endl;
    /// @ingroup Utilities
    class Log
    {
        friend class LogIndent;

        public:

        std::ostream& message();

        std::ostream& warning();

        std::ostream& error();

        std::ostream& debug();

        private:

        static std::mutex mutex_;
        static unsigned int indentLevel_;

        std::unique_lock<std::mutex> lock_{mutex_};
        StreamIndent coutBuf_{std::cout};
        StreamIndent cerrBuf_{std::cerr};
    };

    enum class LogLevel
    {
        NONE,
        NORMAL,
        VERBOSE
    };

    /// @brief Get a reference to the current message logging level.
    LogLevel& messageLogLevel();
    /// @brief Get a reference to the current warning logging level.
    LogLevel& warningLogLevel();
    /// @brief Get a reference to the current error logging level.
    LogLevel& errorLogLevel();
    /// @brief Get a reference to the current debug logging level.
    LogLevel& debugLogLevel();

    /// @brief Get a reference to the current message logging level.
    std::string& messageLogFilter();
    /// @brief Get a reference to the current warning logging level.
    std::string& warningLogFilter();
    /// @brief Get a reference to the current error logging level.
    std::string& errorLogFilter();
    /// @brief Get a reference to the current debug logging level.
    std::string& debugLogFilter();

    /// @brief Create a LogIndent on the stack to indent all logs.
    ///
    /// Indentation will persist while object is in scope.
    /// @ingroup Utilities
    class LogIndent
    {
        public:

        static unsigned int defaultTabWidth;

        public:

        LogIndent(unsigned int nInit = 1, unsigned int tabWidth = defaultTabWidth);

        ~LogIndent();

        void in(unsigned int n = 1);

        void out(unsigned int n = 1);

        private:

        unsigned int tabWidth_{4};
        unsigned int myIndentLevel_{0};
    };

    // Debug macro.
#ifdef DEBUG
#define SGT_DEBUG if (true)
#else 
#define SGT_DEBUG if (false)
#endif

    // Internal macros to help with machinery of logging.
#define SGT_LOG(strm, level, fnames, threshold) if (level >= threshold && (fnames[0] == '\0' || strstr(fnames, __FILE__) != nullptr)) strm

#define SGT_LOG_MESSAGE1(threshold) SGT_LOG(Sgt::Log().message(), Sgt::messageLogLevel(), Sgt::messageLogFilter().c_str(), threshold)
#define SGT_LOG_MESSAGE0() SGT_LOG_MESSAGE1(Sgt::LogLevel::NORMAL)

#define SGT_LOG_WARNING1(threshold) SGT_LOG(Sgt::Log().warning(), Sgt::warningLogLevel(), Sgt::warningLogFilter().c_str(), threshold)
#define SGT_LOG_WARNING0() SGT_LOG_WARNING1(Sgt::LogLevel::NORMAL)

#define SGT_LOG_ERROR1(threshold) SGT_LOG(Sgt::Log().error(), Sgt::errorLogLevel(), Sgt::errorLogFilter().c_str(), threshold)
#define SGT_LOG_ERROR0() SGT_LOG_ERROR1(Sgt::LogLevel::NORMAL)

#define SGT_LOG_DEBUG1(threshold) SGT_LOG(Sgt::Log().debug(), Sgt::debugLogLevel(), Sgt::debugLogFilter().c_str(), threshold)
#define SGT_LOG_DEBUG0() SGT_LOG_DEBUG1(Sgt::LogLevel::NORMAL)

#define SGT_LOG_INDENT2(nInit, tabWidth) Sgt::LogIndent SGT_UNIQUE_NAME(indent)(nInit, tabWidth);
#define SGT_LOG_INDENT1(nInit) Sgt::LogIndent SGT_UNIQUE_NAME(indent)(nInit);
#define SGT_LOG_INDENT0() Sgt::LogIndent SGT_UNIQUE_NAME(indent);

    /// @brief Log a message.
    /// @ingroup Utilities
    ///
    /// E.g. sgtLogMessage(LogLevel::VERBOSE) << "this is a message: number = " << 5 << std::endl;
    /// Parameter is optional and defaults to LogLevel::NORMAL
#define sgtLogMessage(...) SGT_VA_SELECT(SGT_LOG_MESSAGE, __VA_ARGS__)

    /// @brief Log a warning.
    /// @ingroup Utilities
    ///
    /// E.g. sgtLogWarning(LogLevel::VERBOSE) << "this is a warning: number = " << 5 << std::endl;
    /// Parameter is optional and defaults to LogLevel::NORMAL
#define sgtLogWarning(...) SGT_VA_SELECT(SGT_LOG_WARNING, __VA_ARGS__)

    /// @brief Log an error.
    /// @ingroup Utilities
    ///
    /// E.g. sgtLogError(LogLevel::VERBOSE) << "this is an error: number = " << 5 << std::endl;
    /// Parameter is optional and defaults to LogLevel::NORMAL
#define sgtLogError(...) SGT_VA_SELECT(SGT_LOG_ERROR, __VA_ARGS__)

    /// @brief Log a debug message.
    /// @ingroup Utilities
    ///
    /// E.g. sgtLogDebug(LogLevel::VERBOSE) << "this is a debug message: number = " << 5 << std::endl;
    /// Parameter is optional and defaults to LogLevel::NORMAL
#define sgtLogDebug(...) SGT_VA_SELECT(SGT_LOG_DEBUG, __VA_ARGS__)

    /// @brief Indent the logs.
    /// @ingroup Utilities
    ///
    /// Indent lasts until the end of the current scope.
#define sgtLogIndent(...) SGT_VA_SELECT(SGT_LOG_INDENT, __VA_ARGS__)

#define sgtError(msg) {std::ostringstream ss; ss << "SmartGridToolbox: " << __PRETTY_FUNCTION__ << ": " << msg; throw std::runtime_error(ss.str());}

    /// @ingroup Utilities
#define sgtAssert(cond, msg) if (!(cond)) {sgtLogError() << "SmartGridToolbox: " << __PRETTY_FUNCTION__ << ": " << msg << std::endl; std::abort();}

    /// @}

    /// @name String conversion.
    /// @{

    /// @brief Create a value from a string.
    /// @ingroup Utilities
    template<typename T> T from_string(const std::string& s);

    /// @}

    /// @name Constant dimension 1D array type.
    /// @{

    /// @ingroup Utilities
    template <class T, size_t N> using Array = std::array<T, N>; // Just std::array but rename for nice consistency.

    /// @}

    /// @name Constant dimension 2D array type.
    /// @{

    // Note transposition of NR and NC to obey standard matrix index order.
    /// @ingroup Utilities
    template <class T, size_t NR, size_t NC> using Array2D = std::array<std::array<T, NC>, NR>;

    /// @}

    /// @name Complex numbers
    /// @{

    /// @addtogroup Utilities
    /// @{

    using Complex = std::complex<double>;

    inline Complex polar(double m, double theta) // theta is radians.
    {
        // Note the following will use RVO in C++11, no unneeded temporaries.
        return Complex(m * cos(theta), m * sin(theta));
    }

    inline Complex operator*(int i, const Complex& c)
    {
        return Complex(i * c.real(), i * c.imag());
    }

    inline Complex operator*(const Complex& c, int i)
    {
        return Complex(i * c.real(), i * c.imag());
    }

    std::ostream& operator<<(std::ostream& os, const Complex& c);

    template<> Complex from_string<Complex>(const std::string& s);

    std::string to_string(const Complex& c);

    /// @}

    /// @}

    /// @name Linear algebra
    /// @{

    /// @ingroup Utilities
    inline double sumOfSquares(const arma::Col<double>& x)
    {
        return sum(square(x));
    }

    /// @ingroup Utilities
    inline double sumOfSquares(const arma::Col<Complex>& x)
    {
        return std::accumulate(x.begin(), x.end(), 0.0,
                [](double cur, const Complex& xi)->double{return cur + norm(xi);});
    }

    /// @ingroup Utilities
    template<typename T> double rms(const T& x)
    {
        return sqrt(sumOfSquares(x)/x.size());
    }

    /// @ingroup Utilities
    template<typename T> std::ostream& operator<<(std::ostream& os, const arma::Col<T>& v)
    {
        auto w = static_cast<int>(os.width());
        std::ostringstream ss;
        ss.flags(os.flags());
        ss.imbue(os.getloc());
        ss.precision(os.precision());
        ss << "[";
        if (v.size() > 0)
        {
            ss << std::setw(w) << std::left << v(0);
            for (arma::uword i = 1; i < v.size(); ++i) ss << ", " << std::setw(w) << std::left << v(i);
        }
        ss << "]";
        return os << ss.str();
    }
    extern template std::ostream& operator<< <double>(std::ostream& os, const arma::Col<double>& v);
    extern template std::ostream& operator<< <float>(std::ostream& os, const arma::Col<float>& v);
    extern template std::ostream& operator<< <int>(std::ostream& os, const arma::Col<int>& v);
    extern template std::ostream& operator<< <arma::uword>(std::ostream& os, const arma::Col<arma::uword>& v);
    extern template std::ostream& operator<< <Complex>(std::ostream& os, const arma::Col<Complex>& v);

    /// @ingroup Utilities
    template<typename T> std::ostream& operator<<(std::ostream& os, const arma::Mat<T>& m)
    {
        auto w = static_cast<int>(os.width());
        std::ostringstream ss;
        ss.flags(os.flags());
        ss.imbue(os.getloc());
        ss.precision(os.precision());
        ss << std::endl << "[" << std::endl;
        ss << "    [" << std::setw(w) << std::left << m(0, 0);
        for (arma::uword j = 1; j < m.n_cols; ++j)
        {
            ss << ", " << std::setw(w) << std::left << m(0, j);
        }
        ss << "]";
        for (arma::uword i = 1; i < m.n_rows; ++i)
        {
            ss << "," << std::endl << "    [" << std::setw(w) << std::left << m(i, 0);
            for (arma::uword j = 1; j < m.n_cols; ++j)
            {
                ss << ", " << std::setw(w) << std::left << m(i, j);
            }
            ss << "]";
        }
        ss << std::endl << "]" << std::endl;
        return os << ss.str();
    }
    extern template std::ostream& operator<< <double>(std::ostream& os, const arma::Mat<double>& v);
    extern template std::ostream& operator<< <float>(std::ostream& os, const arma::Mat<float>& v);
    extern template std::ostream& operator<< <int>(std::ostream& os, const arma::Mat<int>& v);
    extern template std::ostream& operator<< <arma::uword>(std::ostream& os, const arma::Mat<arma::uword>& v);
    extern template std::ostream& operator<< <Complex>(std::ostream& os, const arma::Mat<Complex>& v);

    /// @}

    /// @name Time
    /// @{

    /// @brief Simulation times and durations.
    ///
    /// For simplicity, we don't distinguish between time points and durations. The interpretation is context
    /// dependent, and time points are treated as an offset from the posix epoch. This means we don't have any
    /// confusion about things like leap seconds and the like. 
    /// @ingroup Utilities
    using Time = boost::posix_time::time_duration;

    /// @brief PIMPL wrapper class for Boost local_time_zone_ptr.
    ///
    /// Hides the machinery of Boost's time_zone_ptr mechanism, as we wish to not be tied to Boost.
    /// @ingroup Utilities
    class Timezone
    {
        public:

        Timezone() : z_(new boost::local_time::posix_time_zone("UTC")) {}
        Timezone(const std::string& s) : z_(new boost::local_time::posix_time_zone(s)) {}
        operator boost::local_time::time_zone_ptr() const {return z_;};

        private:

        boost::local_time::time_zone_ptr z_{nullptr};
    };

    Timezone& timezone();

    using TimeSpecialValues = boost::date_time::special_values;

    /// @ingroup Utilities
    using microseconds = boost::posix_time::microseconds;
    /// @ingroup Utilities
    using milliseconds = boost::posix_time::milliseconds;
    /// @ingroup Utilities
    using seconds = boost::posix_time::seconds;
    /// @ingroup Utilities
    using minutes = boost::posix_time::minutes;
    /// @ingroup Utilities
    using hours = boost::posix_time::hours;

    /// @ingroup Utilities
    inline double dSeconds(const Time& d)
    {
        return double(d.ticks()) / Time::ticks_per_second();
    }

    /// @ingroup Utilities
    Time timeFromDSeconds(double dSeconds);

    /// @ingroup Utilities
    inline Time timeFromDurationString(const std::string& durString)
    {
        return boost::posix_time::duration_from_string(durString);
    }

    /// @ingroup Utilities
    Time timeFromUtcTimeString(const std::string& utcTimeString);

    /// @ingroup Utilities
    std::string utcTimeString(const Time& t);

    /// @ingroup Utilities
    Time timeFromLocalTimeStringAndZone(const std::string& localTimeString, Timezone zone = timezone());

    /// @ingroup Utilities
    std::string localTimeString(const Time& t, Timezone zone = timezone());

    /// @}

    /// @name Boost ptime and time_t access
    /// @brief Useful only in a few select cases, e.g. involving parsing.
    ///
    /// Not part of the normal treatment of time points!
    /// @{

    /// @ingroup Utilities
    Time timeFromUtcPtime(const boost::posix_time::ptime& utcPtime);

    /// @ingroup Utilities
    boost::posix_time::ptime utcPtime(const Time& t);

    /// @ingroup Utilities
    Time timeFromLocalPtime(const boost::posix_time::ptime& localPtime, Timezone zone = timezone());

    /// @ingroup Utilities
    boost::posix_time::ptime localPtime(const Time& t, Timezone zone = timezone());

    /// @ingroup Utilities
    inline Time timeFromUtcTimeT(std::time_t timeT)
    {
        return timeFromUtcPtime(boost::posix_time::from_time_t(timeT));
    }

    /// @ingroup Utilities
    inline std::time_t utcTimeT(const Time& t)
    {
        // return boost::posix_time::to_time_t(utcPtime(t));
        // Issue with e.g. Ubuntu Trusty. Replace with custom code (essentially copy-pasted from boost itself).

        auto dur = utcPtime(t) - boost::posix_time::ptime(boost::gregorian::date(1970,1,1));
        return std::time_t(dur.total_seconds()); 
    }

    /// @}

    /// @name LatLongs
    /// @{

    /// @ingroup Utilities
    /// @brief Simple struct storing a latitude and longitude.
    struct LatLong
    {
        double lat_;
        double long_;
    };

    /// @}

    /// @addtogroup Utilities
    /// @{

    /// @name Constants
    /// @{

    constexpr double pi = 3.141592653589793238462643383279502884;
    constexpr double negInfinity = -std::numeric_limits<double>::infinity(); // No guarantee, but no better option?
    constexpr double infinity = std::numeric_limits<double>::infinity();
    constexpr Complex im{0, 1};
    constexpr double second = 1.0;
    constexpr double minute = 60.0 * second;
    constexpr double hour = 60.0 * minute;
    constexpr double day = 24.0 * hour;
    constexpr double week = 7 * day;
    constexpr double joule = 1.0;
    constexpr double kjoule = 1000.0 * joule;
    constexpr double watt = joule / second;
    constexpr double kwatt = 1000.0 * watt;
    constexpr double kwattHour = kwatt * hour;
    constexpr double amp = 1.0;
    constexpr double coulomb = amp * second;
    constexpr double kelvin = 1.0;
    constexpr LatLong greenwich{51.4791, 0.0};

    /// @}

    /// @}
}

/// @name JSON.
/// @{

namespace Sgt
{
    using json = nlohmann::json;

    /// @brief Deduce if toJson() member function exists.
    template<typename T> struct JsonTraits
    {
        private:
        template<typename U> 
            static auto testMember(int) -> decltype(std::declval<U>().toJson(), std::true_type());
        template<typename> static std::false_type testMember(...);
        using HasMember = decltype(testMember<T>(0));
        public:
        constexpr static bool hasMember = std::is_same<HasMember, std::true_type>::value;
    };

    /// @brief Provide a default class conversion if a toJson() member function exists.
    template<typename T> auto to_json(json& js, const T& t)
        -> typename std::enable_if<JsonTraits<T>::hasMember, void>::type
        {
            js = t.toJson();
        };

    /// @brief Json conversion for ComponentCollection.
    template<typename T> void to_json(json& js, const ComponentCollection<T>& x)
    {
        js = json();
        for (auto c : x)
        {
            js.push_back(*c);
        }
    }
}

namespace nlohmann
{
    /// JSON conversion for complex numbers.
    template<typename T> struct adl_serializer<std::complex<T>>
    {
        static void to_json(json& js, const std::complex<T>& c)
        {
            js = {c.real(), c.imag()};
        }

        static void from_json(const json& js, std::complex<T>& c)
        {
            c = {js[0].get<double>(), js[1].get<double>()};
        }
    };

    /// Automatically dereference pointers.
    template<typename T> struct adl_serializer<T*>
    {
        static void to_json(json& js, T* t)
        {
            js = *t;
        }
    };

    /// JSON conversion for armadillo vectors.
    template<typename T> struct adl_serializer<arma::Col<T>>
    {
        static void to_json(json& js, const arma::Col<T>& vec)
        {
            js = json();
            for (arma::uword i = 0; i < vec.n_rows; ++i)
            {
                js.push_back(vec(i));
            }
        }

        static void from_json(const json& js, arma::Col<T>& vec)
        {
            arma::uword n = js.size();
            vec = arma::Col<T>(n, arma::fill::none);
            for (arma::uword i = 0; i < n; ++i)
            {
                vec(i) = js[i].get<T>();
            }
        }
    };

    /// JSON conversion for armadillo matrices.
    template<typename T> struct adl_serializer<arma::Mat<T>>
    {
        static void to_json(json& js, const arma::Mat<T>& mat)
        {
            js = json();
            for (arma::uword i = 0; i < mat.n_rows; ++i)
            {
                json row;
                for (arma::uword j = 0; j < mat.n_cols; ++j)
                {
                    row.push_back(mat(i, j));
                }
                js.push_back(row);
            }
        }

        static void from_json(const json& js, arma::Mat<T>& mat)
        {
            arma::uword n = js.size();
            if (n == 0)
            {
                mat = arma::Mat<T>(0, 0);
            }
            else
            {
                arma::uword m = js[0].size();
                mat = arma::Mat<T>(n, m, arma::fill::none);
                for (arma::uword i = 0; i < n; ++i)
                {
                    for (arma::uword j = 0; j < m; ++j)
                    {
                        mat(i, j) = js[i][j].get<T>();
                    }
                }
            }
        }
    };
}

namespace arma
{
    // Provide documented but missing arg function in armadillo.
    template<template<typename> class T, typename U> T<U> arg(const T<std::complex<U>>& x)
    {
        T<double> result(size(x));
        for (uword i = 0; i < x.size(); ++i) result(i) = std::arg(x(i));
        return result;
    }

    // Provide a polar function for armadillo.
    template<template<typename> class T, typename U> T<std::complex<U>> polar(const T<U>& m, const T<U>& a)
    {
        return T<std::complex<U>>(m % cos(a), m % sin(a));
    }
}

/// @}

#endif // COMMON_DOT_H
