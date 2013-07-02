#define BOOST_TEST_MODULE test_template
#include <boost/test/included/unit_test.hpp>
#include <cmath>
#include <ostream>
#include <fstream>
#include "Component.h"
#include "Event.h"
#include "Model.h"
#include "Output.h"
#include "Parser.h"
#include "powerflow.h"
#include "SimpleBattery.h"
#include "SimpleBuilding.h"
#include "Simulation.h"
#include "solver_nr.h"
#include "TestComponent.h"
#include "Spline.h"
#include "TimeSeries.h"
#include "WeakOrder.h"
using namespace SmartGridToolbox;
using namespace std;

class TestCompA : public Component
{
   public:
      TestCompA(const string & name, int x, double y) : 
         Component(name),
         x_(x),
         y_(y)
      {
         addProperty<int, PropType::GET>(std::string("x"), 
               [this](){return x_;});
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

   a4->dependsOn(*a0);
   a5->dependsOn(*a0);
   a0->dependsOn(*a1);
   a2->dependsOn(*a1);
   a1->dependsOn(*a3);
   a1->dependsOn(*a4);
   a2->dependsOn(*a5);

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
   const Property<int, PropType::GET> * prop1 = tca->getProperty<int, PropType::GET>("x");
   cout << "Property value: " << prop1->get() << endl;
   BOOST_CHECK(prop1->get() == 3);
   tca->addProperty<double, PropType::BOTH>(
         "y",
         [&](){return tca->getY();}, 
         [&](const double & d){tca->setY(d);});
   Property<double, PropType::BOTH> * prop2 = tca->getProperty<double, PropType::BOTH>("y");
   BOOST_CHECK(prop2 != nullptr);
   BOOST_CHECK(prop2->get() == 0.2);
   cout << "Property 2 " << prop2->get() << endl;
   BOOST_CHECK(prop1->get() == 3);
   //(*prop2) = 0.4;
   prop2->set(0.4);
   cout << "Property 2 " << prop2->get() << endl;
   BOOST_CHECK(prop2->get() == 0.4);
   delete tca;
   message("Testing properties. Completed.");
}

BOOST_AUTO_TEST_CASE (test_simple_battery)
{
   message("Testing SimpleBattery. Starting.");
   SimpleBattery bat1;
   bat1.setName("bat1");
   bat1.setInitCharge(5.0 * kWh);
   bat1.setMaxCharge(8.0 * kWh);
   bat1.setMaxChargePower(1.0 * kW);
   bat1.setMaxDischargePower(2.1 * kW);
   bat1.setChargeEfficiency(0.9);
   bat1.setDischargeEfficiency(0.8);
   bat1.setRequestedPower(-0.4 * kW);
   bat1.initialize(ptime(date(2012, Feb, 11), hours(2)));
   cout << "1 Battery charge = " << bat1.getCharge() / kWh << endl;
   bat1.update(bat1.getTime() + hours(3));
   cout << "2 Battery charge = " << bat1.getCharge() / kWh << endl;
   double comp = bat1.getInitCharge() + 
      dSeconds(hours(3)) * bat1.getRequestedPower() /
      bat1.getDischargeEfficiency();
   cout << "comp = " << comp / kWh << endl;
   BOOST_CHECK(bat1.getCharge() == comp);

   bat1.setRequestedPower(1.3 * kW);
   bat1.initialize(ptime(date(2012, Feb, 11), hours(2)));
   cout << "3 Battery charge = " << bat1.getCharge() / kWh << endl;
   bat1.update(bat1.getTime() + hours(3));
   cout << "4 Battery charge = " << bat1.getCharge() / kWh << endl;
   comp = bat1.getInitCharge() + 
      dSeconds(hours(3)) * bat1.getMaxChargePower() *
      bat1.getChargeEfficiency();
   cout << "comp = " << comp / kWh << endl;
   BOOST_CHECK(bat1.getCharge() == comp);

   bat1.setRequestedPower(-1 * kW);
   bat1.initialize(ptime(date(2012, Feb, 11), hours(2)));
   cout << "3 Battery charge = " << bat1.getCharge() / kWh << endl;
   bat1.update(bat1.getTime() + hours(5.5));
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

BOOST_AUTO_TEST_CASE (test_spline_timeseries)
{
   message("Testing SplineTimeSeries. Starting.");
   ptime base(date(2013, Apr, 26), hours(0));
   SplineTimeSeries<ptime> sts;
   sts.addPoint(base + hours(0), sin(0*pi/12));
   sts.addPoint(base + hours(4), sin(4*pi/12));
   sts.addPoint(base + hours(8), sin(8*pi/12));
   sts.addPoint(base + hours(12), sin(12*pi/12));
   sts.addPoint(base + hours(16), sin(16*pi/12));
   sts.addPoint(base + hours(20), sin(20*pi/12));
   sts.addPoint(base + hours(24), sin(24*pi/12));
   for (int i = -1; i <= 25; ++i)
   {
      double val = sts.value(base + hours(i));
      double err = std::abs(val - sin(i*pi/12));
      cout << i << " " << val << " " << err << endl; 
      if (i > -1 && i < 25)
      {
         BOOST_CHECK(err < 0.005);
      }
   }
   message("Testing SplineTimeSeries. Completed.");
}

BOOST_AUTO_TEST_CASE (test_lerp_timeseries)
{
   message("Testing LerpTimeSeries. Starting.");
   ptime base(date(2013, Apr, 26), hours(0));
   LerpTimeSeries<ptime, Complex> lts;
   lts.addPoint(base + hours(0), Complex(0, 0));
   lts.addPoint(base + hours(1), Complex(3, 1));
   lts.addPoint(base + hours(3), Complex(10, 11));
   for (int i = -1; i <= 4; ++i)
   {
      Complex val = lts.value(base + hours(i));
      cout << i << " " << val << endl; 
   }
   BOOST_CHECK(lts.value(base + hours(-1)) == Complex(0, 0));
   BOOST_CHECK(lts.value(base + hours(0)) == Complex(0, 0));
   BOOST_CHECK(lts.value(base + minutes(30)) == Complex(1.5, 0.5));
   BOOST_CHECK(lts.value(base + hours(1)) == Complex(3, 1));
   BOOST_CHECK(lts.value(base + hours(2)) == Complex(6.5, 6));
   BOOST_CHECK(lts.value(base + hours(3)) == Complex(10, 11));
   BOOST_CHECK(lts.value(base + hours(4) + seconds(1)) == Complex(10, 11));
   message("Testing LerpTimeSeries. Completed.");
}

BOOST_AUTO_TEST_CASE (test_stepwise_timeseries)
{
   message("Testing StepwiseTimeSeries. Starting.");
   time_duration base(minutes(5));
   StepwiseTimeSeries<time_duration, double> sts;
   sts.addPoint(base + hours(0), 1.5);
   sts.addPoint(base + hours(1), 2.5);
   sts.addPoint(base + hours(3), 5.5);
   for (int i = -1; i <= 4; ++i)
   {
      double val = sts.value(base + hours(i));
      cout << i << " " << val << endl; 
   }
   cout << endl;
   for (int i = -1; i <= 4; ++i)
   {
      double val = sts.value(base + hours(i) + seconds(1));
      cout << i << " " << val << endl; 
   }
   BOOST_CHECK(sts.value(base + seconds(-1)) == 1.5);
   BOOST_CHECK(sts.value(base + seconds(1)) == 1.5);
   BOOST_CHECK(sts.value(base + hours(1) - seconds(1)) == 1.5);
   BOOST_CHECK(sts.value(base + hours(1) + seconds(1)) == 2.5);
   BOOST_CHECK(sts.value(base + hours(3) + seconds(1)) == 5.5);
   message("Testing StepwiseTimeSeries. Completed.");
}

BOOST_AUTO_TEST_CASE (test_function_timeseries)
{
   message("Testing FunctionTimeSeries. Starting.");
   FunctionTimeSeries <time_duration, double> 
      fts([] (time_duration td) {return 2 * dSeconds(td);});
   cout << fts.value(seconds(10)+milliseconds(3)) << endl;
   BOOST_CHECK(fts.value(seconds(-1)) == -2.0);
   BOOST_CHECK(fts.value(seconds(3)) == 6.0);
   message("Testing FunctionTimeSeries. Completed.");
}

enum class Event : int
{
   ZERO,
   ONE,
   TWO
};

class TestEventA : public Component
{
   public:

      TestEventA(const std::string & name, time_duration dt, int ctrl) : 
         Component(name),
         state_(0),
         dt_(dt),
         ctrl_(ctrl)
      {
      }

      virtual ptime getValidUntil() const override
      {
         return nextUpdate_;
      }

   private:
      /// Reset state of the object, time is at timestamp t_.
      virtual void initializeState(ptime t) override
      {
         nextUpdate_ = t + dt_; 
      }

      /// Bring state up to time t_.
      virtual void updateState(ptime t0, ptime t1) override
      {
         cout << "Update state of " << getName() << " from time " 
              << t0 << " to " << t1 << "." << endl;
         state_ = (t1-getInitTime()).ticks() * ctrl_;
         nextUpdate_ = t1 + dt_; 
      }

   private:
      ptime nextUpdate_;
      int state_;
      time_duration dt_;
      int ctrl_;
};

BOOST_AUTO_TEST_CASE (test_events_and_sync)
{
   message("Testing events and synchronization. Starting.");
   Model mod;
   TestEventA * a0 = new TestEventA("a0", seconds(3), 3);
   TestEventA * a1 = new TestEventA("a1", seconds(9), 3);
   a0->dependsOn(*a1); 
   mod.addComponent(*a0);
   mod.addComponent(*a1);
   mod.validate();
   Simulation sim(mod);
   message("Initialize simulation. Starting.");
   sim.initialize(epoch, epoch + seconds(10));
   message("Initialize simulation. Completed.");

   sim.doNextUpdate();
   sim.doNextUpdate();
   sim.doNextUpdate();
   sim.doNextUpdate();
   sim.doNextUpdate();
   sim.doNextUpdate();
   sim.doNextUpdate();
   sim.doNextUpdate();

   message("Testing events and synchronization. Completed.");
}

static double sinusoidal(double t, double T, double Delta, 
                         double minim, double maxim)
{
   // Sinusoidal function, T is period, Delta is offset.
   return minim + (maxim - minim) * 0.5 * (1.0 + 
         cos(2.0 * pi * (t - Delta) / T));
}

BOOST_AUTO_TEST_CASE (test_simple_building)
{
   message("Testing SimpleBuilding. Starting.");

   ofstream outfile;
   outfile.open("simple_building.out");

   Model mod;
   Simulation sim(mod);

   ptime t0 = epoch;

   auto Te = [&](ptime t){return sinusoidal(dSeconds(t-t0), day, 12*hour, 
         10*K, 28*K);};
   auto dQg = [&](ptime t){return sinusoidal(dSeconds(t-t0), day, 14*hour, 
         40*kW, 60*kW);};

   SimpleBuilding build1;
   build1.setName("build1");
   build1.setkb(5*kW/K);
   build1.setCb(1e5*kJ/K);
   build1.setTbInit(22*K);
   build1.setkh(10*kW/K);
   build1.setCopCool(3);
   build1.setCopHeat(4);
   build1.setPmax(30*kW);
   build1.setTs(20*K);
   build1.setTeFunc(Te);
   build1.setdQgFunc(dQg);

   mod.addComponent(build1);

   sim.initialize(t0, t0 + seconds(10));
   auto print = [&] () -> void 
   {
      outfile << dSeconds(build1.getTime())/hour << " " << build1.getTb()
           << " " << build1.getPh() << " " << build1.getdQh() << endl;
   };
   print();
   while (build1.getTime() <= t0 + hours(240))
   {
      sim.doNextUpdate();
      print();
   }

   message("Testing SimpleBuilding. Completed.");
}

BOOST_AUTO_TEST_CASE (test_parser)
{
   message("Testing Parser. Starting.");
   Model mod;
   Simulation sim(mod);
   Parser & p = Parser::getGlobalParser();
   p.registerComponentParser<TestComponentParser>();
   p.parse("test_parser.yaml", mod, sim);
   message("Testing Parser. Completed.");
   const TestComponent * tc = mod.getComponentNamed<TestComponent>(
         "test_component_1");
   message("test_component_1 another is %s", 
         tc->getAnother()->getName().c_str());
   BOOST_CHECK(tc->getAnother()->getName() == "test_component_2");
   message("Testing Parser. Completed.");
}

BOOST_AUTO_TEST_CASE (test_solver_nr)
{
   message("Testing solver_nr. Starting.");
   std::vector<std::unique_ptr<BUSDATA>> busses;
   std::vector<std::unique_ptr<BRANCHDATA>> branches;
   std::unique_ptr<BUSDATA> bus(new BUSDATA);
   bus->type = BusType::SL;
   bus->phases = 0x7;
   setPolar(bus->V[0], 1.0, 0.0);
   setPolar(bus->V[1], 1.0, -2.0 * pi / 3.0);
   setPolar(bus->V[2], 1.0, 2.0 * pi / 3.0);
   setPolar(bus->S[0], 0.0, 0.0);
   setPolar(bus->S[1], 0.0, 0.0);
   setPolar(bus->S[2], 0.0, 0.0);
   setPolar(bus->Y[0], 0.0, 0.0);
   setPolar(bus->Y[1], 0.0, 0.0);
   setPolar(bus->Y[2], 0.0, 0.0);
   busses.push_back(std::move(bus));
   bus.reset(new BUSDATA);
   message("Testing solver_nr. Completed.");
}

BOOST_AUTO_TEST_SUITE_END( )
