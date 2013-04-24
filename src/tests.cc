#define BOOST_TEST_MODULE test_template
#include <boost/test/included/unit_test.hpp>
#include <ostream>
#include "Component.h"
#include "Model.h"
#include "Output.h"
#include "SimpleBattery.h"
#include "Simulation.h"
#include "Spline.h"
#include "WeakOrder.h"
using namespace SmartGridToolbox;
using namespace std;

class TestCompA : public Component
{
   public:
      TestCompA(const string & name, int x, double y) : 
         Component(name), x_(x), y_(y)
      {
         addReadProperty<int>(std::string("x"), [this](){return x_;});
      }

      virtual void advanceToTime(ptime t)
      {
         std::cout << "TCA AdvanceToTimestep " << t << std::endl;
      }

      int getX() {return x_;}
      void setX(int x) {x_ = x;}

      double getY() {return y_;}
      void setY(double y) {y_ = y;}

   private:
      int x_;
      double y_;
};

BOOST_AUTO_TEST_SUITE (tests) // Name of test suite is test_template.

BOOST_AUTO_TEST_CASE (test_weak_order)
{
   message("Testing weak ordering. Starting.");
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
      cout << " " << g.getNodes()[i]->getIndex() << endl;
   }
   cout << endl;

   cout << "   ";
   for (int i = 0; i < g.size(); ++i)
   {
      cout << " " << g.getNodes()[i]->getIndex(); 
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

   BOOST_CHECK(g.getNodes()[0]->getIndex() == 3);
   BOOST_CHECK(g.getNodes()[1]->getIndex() == 4);
   BOOST_CHECK(g.getNodes()[2]->getIndex() == 1);
   BOOST_CHECK(g.getNodes()[3]->getIndex() == 0);
   BOOST_CHECK(g.getNodes()[4]->getIndex() == 5);
   BOOST_CHECK(g.getNodes()[5]->getIndex() == 2);
   message("Testing weak ordering. Completed.");
}

BOOST_AUTO_TEST_CASE (test_model_dependencies)
{
   message("Testing model dependencies. Starting.");
   Model mod;
   Simulation sim(mod);

   TestCompA * a0 = new TestCompA("tca0", 0, 0.1);
   TestCompA * a1 = new TestCompA("tca1", 1, 0.1);
   TestCompA * a2 = new TestCompA("tca2", 2, 0.1);
   TestCompA * a3 = new TestCompA("tca3", 3, 0.1);
   TestCompA * a4 = new TestCompA("tca4", 4, 0.1);
   TestCompA * a5 = new TestCompA("tca5", 5, 0.1);

   a4->addDependency(*a0);
   a5->addDependency(*a0);
   a0->addDependency(*a1);
   a2->addDependency(*a1);
   a1->addDependency(*a3);
   a1->addDependency(*a4);
   a2->addDependency(*a5);

   mod.addComponent(*a0);
   mod.addComponent(*a1);
   mod.addComponent(*a2);
   mod.addComponent(*a3);
   mod.addComponent(*a4);
   mod.addComponent(*a5);

   mod.validate();

   BOOST_CHECK(mod.getComponents()[0] == a3);
   BOOST_CHECK(mod.getComponents()[1] == a4);
   BOOST_CHECK(mod.getComponents()[2] == a1);
   BOOST_CHECK(mod.getComponents()[3] == a0);
   BOOST_CHECK(mod.getComponents()[4] == a5);
   BOOST_CHECK(mod.getComponents()[5] == a2);

   delete a0;
   delete a1;
   delete a2;
   delete a3;
   delete a4;
   delete a5;
   message("Testing model dependencies. Completed.");
}

BOOST_AUTO_TEST_CASE (test_properties)
{
   message("Testing properties. Starting.");
   TestCompA * tca = new TestCompA("tca0", 3, 0.2);
   const ReadProperty<int> * prop1 = tca->getReadProperty<int>("x");
   cout << "Property operator(): " << (*prop1)() << endl;
   cout << "Property implicit conversion: " << (*prop1) << endl;
   BOOST_CHECK(*prop1 == 3);
   tca->addReadWriteProperty<double>(
         "y",
         [&](){return tca->getY();}, 
         [&](const double & d){tca->setY(d);});
   ReadWriteProperty<double> * prop2 = tca->getReadWriteProperty<double>("y");
   BOOST_CHECK(prop2 != nullptr);
   BOOST_CHECK(*prop2 == 0.2);
   cout << "Property 2 " << (*prop2) << endl;
   BOOST_CHECK(*prop1 == 3);
   //(*prop2) = 0.4;
   prop2->set(0.4);
   cout << "Property 2 " << (*prop2) << endl;
   BOOST_CHECK(*prop2 == 0.4);
   delete tca;
   message("Testing properties. Completed.");
}

BOOST_AUTO_TEST_CASE (test_simple_battery)
{
   message("Testing SimpleBattery. Starting.");
   SimpleBattery bat1("bat1");
   bat1.setInitCharge(5.0 * kWh);
   bat1.setMaxCharge(8.0 * kWh);
   bat1.setMaxChargePower(1.0 * kW);
   bat1.setMaxDischargePower(2.1 * kW);
   bat1.setChargeEfficiency(0.9);
   bat1.setDischargeEfficiency(0.8);
   bat1.setRequestedPower(-0.4 * kW);
   bat1.initializeComponent(ptime(date(2012, Feb, 11), hours(2)));
   cout << "1 Battery charge = " << bat1.getCharge() / kWh << endl;
   bat1.advanceComponent(bat1.getTimestamp() + hours(3));
   cout << "2 Battery charge = " << bat1.getCharge() / kWh << endl;
   double comp = bat1.getInitCharge() + 
      dseconds(hours(3)) * bat1.getRequestedPower() /
      bat1.getDischargeEfficiency();
   cout << "comp = " << comp / kWh << endl;
   BOOST_CHECK(bat1.getCharge() == comp);

   bat1.setRequestedPower(1.3 * kW);
   bat1.initializeComponent(ptime(date(2012, Feb, 11), hours(2)));
   cout << "3 Battery charge = " << bat1.getCharge() / kWh << endl;
   bat1.advanceComponent(bat1.getTimestamp() + hours(3));
   cout << "4 Battery charge = " << bat1.getCharge() / kWh << endl;
   comp = bat1.getInitCharge() + 
      dseconds(hours(3)) * bat1.getMaxChargePower() *
      bat1.getChargeEfficiency();
   cout << "comp = " << comp / kWh << endl;
   BOOST_CHECK(bat1.getCharge() == comp);

   bat1.setRequestedPower(-1 * kW);
   bat1.initializeComponent(ptime(date(2012, Feb, 11), hours(2)));
   cout << "3 Battery charge = " << bat1.getCharge() / kWh << endl;
   bat1.advanceComponent(bat1.getTimestamp() + hours(5.5));
   cout << "4 Battery charge = " << bat1.getCharge() / kWh << endl;
   comp = 0.0;
   cout << "comp = " << comp / kWh << endl;
   BOOST_CHECK(bat1.getCharge() == comp);
   message("Testing SimpleBattery. Completed.");
}

BOOST_AUTO_TEST_CASE (test_spline)
{
  Spline spline;
  //Add points to the spline in any order, they're sorted in ascending
  //x later. (If you want to spline a circle you'll need to change the class)
  spline.addPoint(0,        0.0);
  spline.addPoint(40.0/255, 0.0);
  spline.addPoint(60.0/255, 0.2);
  spline.addPoint(63.0/255, 0.05);
  spline.addPoint(80.0/255, 0.0);
  spline.addPoint(82.0/255, 0.9);
  spline.addPoint(1.0, 1.0);

  { //We can extract the original data points by treating the spline as
    //a read-only STL container.
    std::ofstream of("orig.dat");
    for (Spline::const_iterator iPtr = spline.begin();
	 iPtr != spline.end(); ++iPtr)
      of << iPtr->first << " " << iPtr->second << "\n";
  }
  
  { //A "natural spline" where the second derivatives are set to 0 at the boundaries.

    //Each boundary condition is set seperately

    //The following aren't needed as its the default setting. The 
    //zero values are the second derivatives at the spline points.
    spline.setLowBC(Spline::FIXED_2ND_DERIV_BC, 0);
    spline.setHighBC(Spline::FIXED_2ND_DERIV_BC, 0);

    //Note: We can calculate values outside the range spanned by the
    //points. The extrapolation is based on the boundary conditions
    //used.

    std::ofstream of("spline.natural.dat");
    for (double x(-0.2); x <= 1.2001; x += 0.005)
      of << x << " " << spline(x) << "\n";
  }
}   

BOOST_AUTO_TEST_SUITE_END( )
