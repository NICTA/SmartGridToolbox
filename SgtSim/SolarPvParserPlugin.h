// Copyright 2015-2016 National ICT Australia Limited (NICTA)
// Copyright 2016-2019 The Australian National University
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

#ifndef SOLAR_PV_PARSER_PLUGIN_DOT_H
#define SOLAR_PV_PARSER_PLUGIN_DOT_H

#include <SgtSim/SimParser.h>

namespace Sgt
{
    class Simulation;

    /// @addtogroup SimYamlSpec
    /// @{
    /// <b>YAML schema for `solar_pv` keyword.</b>
    ///
    /// The `solar_pv` keyword adds a new SolarPv Component to the Simulation.
    ///
    /// ~~~{.yaml}
    /// - solar_pv:
    ///     id:                         <string>    # Component ID.
    ///     inverter_id:                <string>    # ID of Inverter to which PV is attached.
    ///     weather:                    <string>    # ID of Weather component.
    ///     zenith_degrees:             <real>      # Zenith angle, degrees.
    ///     azimuth_degrees:            <real>      # Azimuth angle, degrees.
    ///     n_panels:                   <int>       # Number of panels.
    ///     panel_area_m2:              <int>       # Area of each panel, m^2.
    ///     irradiance_ref_W_per_m2:    <real>      # Reference (not actual) irradiance, W/m^2.
    ///     T_cell_ref_C:               <real>      # Reference cell temperature, °C.
    ///     P_max_ref_W:                <real>      # Reference MPPT power, W.
    ///     temp_coeff_P_max_per_C:     <real>      # Temperature coefficient at max power, °C^-1.
    ///     NOCT_C:                     <real>      # Normal Operating Cell Temperature, °C.
    /// ~~~
    /// @}

    /// @brief Parses the `solar_pv` keyword, adding a SolarPv to the simulation.
    /// @ingroup SimParserPlugins
    class SolarPvParserPlugin : public SimParserPlugin
    {
        public:
        virtual const char* key() const override
        {
            return "solar_pv";
        }

        public:
        virtual void parse(const YAML::Node& nd, Simulation& sim, const ParserBase& parser) const override;
    };
}

#endif // SOLAR_PV_PARSER_PLUGIN_DOT_H
