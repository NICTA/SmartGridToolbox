#define BOOST_TEST_MODULE test_template
#include <boost/test/included/unit_test.hpp>
#include <ostream>
#include "Component.h"
#include "Model.h"
#include "SimpleBattery.h"
#include "Simulation.h"
#include "WeakOrder.h"
using namespace SmartGridToolbox;
using namespace std;

class TestCompA : public Component
{
   public:
      TestCompA(const string & name, int x) : Component(name), x_(x)
      {
         addProperty<int>(std::string("prop"), [this]()->int{return x_;});
      }

      virtual void advanceToTime(ptime t)
      {
         std::cout << "TCA AdvanceToTimestep " << t << std::endl;
      }
      friend std::ostream & operator<<(std::ostream & os, const TestCompA & tca)
      {
         return os << tca.getName() << " " << tca.x_ << std::endl;
      }
      int x_;
};

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
}

BOOST_AUTO_TEST_CASE (test_model_dependencies)
{
   Model mod;
   Simulation sim(mod);

   TestCompA * a0 = new TestCompA("tca0", 0);
   TestCompA * a1 = new TestCompA("tca1", 1);
   TestCompA * a2 = new TestCompA("tca2", 2);
   TestCompA * a3 = new TestCompA("tca3", 3);
   TestCompA * a4 = new TestCompA("tca4", 4);
   TestCompA * a5 = new TestCompA("tca5", 5);

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
}

BOOST_AUTO_TEST_CASE (test_properties)
{
   TestCompA * tca = new TestCompA("tca0", 3);
   const Property<int> * prop = tca->getPropertyNamed<int>("prop");
   cout << prop->getValue() << endl;
   BOOST_CHECK(*prop == 3);
   delete tca;
}

BOOST_AUTO_TEST_CASE (test_simple_battery)
{
   SimpleBattery bat1("bat1");
   bat1.setInitCharge(1.0 * kWh);
   bat1.setMaxCharge(4.0 * kWh);
   bat1.setMaxChargePower(1.0 * kW);
   bat1.setMaxDischargePower(2.0 * kW);
   bat1.setChargeEfficiency(0.9);
   bat1.setDischargeEfficiency(0.8);
   bat1.setRequestedPower(0.4 * kW);
}
   
BOOST_AUTO_TEST_SUITE_END( )
