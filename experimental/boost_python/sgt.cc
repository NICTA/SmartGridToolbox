#include <SgtCore.h>
#include <SgtSim.h>

#include <boost/iostreams/stream.hpp>
#include <boost/python.hpp>

#include <iostream>

BOOST_PYTHON_MODULE(sgt)
{
    boost::python::enum_<Sgt::Phase>("Phase")
        .value("BAL", Sgt::Phase::BAL)
        .value("A", Sgt::Phase::A)
        .value("B", Sgt::Phase::B)
        .value("C", Sgt::Phase::C)
        .value("G", Sgt::Phase::G)
        .value("N", Sgt::Phase::N)
        .value("SP", Sgt::Phase::SP)
        .value("SM", Sgt::Phase::SM)
        .value("SN", Sgt::Phase::SN)
        .value("BAD", Sgt::Phase::BAD);

    boost::python::class_<Sgt::Phases>("Phases", boost::python::init<>())
        .def(boost::python::init<unsigned int>())
        .def(boost::python::init<Sgt::Phase>())
        .def(str(boost::python::self));

    boost::python::class_<Sgt::Network, boost::noncopyable>("Network", boost::python::init<double>())
        .def(str(boost::python::self));
}
