#define BOOST_TEST_MODULE test_template
#include <boost/test/included/unit_test.hpp>
#include "WeakOrder.h"
using namespace SmartGridToolbox;
using namespace std;

BOOST_AUTO_TEST_SUITE (tests) // Name of test suite is test_template.

BOOST_AUTO_TEST_CASE (test_weak_order)
{
   WoGraph g(6);
   g.link(3, 1);
   g.link(4, 1);
   g.link(0, 4);
   g.link(1, 0);
   g.link(1, 2);
   g.link(0, 5);
   g.link(5, 2);
   // Order should be 3, (4, 1, 0), 5, 2.
   g.weakOrder();
   for (int i = 0; i < g.size(); ++i)
   {
      cout << " " << g[i].getIndex() << endl;
   }
   cout << endl;

   cout << "   ";
   for (int i = 0; i < g.size(); ++i)
   {
      cout << " " << g[i].getIndex(); 
   }
   cout << endl << endl;
   for (const WoNode * nd1 : g.getNodes())
   {
      cout << nd1->getIndex() << "   ";
      for (const WoNode * nd2 : g.getNodes())
      {
         std::cout << nd1->dominates(*nd2) << " ";
      }
      std::cout << endl;
   }

   BOOST_CHECK(g[0].getIndex() == 3);
   BOOST_CHECK(g[1].getIndex() == 4);
   BOOST_CHECK(g[2].getIndex() == 1);
   BOOST_CHECK(g[3].getIndex() == 0);
   BOOST_CHECK(g[4].getIndex() == 5);
   BOOST_CHECK(g[5].getIndex() == 2);
}
   
BOOST_AUTO_TEST_SUITE_END( )
