#define BOOST_TEST_MODULE test_template
#include <boost/test/included/unit_test.hpp>
#include "WeakOrder.h"
using namespace SmartGridToolbox;
using namespace std;

BOOST_AUTO_TEST_SUITE (tests) // Name of test suite is test_template.

BOOST_AUTO_TEST_CASE (test_weak_order)
{
   WOGraph g(6);
   g.Link(3, 1);
   g.Link(4, 1);
   g.Link(0, 4);
   g.Link(1, 0);
   g.Link(1, 2);
   g.Link(0, 5);
   g.Link(5, 2);
   // Order should be 3, (4, 1, 0), 5, 2.
   g.WeakOrder();
   for (int i = 0; i < g.Size(); ++i)
   {
      cout << " " << g[i].Index() << endl;
   }
   cout << endl;

   cout << "   ";
   for (int i = 0; i < g.Size(); ++i)
   {
      cout << " " << g[i].Index(); 
   }
   cout << endl << endl;
   for (const WONode * nd1 : g)
   {
      cout << nd1->Index() << "   ";
      for (const WONode * nd2 : g)
      {
         std::cout << nd1->Dominates(*nd2) << " ";
      }
      std::cout << endl;
   }

   BOOST_CHECK(g[0].Index() == 3);
   BOOST_CHECK(g[1].Index() == 4);
   BOOST_CHECK(g[2].Index() == 1);
   BOOST_CHECK(g[3].Index() == 0);
   BOOST_CHECK(g[4].Index() == 5);
   BOOST_CHECK(g[5].Index() == 2);
}
   
BOOST_AUTO_TEST_SUITE_END( )
