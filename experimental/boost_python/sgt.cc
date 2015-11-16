#include <SgtCore.h>
#include <SgtSim.h>

#include <boost/iostreams/stream.hpp>
#include <boost/python.hpp>

#include <iostream>

using namespace Sgt;
using namespace boost::python;

namespace 
{
    inline Phases phaseOr(const Phase& pl, const Phase& pr)
    {
        return pl | pr;
    }

    inline Complex test()
    {
        return {0.1, 0.9};
    }
}

BOOST_PYTHON_MODULE(sgt)
{
    using PyPhase = enum_<Phase>;
    PyPhase("Phase")
        .value("BAL", Phase::BAL)
        .value("A", Phase::A)
        .value("B", Phase::B)
        .value("C", Phase::C)
        .value("G", Phase::G)
        .value("N", Phase::N)
        .value("SP", Phase::SP)
        .value("SM", Phase::SM)
        .value("SN", Phase::SN)
        .value("BAD", Phase::BAD);

    using PyPhases = class_<Phases>;
    PyPhases("Phases", init<>())
        .def(init<unsigned int>())
        .def(init<Phase>())
        .def("__init__", make_constructor(phaseOr))
        .def(str(self))
        .def(self | self)
        .def(self | Phase::BAD);

    class_<Network, boost::noncopyable>("Network", init<double>())
        .def(str(self));

    def("test", test);
}
