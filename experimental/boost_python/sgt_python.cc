#include <boost/python.hpp>

#include <SgtCore.h>
#include <SgtSim.h>

#include <iostream>

using namespace boost::python;

void test()
{
    using namespace Sgt;
    Network netw;
    std::cout << netw.busses().size() << std::endl;
}
 
BOOST_PYTHON_MODULE(sgt_python)
{
    using namespace boost::python;
    def("test", test);
}
