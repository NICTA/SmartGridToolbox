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

#include "TimeSeriesParserPlugin.h"

namespace Sgt
{
    namespace
    {
        double getToSecsFactor(const std::string& timeUnitStr)
        {
            double result = 1.0;
            if (timeUnitStr == "s")
            {
                result = 1.0;
            }
            else if (timeUnitStr == "m")
            {
                result = 60.0;
            }
            else if (timeUnitStr == "h")
            {
                result = 3600.0;
            }
            else if (timeUnitStr == "d")
            {
                result = 86400.0;
            }
            else
            {
                Log().error() << "Invalid time unit in data_time_series. Aborting." << std::endl;
                error();
            }
            return result;
        }

        enum TsType
        {
            CONST_TS,
            DATA_TS,
            BAD_TS_TYPE
        };

        enum ValType
        {
            REAL_SCALAR,
            COMPLEX_SCALAR,
            REAL_VECTOR,
            COMPLEX_VECTOR,
            BAD_VAL_TYPE
        };

        enum InterpType
        {
            STEPWISE,
            LERP,
            //SPLINE,
            BAD_INTERP_TYPE
        };

        TsType getTsType(const std::string& tsTypeStr)
        {
            TsType result = BAD_TS_TYPE;

            if (tsTypeStr == "const_time_series")
            {
                result = CONST_TS;
            }
            else if (tsTypeStr == "data_time_series")
            {
                result = DATA_TS;
            }
            else
            {
                Log().error() << "Bad time series type " << tsTypeStr << std::endl;
                error();
            }
            return result;
        }

        ValType getValType(const std::string& valTypeStr)
        {
            ValType result = BAD_VAL_TYPE;

            if (valTypeStr == "real_scalar")
            {
                result = REAL_SCALAR;
            }
            else if (valTypeStr == "complex_scalar")
            {
                result = COMPLEX_SCALAR;
            }
            else if (valTypeStr == "real_vector")
            {
                result = REAL_VECTOR;
            }
            else if (valTypeStr == "complex_vector")
            {
                result = COMPLEX_VECTOR;
            }
            else
            {
                Log().error() << "Bad value_type " << valTypeStr << " for time series." << std::endl;
                error();
            }
            return result;
        }

        InterpType getInterpType(const std::string& interpTypeStr)
        {
            InterpType result = BAD_INTERP_TYPE;

            if (interpTypeStr == "stepwise")
            {
                result = STEPWISE;
            }
            else if (interpTypeStr == "lerp")
            {
                result = LERP;
            }
            //else if (interpTypeStr == "spline")
            //{
            //    result = SPLINE;
            //}
            else
            {
                Log().error() << "Bad interp_type " << interpTypeStr << " for time series." << std::endl;
                error();
            }
            return result;
        }

        template<typename T> std::unique_ptr<DataTimeSeries<Time, T>> initDataTimeSeries_(InterpType interpType)
        {
            std::unique_ptr<DataTimeSeries<Time, T>> dts;
            switch (interpType)
            {
                case STEPWISE:
                    dts.reset(new StepwiseTimeSeries<Time, T>());
                    break;
                case LERP:
                    dts.reset(new LerpTimeSeries<Time, T>());
                    break;
                //case SPLINE:
                //    Log().error() << "Spline data time series can only be used with real data" << std::endl;
                //    error();
                //    break;
                default:
                    Log().error() << "Bad time series interpolation type." << std::endl;
                    error();
                    break;
            }
            return dts;
        }

        template<typename T> std::unique_ptr<DataTimeSeries<Time, T>> initDataTimeSeries(InterpType interpType)
        {
            return initDataTimeSeries_<T>(interpType);
        }

        template<> std::unique_ptr<DataTimeSeries<Time, double>> initDataTimeSeries<double>(InterpType interpType)
        {
            //if (interpType == SPLINE)
            //{
            //    return std::unique_ptr<DataTimeSeries<Time, double>>(new SplineTimeSeries<Time>());
            //}
            //else
            {
                return initDataTimeSeries_<double>(interpType);
            }
        }

        Time readTime(std::istringstream& ss, const Time& t0, double toSecs)
        {
            double secs;
            ss >> secs;
            secs *= toSecs;
            return t0 + timeFromDSeconds(secs);
        }

    } // Anon. namespace.

    void TimeSeriesParserPlugin::parse(const YAML::Node& nd, Simulation& sim, const ParserBase& parser) const
    {
        std::unique_ptr<TimeSeriesBase> ts;

        assertFieldPresent(nd, "id"); // data
        assertFieldPresent(nd, "type"); // data
        assertFieldPresent(nd, "value_type"); // real_scalar/complex_scalar/real_vector/complex_vector

        std::string id = parser.expand<std::string>(nd["id"]);
        auto tsType = getTsType(parser.expand<std::string>(nd["type"]));
        auto valType = getValType(parser.expand<std::string>(nd["value_type"]));

        switch (tsType)
        {
            case CONST_TS:
            {
                assertFieldPresent(nd, "const_value");
                auto ndVal = nd["const_value"];
                switch(valType)
                {
                    case REAL_SCALAR:
                    {
                        double v = parser.expand<double>(ndVal);
                        ts.reset(new ConstTimeSeries<Time, double>(v));
                        break;
                    }
                    case COMPLEX_SCALAR:
                    {
                        Complex v = parser.expand<Complex>(ndVal);
                        ts.reset(new ConstTimeSeries<Time, Complex>(v));
                        break;
                    }
                    case REAL_VECTOR:
                    {
                        arma::Col<double> v = parser.expand<arma::Col<double>>(ndVal);
                        ts.reset(new ConstTimeSeries<Time, arma::Col<double>>(v));
                        break;
                    }
                    case COMPLEX_VECTOR:
                    {
                        arma::Col<Complex> v = parser.expand<arma::Col<Complex>>(ndVal);
                        ts.reset(new ConstTimeSeries<Time, arma::Col<Complex>>(v));
                        break;
                    }
                    default:
                    {
                        Log().error() << "Bad time series value type." << std::endl;
                        error();
                        break;
                    }
                }
                break;
            } // CONST_TS
            case DATA_TS:
            {
                assertFieldPresent(nd, "data_file");
                assertFieldPresent(nd, "interp_type");
                assertFieldPresent(nd, "time_unit");

                std::string dataFName = parser.expand<std::string>(nd["data_file"]);
                std::ifstream infile(dataFName);
                if (!infile.is_open())
                {
                    Log().error() << "Could not open the timeseries input file " << dataFName << "." << std::endl;
                    error();
                }

                auto interpType = getInterpType(parser.expand<std::string>(nd["interp_type"]));

                Time t0 = posix_time::seconds(0);
                auto ndRelto = nd["relative_to_time"];
                if (ndRelto)
                {
                    std::string relto = parser.expand<std::string>(nd["relative_to_time"]);
                    posix_time::ptime pt = posix_time::time_from_string(relto);
                    t0 = timeFromLocalTime(pt, sim.timezone());
                }

                double toSecs = getToSecsFactor(parser.expand<std::string>(nd["time_unit"]));

                std::string line;
                switch(valType)
                {
                    case REAL_SCALAR:
                    {
                        std::unique_ptr<DataTimeSeries<Time, double>> dts = initDataTimeSeries<double>(interpType);
                        while (std::getline(infile, line))
                        {
                            std::istringstream ss(line);
                            Time t = readTime(ss, t0, toSecs);
                            double val;
                            ss >> val;
                            assert(ss.eof());
                            dts->addPoint(t, val);
                        }
                        ts = std::move(dts);
                        break;
                    }
                    case COMPLEX_SCALAR:
                    {
                        std::unique_ptr<DataTimeSeries<Time, Complex>> dts = initDataTimeSeries<Complex>(interpType);
                        while (std::getline(infile, line))
                        {
                            std::istringstream ss(line);
                            Time t = readTime(ss, t0, toSecs);
                            std::string valStr;
                            ss >> valStr;
                            assert(ss.eof());
                            dts->addPoint(t, from_string<Complex>(valStr));
                        }
                        ts = std::move(dts);
                        break;
                    }
                    case REAL_VECTOR:
                    {
                        std::unique_ptr<DataTimeSeries<Time, arma::Col<double>>> dts =
                            initDataTimeSeries<arma::Col<double>>(interpType);
                        while (std::getline(infile, line))
                        {
                            std::istringstream ss(line);
                            Time t = readTime(ss, t0, toSecs);
                            std::vector<double> valVec;
                            while (!ss.eof())
                            {
                                double val;
                                ss >> val;
                                valVec.push_back(val);
                            }
                            arma::Col<double> val(valVec.size());
                            std::copy(valVec.begin(), valVec.end(), val.begin());
                            dts->addPoint(t, val);
                        }
                        ts = std::move(dts);
                        break;
                    }
                    case COMPLEX_VECTOR:
                    {
                        std::unique_ptr<DataTimeSeries<Time, arma::Col<Complex>>> dts =
                            initDataTimeSeries<arma::Col<Complex>>(interpType);
                        while (std::getline(infile, line))
                        {
                            std::istringstream ss(line);
                            Time t = readTime(ss, t0, toSecs);
                            std::vector<Complex> valVec;
                            while (!ss.eof())
                            {
                                std::string valStr;
                                ss >> valStr;
                                valVec.push_back(from_string<Complex>(valStr));
                            }
                            arma::Col<Complex> val(valVec.size());
                            std::copy(valVec.begin(), valVec.end(), val.begin());
                            dts->addPoint(t, val);
                        }
                        ts = std::move(dts);
                        break;
                    }
                    default:
                    {
                        Log().error() << "Bad time series value type." << std::endl;
                        error();
                        break;
                    }
                }
                break;
            } // DATA_TS
            default:
            {
                Log().error() << "Bad time series type." << std::endl;
                error();
                break;
            }
        }
        sim.acquireTimeSeries(id, std::move(ts));
    }
}
