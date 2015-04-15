#include "Gen.h"

#include <ostream>

namespace Sgt
{
    void GenAbc::print(std::ostream& os) const
    {
        Component::print(os);
        StreamIndent _(os);
        os << "phases: " << phases() << std::endl;
        os << "S: " << S() << std::endl;
        os << "PMin: " << PMin() << std::endl;
        os << "PMax: " << PMax() << std::endl;
        os << "QMin: " << QMin() << std::endl;
        os << "QMax: " << QMax() << std::endl;
        os << "cStartup : " << cStartup() << std::endl;
        os << "cShutdown: " << cShutdown() << std::endl;
        os << "c0: " << c0() << std::endl;
        os << "c1: " << c1() << std::endl;
        os << "c2: " << c2() << std::endl;
    }

    double GenAbc::cost() const
    {
        double P = 0;
        for (const auto& Pi : S())
        {
            P += Pi.real();
        }
        return c0() + c1() * P + c2() * P * P;
    }
}
