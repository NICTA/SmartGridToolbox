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

#ifndef YAML_SUPPORT_DOT_H
#define YAML_SUPPORT_DOT_H

#include <SgtCore/Common.h>
#include <SgtCore/PowerFlow.h>

#include <yaml-cpp/yaml.h>

namespace Sgt
{
    /// @brief Convert a value to a YAML string.
    /// @ingroup Utilities
    template<typename T> std::string toYamlString(const T& t)
    {
        YAML::Emitter e;
        return (e << YAML::Flow << YAML::Node(t)).c_str();
    }

    /// @brief Create a value from a YAML string.
    /// @ingroup Utilities
    template<typename T> T fromYamlString(const std::string& s)
    {
        return YAML::Load(s).as<T>();
    }

    template<> inline const char* fromYamlString(const std::string& s)
    {
        return YAML::Load(s).as<std::string>().c_str();
    }
}

namespace YAML
{
    template<> struct convert<std::nullptr_t>
    {
        static Node encode(const std::nullptr_t& from);
        static bool decode(const Node& nd, std::nullptr_t& to);
    };
    
    template<> struct convert<Sgt::json>
    {
        static Node encode(const Sgt::json& from);
        static bool decode(const Node& nd, Sgt::json& to);
    };

    template<> struct convert<Sgt::Complex>
    {
        static Node encode(const Sgt::Complex& from);
        static bool decode(const Node& nd, Sgt::Complex& to);
    };

    template<> struct convert<Sgt::Phase>
    {
        static Node encode(const Sgt::Phase& from);
        static bool decode(const Node& nd, Sgt::Phase& to);
    };

    template<> struct convert<Sgt::Phases>
    {
        static Node encode(const Sgt::Phases& from);
        static bool decode(const Node& nd, Sgt::Phases& to);
    };

    template<> struct convert<Sgt::BusType>
    {
        static Node encode(const Sgt::BusType& from);
        static bool decode(const Node& nd, Sgt::BusType& to);
    };

    template<> struct convert<Sgt::Time>
    {
        static Node encode(const Sgt::Time& from);
        static bool decode(const Node& nd, Sgt::Time& to);
    };

    template<> struct convert<boost::posix_time::ptime>
    {
        static Node encode(const boost::posix_time::ptime& from);
        static bool decode(const Node& nd, boost::posix_time::ptime& to);
    };

    template<typename T> struct convert<arma::Col<T>>
    {
        static Node encode(const arma::Col<T>& from);
        static bool decode(const Node& nd, arma::Col<T>& to);
    };

    template<typename T> struct convert<arma::Mat<T>>
    {
        static Node encode(const arma::Mat<T>& from);
        static bool decode(const Node& nd, arma::Mat<T>& to);
    };
}

#endif // YAML_SUPPORT_DOT_H
