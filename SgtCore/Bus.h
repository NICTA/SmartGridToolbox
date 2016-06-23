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

#ifndef BUS_DOT_H
#define BUS_DOT_H

#include <SgtCore/Common.h>
#include <SgtCore/Component.h>
#include <SgtCore/Event.h>
#include <SgtCore/PowerFlow.h>

#include<iostream>
#include<map>
#include<vector>

namespace Sgt
{
    class GenAbc;
    class ZipAbc;

    /// @brief A Bus is a grouped set of conductors / terminals, one per phase.
    /// @ingroup PowerFlowCore
    class Bus : virtual public Component
    {
        public:

            SGT_PROPS_INIT(Bus);
            SGT_PROPS_INHERIT(Component);

        /// @name Static member functions:
        /// @{

            static const std::string& sComponentType()
            {
                static std::string result("bus");
                return result;
            }

        /// @}

        /// @name Lifecycle:
        /// @{

            Bus(const std::string& id, const Phases& phases, const arma::Col<Complex>& VNom, double VBase);

        /// @}

        /// @name Component virtual overridden member functions.
        /// @{

            virtual const std::string& componentType() const override
            {
                return sComponentType();
            }

            virtual json toJson() const override;

        /// @}

        /// @name Basic identity and type:
        /// @{

            virtual const Phases& phases() const
            {
                return phases_;
            }

            SGT_PROP_GET(phases, phases, const Phases&);

            virtual arma::Col<Complex> VNom() const
            {
                return VNom_;
            }

            SGT_PROP_GET(VNom, VNom, arma::Col<Complex>);

            virtual double VBase() const
            {
                return VBase_;
            }

            SGT_PROP_GET(VBase, VBase, double);

        /// @}

        /// @name Generators
        /// @{

            const std::vector<GenAbc*> gens() const {return genVec_;}

            void addGen(GenAbc& gen) {genVec_.push_back(&gen);}

            void removeAllGens() {genVec_.clear();}

            int nInServiceGens() const;

            SGT_PROP_GET(nInServiceGens, nInServiceGens, int);
            
            /// @brief Requested power of gens.
            arma::Col<Complex> SGenRequested() const;

            SGT_PROP_GET(SGenRequested, SGenRequested, arma::Col<Complex>);
            
            /// @brief Actual power of gens, including possible unserved generation.
            arma::Col<Complex> SGen() const;

            SGT_PROP_GET(SGen, SGen, arma::Col<Complex>);

            double JGen() const;
            
            SGT_PROP_GET(JGen, JGen, double);

        /// @}

        /// @name Zips
        /// @{

            const std::vector<ZipAbc*> zips() const {return zipVec_;}

            void addZip(ZipAbc& zip) {zipVec_.push_back(&zip);}

            void removeAllZips() {zipVec_.clear();}

            int nInServiceZips() const;

            SGT_PROP_GET(nInServiceZips, nInServiceZips, int);
            
            /// @brief Constant impedance component of zip.
            arma::Col<Complex> YConst() const;

            SGT_PROP_GET(YConst, YConst, arma::Col<Complex>);
            
            /// @brief Complex power from constant impedance component.
            arma::Col<Complex> SYConst() const;
            
            SGT_PROP_GET(SYConst, SYConst, arma::Col<Complex>);
            
            /// @brief Constant current component of zip.
            ///
            /// Relative to phase of V. Actual current will be IConst V / |V|, so that S doesn't depend on phase of V.
            arma::Col<Complex> IConst() const;
            
            SGT_PROP_GET(IConst, IConst, arma::Col<Complex>);
            
            /// @brief Complex power from constant current component of zip.
            /// 
            /// Independent of phase of V.
            arma::Col<Complex> SIConst() const;
            
            SGT_PROP_GET(SIConst, SIConst, arma::Col<Complex>);
            
            /// @brief Complex power component of zip.
            arma::Col<Complex> SConst() const;
            
            SGT_PROP_GET(SConst, SConst, arma::Col<Complex>);
            
            /// @brief Requested power of zips.
            arma::Col<Complex> SZipRequested() const;
            
            SGT_PROP_GET(SZipRequested, SZipRequested, arma::Col<Complex>);
           
            /// @brief Actual power of zips, including possible unserved load.
            arma::Col<Complex> SZip() const;
            
            SGT_PROP_GET(SZip, SZip, arma::Col<Complex>);

        /// @}

        /// @name Control and limits:
        /// @{

            virtual BusType type() const
            {
                return type_;
            }

            virtual void setType(BusType type)
            {
                type_ = type;
                setpointChanged_.trigger();
            }

            SGT_PROP_GET_SET(type, type, BusType, setType, BusType);

            virtual arma::Col<double> VMagSetpoint() const
            {
                return VMagSetpoint_;
            }

            virtual void setVMagSetpoint(const arma::Col<double>& VMagSetpoint)
            {
                VMagSetpoint_ = VMagSetpoint;
                setpointChanged_.trigger();
            }

            SGT_PROP_GET_SET(VMagSetpoint, VMagSetpoint, arma::Col<double>, setVMagSetpoint, const arma::Col<double>&);

            virtual arma::Col<double> VAngSetpoint() const
            {
                return VAngSetpoint_;
            }

            virtual void setVAngSetpoint(const arma::Col<double>& VAngSetpoint)
            {
                VAngSetpoint_ = VAngSetpoint;
                setpointChanged_.trigger();
            }

            SGT_PROP_GET_SET(VAngSetpoint, VAngSetpoint, arma::Col<double>, setVAngSetpoint, const arma::Col<double>&);

            virtual void applyVSetpoints();

            virtual double VMagMin() const
            {
                return VMagMin_;
            }

            virtual void setVMagMin(double VMagMin)
            {
                VMagMin_ = VMagMin;
                setpointChanged_.trigger();
            }

            SGT_PROP_GET_SET(VMagMin, VMagMin, double, setVMagMin, double);
            
            virtual double VMagMax() const
            {
                return VMagMax_;
            }

            virtual void setVMagMax(double VMagMax)
            {
                VMagMax_ = VMagMax;
                setpointChanged_.trigger();
            }

            SGT_PROP_GET_SET(VMagMax, VMagMax, double, setVMagMax, double);

        /// @}

        /// @name State
        /// @{

            virtual bool isInService() const
            {
                return isInService_;
            }

            virtual void setIsInService(bool isInService)
            {
                isInService_ = isInService;
                isInServiceChanged_.trigger();
            }

            SGT_PROP_GET_SET(isInService, isInService, bool, setIsInService, bool);

            virtual const arma::Col<Complex>& V() const
            {
                return V_;
            }

            SGT_PROP_GET(V, V, const arma::Col<Complex>&);

            virtual void setV(const arma::Col<Complex>& V)
            {
                V_ = V;
                voltageUpdated_.trigger();
            }

            virtual const arma::Col<Complex>& SGenUnserved() const
            {
                return SGenUnserved_;
            }

            SGT_PROP_GET(SGenUnserved, SGenUnserved, const arma::Col<Complex>&);

            virtual void setSGenUnserved(const arma::Col<Complex>& SGenUnserved)
            {
                SGenUnserved_ = SGenUnserved;
            }

            virtual const arma::Col<Complex>& SZipUnserved() const
            {
                return SZipUnserved_;
            }

            virtual void setSZipUnserved(const arma::Col<Complex>& SZipUnserved)
            {
                SZipUnserved_ = SZipUnserved;
            }

            SGT_PROP_GET(SZipUnserved, SZipUnserved, const arma::Col<Complex>&);

        /// @}
        
        /// @name coordinates.
        /// @{
      
            arma::Col<double> coords() const
            {
                return coords_;
            }
            
            void setCoords(const arma::Col<double>& coords)
            {
                coords_ = coords;
            }
            
            SGT_PROP_GET_SET(coords, coords, arma::Col<double>, setCoords, const arma::Col<double>&);

        /// @}

        /// @name Events.
        /// @{

            /// @brief Event triggered when I go in or out of service.
            virtual Event& isInServiceChanged()
            {
                return isInServiceChanged_;
            }

            /// @brief Event triggered when bus setpoint has changed.
            virtual Event& setpointChanged()
            {
                return setpointChanged_;
            }

            /// @brief Event triggered when bus state (e.g. voltage) has been updated.
            virtual Event& voltageUpdated()
            {
                return voltageUpdated_;
            }

        /// @}

        private:

            Phases phases_{Phase::BAL};
            arma::Col<Complex> VNom_;
            double VBase_{1.0};

            std::vector<GenAbc*> genVec_;

            std::vector<ZipAbc*> zipVec_;

            BusType type_{BusType::NA};
            arma::Col<double> VMagSetpoint_;
            arma::Col<double> VAngSetpoint_;
            double VMagMin_{-infinity};
            double VMagMax_{infinity};

            bool isInService_{true};

            arma::Col<Complex> V_;
            arma::Col<Complex> SGenUnserved_; // SGen + SGenUnserved = SGenRequested
            arma::Col<Complex> SZipUnserved_; // SZip + SZipUnserved = SZipRequested

            arma::Col<double>::fixed<2> coords_{{0.0, 0.0}};

            Event isInServiceChanged_{std::string(sComponentType()) + " : Is in service changed"};
            Event setpointChanged_{std::string(sComponentType()) + " : Setpoint changed"};
            Event voltageUpdated_{std::string(sComponentType()) + " : Voltage updated"};
    };
}

#endif // BUS_DOT_H
