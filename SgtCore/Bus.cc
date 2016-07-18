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
        SZipUnserved_(phases.size(), phases.size(), fill::zeros)
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
        auto sum = Col<Complex>(phases().size(), fill::zeros);
        for (auto gen : genVec_)
        {
            if (gen->isInService())
            {
                sum += mapPhases(gen->inServiceS(), gen->phases(), phases_);
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

    Mat<Complex> Bus::YConst() const
    {
        // Note: std::accumulate gave weird, hard to debug malloc errors under certain circumstances...
        // Easier to just do this.
        auto sum = Mat<Complex>(phases().size(), phases().size(), fill::zeros);
        for (auto zip : zipVec_)
        {
            if (zip->isInService())
            {
                sum += mapPhases(zip->YConst(), zip->phases(), phases_);
            }
        }
        return sum;
    }

    Mat<Complex> Bus::SYConst() const
    {
        Mat<Complex> result = -conj(YConst());
        for (uword i = 0; i < result.n_rows; ++i)
        {
            for (uword k = 0; k < result.n_cols; ++k)
            {
                if (result(i, k) != Complex(0.0))
                {
                    Complex Vik = i == k ? V_(i) : V_(i) - V_(k);
                    result(i, k) *= norm(Vik); // std::norm gives |Vik|^2
                }
            }
        }
        return result;
    }

    Mat<Complex> Bus::IConst() const
    {
        // Note the returned current is relative to the phase of V.
        auto sum = Mat<Complex>(phases().size(), phases().size(), fill::zeros);
        for (auto zip : zipVec_)
        {
            if (zip->isInService())
            {
                sum += mapPhases(zip->IConst(), zip->phases(), phases_);
            }
        }
        return sum;
    }

    Mat<Complex> Bus::SIConst() const
    {
        Mat<Complex> result = -conj(IConst());
        for (uword i = 0; i < result.n_rows; ++i)
        {
            for (uword k = 0; k < result.n_cols; ++k)
            {
                if (result(i, k) != Complex(0.0))
                {
                    Complex Vik = i == k ? V_(i) : V_(i) - V_(k);
                    result(i, k) *= abs(Vik); // std::norm gives |Vik|^2
                }
            }
        }
        return result;
    }

    Mat<Complex> Bus::SConst() const
    {
        auto sum = Mat<Complex>(phases().size(), phases().size(), fill::zeros);
        for (auto zip : zipVec_)
        {
            if (zip->isInService())
            {
                sum += mapPhases(zip->SConst(), zip->phases(), phases_);
            }
        }
        return sum;
    }

    Mat<Complex> Bus::SZipRequested() const
    {
        return SYConst() + SIConst() + SConst();
    }

    Mat<Complex> Bus::SZip() const
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

    json Bus::toJson() const
    {
        json j = this->Component::toJson();

        json gensJson = json::array();
        for (const auto& g : gens())
        {
            gensJson.push_back(g->id());
        }
        
        json zipsJson = json::array();
        for (const auto& z : zips())
        {
            zipsJson.push_back(z->id());
        }

        j[sComponentType()] = {
            {"phases", phases()},
            {"type", type()},
            {"V_base", VBase()},
            {"V_nom", VNom()},
            {"V_mag_min", VMagMin()},
            {"V_mag_max", VMagMax()},
            {"V", V()},
            {"islandIdx", islandIdx()},
            {"coords", coords()},
            {"gens", gensJson},
            {"zips", zipsJson}};
        return j;
    }
}
