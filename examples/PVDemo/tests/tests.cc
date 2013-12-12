#define BOOST_TEST_MODULE test_template
#include <boost/test/included/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(tests) // Name of test suite is test_template.

BOOST_AUTO_TEST_CASE(test_dummy)
{
   BOOST_CHECK(1 == 1);
}

BOOST_AUTO_TEST_SUITE_END( )
