#include <SgtCore.h>
#include <SgtSim.h>

#include <boost/iostreams/stream.hpp>
#include <boost/python.hpp>

#include <iostream>

BOOST_PYTHON_MODULE(sgt)
{
    boost::python::class_<Sgt::Network, boost::noncopyable>("Network", boost::python::init<double>())
        .def(str(boost::python::self));
}
