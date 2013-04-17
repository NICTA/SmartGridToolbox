#define BOOST_TEST_MODULE test_template
#include <boost/test/included/unit_test.hpp>
#include "WeakOrder.h"
using namespace SmartGridToolbox;
using namespace std;

BOOST_AUTO_TEST_SUITE (tests) // Name of test suite is test_template.

BOOST_AUTO_TEST_CASE (test_weak_order)
{
   WOGraph g(6);
   g.Link(0, 4);
   g.Link(0, 5);
   g.Link(1, 2);
   g.Link(1, 0);
   g.Link(3, 1);
   g.Link(4, 1);
   g.Link(5, 2);
   // Such a graph should have the weak order 3, (4, 1, 0), 5, 2.

   g.WeakOrder();

   cout << "Precedes: " << endl;
   cout << "   ";
   for (int i = 0; i < 6; ++i)
   {
      cout << " " << g[i].GetIndex(); 
   }
   cout << endl;
   for (const WONode * nd1 : g)
   {
      cout << nd1->GetIndex() << "   ";
      for (const WONode * nd2 : g)
      {
         std::cout << nd1->Precedes(*nd2) << " ";
      }
      std::cout << endl;
   }
   cout << endl;
   cout << "Dominates: " << endl;
   cout << "   ";
   for (int i = 0; i < 6; ++i)
   {
      cout << " " << g[i].GetIndex(); 
   }
   cout << endl;
   for (const WONode * nd1 : g)
   {
      cout << nd1->GetIndex() << "   ";
      for (const WONode * nd2 : g)
      {
         std::cout << nd1->Dominates(*nd2) << " ";
      }
      std::cout << endl;
   }
   BOOST_CHECK(g[0].GetIndex() == 3);
   BOOST_CHECK(g[1].GetIndex() == 4);
   BOOST_CHECK(g[2].GetIndex() == 1);
   BOOST_CHECK(g[3].GetIndex() == 0);
   BOOST_CHECK(g[4].GetIndex() == 5);
   BOOST_CHECK(g[5].GetIndex() == 2);
}
   
BOOST_AUTO_TEST_SUITE_END( )
