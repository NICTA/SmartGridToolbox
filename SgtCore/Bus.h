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
#include <SgtCore/ComponentCollection.h>
#include <SgtCore/Event.h>
#include <SgtCore/PowerFlow.h>

#include<iostream>
#include<map>
#include<vector>

namespace Sgt
{
    class BranchAbc;
    class GenAbc;
    class ZipAbc;

    /// @brief A Bus is a grouped set of conductors / terminals, one per phase.
    /// @ingroup PowerFlowCore
    class Bus : virtual public Component
    {
        friend class Network;

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

            /// @name Control and limits:
            /// @{

            virtual BusType type() const
            {
                return type_;
            }

            virtual void setType(BusType type);

            SGT_PROP_GET_SET(type, type, BusType, setType, BusType);

            virtual arma::Col<double> VMagSetpoint() const
            {
                return VMagSetpoint_;
            }

            virtual void setVMagSetpoint(const arma::Col<double>& VMagSetpoint);

            SGT_PROP_GET_SET(VMagSetpoint, VMagSetpoint, arma::Col<double>, setVMagSetpoint, const arma::Col<double>&);

            virtual arma::Col<double> VAngSetpoint() const
            {
                return VAngSetpoint_;
            }

            virtual void setVAngSetpoint(const arma::Col<double>& VAngSetpoint);

            SGT_PROP_GET_SET(VAngSetpoint, VAngSetpoint, arma::Col<double>, setVAngSetpoint, const arma::Col<double>&);

            virtual void applyVSetpoints();

            virtual double VMagMin() const
            {
                return VMagMin_;
            }

            virtual void setVMagMin(double VMagMin);

            SGT_PROP_GET_SET(VMagMin, VMagMin, double, setVMagMin, double);
            
            virtual double VMagMax() const
            {
                return VMagMax_;
            }

            virtual void setVMagMax(double VMagMax);

            SGT_PROP_GET_SET(VMagMax, VMagMax, double, setVMagMax, double);

            /// @}

            /// @name State
            /// @{

            virtual bool isInService() const
            {
                return isInService_;
            }

            virtual void setIsInService(bool isInService);

            SGT_PROP_GET_SET(isInService, isInService, bool, setIsInService, bool);
            
            virtual bool isSupplied() const
            {
                return isSupplied_;
            }

            virtual void setIsSupplied(bool isSupplied);

            SGT_PROP_GET_SET(isSupplied, isSupplied, bool, setIsSupplied, bool);

            virtual const arma::Col<Complex>& V() const
            {
                return V_;
            }

            SGT_PROP_GET(V, V, const arma::Col<Complex>&);

            virtual void setV(const arma::Col<Complex>& V);

            virtual const arma::Col<Complex>& SGenUnserved() const
            {
                return SGenUnserved_;
            }

            SGT_PROP_GET(SGenUnserved, SGenUnserved, const arma::Col<Complex>&);

            virtual void setSGenUnserved(const arma::Col<Complex>& SGenUnserved)
            {
                SGenUnserved_ = SGenUnserved;
            }

            virtual const arma::Mat<Complex>& SZipUnserved() const
            {
                return SZipUnserved_;
            }

            virtual void setSZipUnserved(const arma::Mat<Complex>& SZipUnserved)
            {
                SZipUnserved_ = SZipUnserved;
            }

            SGT_PROP_GET(SZipUnserved, SZipUnserved, const arma::Mat<Complex>&);

            /// @}
        
            /// @name Island id.
            /// @{
            
            int islandIdx() const
            {
                return islandIdx_;
            }
            
            void setIslandIdx(int islandIdx)
            {
                islandIdx_ = islandIdx;
            }

            SGT_PROP_GET_SET(islandIdx, islandIdx, int, setIslandIdx, int);

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
            virtual const Event& isInServiceChanged() const
            {
                return isInServiceChanged_;
            }
            
            /// @brief Event triggered when I go in or out of service.
            virtual Event& isInServiceChanged()
            {
                return isInServiceChanged_;
            }

            /// @brief Event triggered when my supplied state changes.
            virtual const Event& isSuppliedChanged() const
            {
                return isSuppliedChanged_;
            }
            
            /// @brief Event triggered when my supplied state changes.
            virtual Event& isSuppliedChanged()
            {
                return isSuppliedChanged_;
            }

            /// @brief Event triggered when bus setpoint has changed.
            virtual const Event& setpointChanged() const
            {
                return setpointChanged_;
            }
            
            /// @brief Event triggered when bus setpoint has changed.
            virtual Event& setpointChanged()
            {
                return setpointChanged_;
            }

            /// @brief Event triggered when bus state (e.g. voltage) has been updated.
            virtual const Event& voltageUpdated() const
            {
                return voltageUpdated_;
            }
            
            /// @brief Event triggered when bus state (e.g. voltage) has been updated.
            virtual Event& voltageUpdated()
            {
                return voltageUpdated_;
            }

            /// @}
            
            /// @name Branches
            /// @{
            
            const ComponentCollection<BranchAbc>& branches0() const
            {
                return branches0_;
            }
            ComponentCollection<BranchAbc>& branches0()
            {
                return branches0_;
            }
            
            const ComponentCollection<BranchAbc>& branches1() const
            {
                return branches1_;
            }
            ComponentCollection<BranchAbc>& branches1()
            {
                return branches1_;
            }

            /// @}

            /// @name Generators
            /// @{

            const ComponentCollection<GenAbc>& gens() const
            {
                return gens_;
            }
            ComponentCollection<GenAbc>& gens()
            {
                return gens_;
            }

            int nInServiceGens() const;

            SGT_PROP_GET(nInServiceGens, nInServiceGens, int);
            
            /// @brief Requested power of gens.
            arma::Col<Complex> SGenRequested() const;

            SGT_PROP_GET(SGenRequested, SGenRequested, arma::Col<Complex>);
            
            /// @brief Actual of gens, including possible unserved generation.
            arma::Col<Complex> SGen() const
            {
                return SGenRequested() - SGenUnserved_;
            }

            SGT_PROP_GET(SGen, SGen, arma::Col<Complex>);

            /// @brief Sum of actual generation at bus.
            Complex SGenTot() const
            {
                return sum(SGen());
            }

            SGT_PROP_GET(SGenTot, SGenTot, Complex);

            double JGen() const;
            
            SGT_PROP_GET(JGen, JGen, double);

            /// @}

            /// @name Zips
            /// @{

            const ComponentCollection<ZipAbc>& zips() const
            {
                return zips_;
            }
            ComponentCollection<ZipAbc>& zips()
            {
                return zips_;
            }

            int nInServiceZips() const;

            SGT_PROP_GET(nInServiceZips, nInServiceZips, int);
            
            /// @brief Constant impedance component of zip, as described in Zip documentation.
            arma::Mat<Complex> YConst() const;

            SGT_PROP_GET(YConst, YConst, arma::Mat<Complex>);
            
            /// @brief Complex power from constant impedance component.
            arma::Mat<Complex> SYConst() const;
            
            SGT_PROP_GET(SYConst, SYConst, arma::Mat<Complex>);
            
            /// @brief Constant current component of zip, as described in Zip documentation.
            ///
            /// Relative to phase of V. Actual current will be IConst V / |V|, so that S doesn't depend on phase of V.
            arma::Mat<Complex> IConst() const;
            
            SGT_PROP_GET(IConst, IConst, arma::Mat<Complex>);
            
            /// @brief Complex power from constant current component of zip.
            /// 
            /// Independent of phase of V.
            arma::Mat<Complex> SIConst() const;
            
            SGT_PROP_GET(SIConst, SIConst, arma::Mat<Complex>);
            
            /// @brief Complex power component of zip, as described in Zip documentation.
            arma::Mat<Complex> SConst() const;
            
            SGT_PROP_GET(SConst, SConst, arma::Mat<Complex>);
            
            /// @brief Requested power of zips.
            arma::Mat<Complex> SZipRequested() const
            {
                return SYConst() + SIConst() + SConst();
            }
            
            SGT_PROP_GET(SZipRequested, SZipRequested, arma::Mat<Complex>);
           
            /// @brief Actual power of zips, including possible unserved load.
            arma::Mat<Complex> SZip() const
            {
                return SZipRequested() - SZipUnserved_;
            }
            
            SGT_PROP_GET(SZip, SZip, arma::Mat<Complex>);
           
            /// @brief Scalar actual power consumed at bus.
            Complex SZipTot() const
            {
                return sum(sum(trimatu(SZip())));
            }

            SGT_PROP_GET(SZipTot, SZipTot, Complex);

            /// @}

        private:

            Phases phases_{Phase::BAL};
            arma::Col<Complex> VNom_;
            double VBase_{1.0};

            BusType type_{BusType::NA};
            arma::Col<double> VMagSetpoint_;
            arma::Col<double> VAngSetpoint_;
            double VMagMin_{-infinity};
            double VMagMax_{infinity};

            bool isInService_{true};
            bool isSupplied_{true};
            arma::Col<Complex> V_;
            arma::Col<Complex> SGenUnserved_; // SGen + SGenUnserved = SGenRequested
            arma::Mat<Complex> SZipUnserved_; // SZip + SZipUnserved = SZipRequested

            int islandIdx_{-1};

            arma::Col<double>::fixed<2> coords_{{0.0, 0.0}};

            Event isInServiceChanged_{std::string(sComponentType()) + " : Is in service changed"};
            Event isSuppliedChanged_{std::string(sComponentType()) + " : Is supplied changed"};
            Event setpointChanged_{std::string(sComponentType()) + " : Setpoint changed"};
            Event voltageUpdated_{std::string(sComponentType()) + " : Voltage updated"};
            
            MutableComponentCollection<BranchAbc> branches0_;
            MutableComponentCollection<BranchAbc> branches1_;
            MutableComponentCollection<GenAbc> gens_;
            MutableComponentCollection<ZipAbc> zips_;
    };
}

#endif // BUS_DOT_H
