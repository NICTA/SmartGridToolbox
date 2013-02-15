#define BOOST_TEST_MODULE test_template
#include <boost/test/included/unit_test.hpp>
#include <boost/numeric/odeint.hpp>

using namespace boost::numeric::odeint;
typedef std::vector<double> state_type;
void test_ode(const state_type & x, state_type & dxdt, const double t)
{
   dxdt[0] = -x[0];
}
void write_test_ode(const state_type & x, const double t)
{
   std::cout << t << " " << x[0] << std::endl;
}

BOOST_AUTO_TEST_SUITE (tests) // Name of test suite is test_template.

BOOST_AUTO_TEST_CASE (test_odeint)
{
   state_type x(1);
   x[0] = 1;
   size_t steps = integrate(test_ode, x, 0.0, 10.0, 0.1, write_test_ode);
   BOOST_CHECK(0 == 0);
}

BOOST_AUTO_TEST_SUITE_END( )
