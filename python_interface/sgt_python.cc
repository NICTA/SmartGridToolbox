#include <boost/python.hpp>

using namespace boost::python;

char const* greet()
{
   return "hello, world";
}
 
BOOST_PYTHON_MODULE(sgt_python)
{
    using namespace boost::python;
    def("greet", greet);
}
