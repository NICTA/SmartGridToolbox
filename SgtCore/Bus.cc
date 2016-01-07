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

#include "Bus.h"

#include "Gen.h"
#include "Zip.h"

#include <ostream>
#include <istream>

using namespace arma;

namespace
{
    using namespace Sgt;
    template<typename T> std::istream& operator>>(std::istringstream& ss, const Col<T>& v)
    {
        return ss;
    }
}

namespace Sgt
{
    Bus::Bus(const std::string& id, const Phases& phases, const Col<Complex>& VNom, double VBase) :
        Component(id),
        phases_(phases),
        VNom_(VNom),
        VBase_(VBase),
        VMagSetpoint_(phases.size()),
        VAngSetpoint_(phases.size()),
        V_(VNom),
        SGenUnserved_(phases.size(), fill::zeros),
        SZipUnserved_(phases.size(), fill::zeros)
    {
        for (uword i = 0; i < phases_.size(); ++i)
        {
            VMagSetpoint_(i) = std::abs(VNom_(i));
            VAngSetpoint_(i) = std::arg(VNom_(i));
        }
    }

    int Bus::nInServiceGens() const
    {
        int sum = 0;
        for (auto gen : genVec_)
        {
            if (gen->isInService())
            {
                ++sum;
            }
        }
        return sum;
    }

    Col<Complex> Bus::SGenRequested() const
    {
        // Note: std::accumulate gave weird, hard to debug malloc errors under certain circumstances...
        // Easier to just do this.
        auto sum = Col<Complex>(phases().size(), fill::zeros);
        for (auto gen : genVec_)
        {
            if (gen->isInService())
            {
                sum += gen->inServiceS();
            }
        }
        return sum;
    }

    Col<Complex> Bus::SGen() const
    {
        return SGenRequested() - SGenUnserved_;
    }

    double Bus::JGen() const
    {
        // Note: std::accumulate gave weird, hard to debug malloc errors under certain circumstances...
        // Easier to just do this.
        double sum = 0;
        for (auto gen : genVec_)
        {
            if (gen->isInService())
            {
                sum += gen->J();
            }
        }
        return sum;
    }

    int Bus::nInServiceZips() const
    {
        int sum = 0;
        for (auto zip : zipVec_)
        {
            if (zip->isInService())
            {
                ++sum;
            }
        }
        return sum;
    }

    Col<Complex> Bus::YConst() const
    {
        // Note: std::accumulate gave weird, hard to debug malloc errors under certain circumstances...
        // Easier to just do this.
        auto sum = Col<Complex>(phases().size(), fill::zeros);
        for (auto zip : zipVec_)
        {
            sum += zip->YConst();
        }
        return sum;
    }

    Col<Complex> Bus::SYConst() const
    {
        return -conj(YConst()) % conj(V()) % V();
    }

    Col<Complex> Bus::IConst() const
    {
        // Note the returned current is relative to the phase of V.
        
        // Note: std::accumulate gave weird, hard to debug malloc errors under certain circumstances...
        // Easier to just do this.
        auto sum = Col<Complex>(phases().size(), fill::zeros);
        for (auto zip : zipVec_)
        {
            sum += zip->IConst();
        }
        return sum;
    }

    Col<Complex> Bus::SIConst() const
    {
        // Note special phase relationship with V.
        return conj(IConst()) % abs(V());
    }

    Col<Complex> Bus::SConst() const
    {
        // Note: std::accumulate gave weird, hard to debug malloc errors under certain circumstances...
        // Easier to just do this.
        auto sum = Col<Complex>(phases().size(), fill::zeros);
        for (auto zip : zipVec_)
        {
            sum += zip->SConst();
        }
        return sum;
    }

    Col<Complex> Bus::SZipRequested() const
    {
        return SYConst() + SIConst() + SConst();
    }

    Col<Complex> Bus::SZip() const
    {
        return SZipRequested() - SZipUnserved_;
    }

    void Bus::applyVSetpoints()
    {
        Col<Complex> VNew(phases_.size());
        switch (type_)
        {
            case BusType::SL:
                for (uword i = 0; i < phases_.size(); ++i)
                {
                    VNew(i) = std::polar(VMagSetpoint_(i), VAngSetpoint_(i));
                }
                setV(VNew); // TODO: this triggers an event: is this desirable, or just set V_ directly?
                break;
            case BusType::PV:
                VNew = V_;
                for (uword i = 0; i < phases_.size(); ++i)
                {
                    VNew(i) *= VMagSetpoint_(i) / std::abs(V_(i));
                }
                setV(VNew);
                break;
            default:
                break;
        }
    }

    void Bus::print(std::ostream& os) const
    {
        Component::print(os);
        StreamIndent _(os);
        os << "phases: " << phases() << std::endl;
        os << "type: " << type() << std::endl;
        os << "V_base: " << VBase() << std::endl;
        os << "V_nom: " << VNom() << std::endl;
        os << "V_mag_min: " << VMagMin() << std::endl;
        os << "V_mag_max: " << VMagMax() << std::endl;
        os << "V: " << V() << std::endl;
        os << "coords: " << coords() << std::endl;
    }
    
    json Bus::asJson() const
    {
        json j = this->Component::asJson();
        j[sComponentType()] = {
            {"phases", toJson(phases())},
            {"type", toJson(type())},
            {"V_base", VBase()},
            {"V_nom", toJson(VNom())},
            {"V_mag_min", VMagMin()},
            {"V_mag_max", VMagMax()},
            {"V", toJson(V())},
            {"coords", toJson(coords())}};
        return j;
    }
}
