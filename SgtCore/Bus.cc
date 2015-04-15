#include "Bus.h"

#include "Gen.h"
#include "Zip.h"

#include <ostream>
#include <istream>

namespace
{
    using namespace Sgt;
    template<typename T> std::istream& operator>>(std::istringstream& ss, const arma::Col<T>& v)
    {
        return ss;
    }
}

namespace Sgt
{
    Bus::Bus(const std::string& id, const Phases& phases, const arma::Col<Complex>& VNom, double VBase) :
        Component(id),
        phases_(phases),
        VNom_(VNom),
        VBase_(VBase),
        VMagSetpoint_(phases.size()),
        VAngSetpoint_(phases.size()),
        V_(VNom),
        SGenUnserved_(phases.size(), arma::fill::zeros),
        SZipUnserved_(phases.size(), arma::fill::zeros)
    {
        for (int i = 0; i < phases_.size(); ++i)
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

    arma::Col<Complex> Bus::SGenRequested() const
    {
        // Note: std::accumulate gave weird, hard to debug malloc errors under certain circumstances...
        // Easier to just do this.
        auto sum = arma::Col<Complex>(phases().size(), arma::fill::zeros);
        for (auto gen : genVec_)
        {
            if (gen->isInService())
            {
                sum += gen->S();
            }
        }
        return sum;
    }

    arma::Col<Complex> Bus::SGen() const
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

    arma::Col<Complex> Bus::YConst() const
    {
        // Note: std::accumulate gave weird, hard to debug malloc errors under certain circumstances...
        // Easier to just do this.
        auto sum = arma::Col<Complex>(phases().size(), arma::fill::zeros);
        for (auto zip : zipVec_)
        {
            sum += zip->YConst();
        }
        return sum;
    }

    arma::Col<Complex> Bus::SYConst() const
    {
        return -arma::conj(YConst()) % arma::conj(V()) % V();
    }

    arma::Col<Complex> Bus::IConst() const
    {
        // Note: std::accumulate gave weird, hard to debug malloc errors under certain circumstances...
        // Easier to just do this.
        auto sum = arma::Col<Complex>(phases().size(), arma::fill::zeros);
        for (auto zip : zipVec_)
        {
            sum += zip->IConst();
        }
        return sum;
    }

    arma::Col<Complex> Bus::SIConst() const
    {
        return arma::conj(IConst()) % V();
    }

    arma::Col<Complex> Bus::SConst() const
    {
        // Note: std::accumulate gave weird, hard to debug malloc errors under certain circumstances...
        // Easier to just do this.
        auto sum = arma::Col<Complex>(phases().size(), arma::fill::zeros);
        for (auto zip : zipVec_)
        {
            sum += zip->SConst();
        }
        return sum;
    }

    arma::Col<Complex> Bus::SZipRequested() const
    {
        return SYConst() + SIConst() + SConst();
    }

    arma::Col<Complex> Bus::SZip() const
    {
        return SZipRequested() - SZipUnserved_;
    }

    void Bus::applyVSetpoints()
    {
        arma::Col<Complex> VNew(phases_.size());
        switch (type_)
        {
            case BusType::SL:
                for (int i = 0; i < phases_.size(); ++i)
                {
                    VNew(i) = std::polar(VMagSetpoint_(i), VAngSetpoint_(i));
                }
                setV(VNew); // TODO: this triggers an event: is this desirable, or just set V_ directly?
                break;
            case BusType::PV:
                VNew = V_;
                for (int i = 0; i < phases_.size(); ++i)
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
    }
}
