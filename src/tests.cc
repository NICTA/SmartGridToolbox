#define BOOST_TEST_MODULE test_template
#include <boost/test/included/unit_test.hpp>

BOOST_AUTO_TEST_SUITE (test_template) // Name of test suite is test_template.

BOOST_AUTO_TEST_CASE (test1)
{
  BOOST_CHECK(0 == 0);
}

BOOST_AUTO_TEST_SUITE_END( )
