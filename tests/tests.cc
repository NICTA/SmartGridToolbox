#define BOOST_TEST_MODULE test_template
#include <boost/test/included/unit_test.hpp>
#include <cmath>
#include <ostream>
#include <fstream>
#include "Branch.h"
#include "Bus.h"
#include "Component.h"
#include "Event.h"
#include "Model.h"
#include "Network.h"
#include "Parser.h"
#include "PowerFlow.h"
#include "SimpleBattery.h"
#include "SimpleBuilding.h"
#include "Simulation.h"
#include "SparseSolver.h"
#include "TestComponent.h"
#include "Spline.h"
#include "TimeSeries.h"
#include "WeakOrder.h"
using namespace SmartGridToolbox;
using namespace std;
using namespace boost::posix_time;

class TestCompA : public Component
{
   public:
      TestCompA(const std::string & name, int x, double y) : 
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
      message() << " " << g.getNodes()[i]->getIndex() << endl;
   }
   message() << endl;

   message() << "   ";
   for (int i = 0; i < g.size(); ++i)
   {
      std::cout << " " << g.getNodes()[i]->getIndex(); 
   }
   std::cout << endl << endl;
   for (const WoNode * nd1 : g.getNodes())
   {
      message() << nd1->getIndex() << "   ";
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

   TestCompA & a0 = mod.newComponent<TestCompA>("tca0", 0, 0.1);
   TestCompA & a1 = mod.newComponent<TestCompA>("tca1", 1, 0.1);
   TestCompA & a2 = mod.newComponent<TestCompA>("tca2", 2, 0.1);
   TestCompA & a3 = mod.newComponent<TestCompA>("tca3", 3, 0.1);
   TestCompA & a4 = mod.newComponent<TestCompA>("tca4", 4, 0.1);
   TestCompA & a5 = mod.newComponent<TestCompA>("tca5", 5, 0.1);

   a4.dependsOn(a0);
   a5.dependsOn(a0);
   a0.dependsOn(a1);
   a2.dependsOn(a1);
   a1.dependsOn(a3);
   a1.dependsOn(a4);
   a2.dependsOn(a5);

   mod.validate();

   BOOST_CHECK(mod.getComponents()[0] == &a3);
   BOOST_CHECK(mod.getComponents()[1] == &a4);
   BOOST_CHECK(mod.getComponents()[2] == &a1);
   BOOST_CHECK(mod.getComponents()[3] == &a0);
   BOOST_CHECK(mod.getComponents()[4] == &a5);
   BOOST_CHECK(mod.getComponents()[5] == &a2);
}

BOOST_AUTO_TEST_CASE (test_properties)
{
   TestCompA * tca = new TestCompA("tca0", 3, 0.2);
   const Property<int, PropType::GET> * prop1 = tca->getProperty<int, PropType::GET>("x");
   message() << "Property value: " << prop1->get() << endl;
   BOOST_CHECK(prop1->get() == 3);
   tca->addProperty<double, PropType::BOTH>(
         "y",
         [&](){return tca->getY();}, 
         [&](const double & d){tca->setY(d);});
   Property<double, PropType::BOTH> * prop2 = tca->getProperty<double, PropType::BOTH>("y");
   BOOST_CHECK(prop2 != nullptr);
   BOOST_CHECK(prop2->get() == 0.2);
   message() << "Property 2 " << prop2->get() << endl;
   BOOST_CHECK(prop1->get() == 3);
   //(*prop2) = 0.4;
   prop2->set(0.4);
   message() << "Property 2 " << prop2->get() << endl;
   BOOST_CHECK(prop2->get() == 0.4);
   delete tca;
}

BOOST_AUTO_TEST_CASE (test_simple_battery)
{
   using namespace boost::gregorian;
   SimpleBattery bat1("sb0");
   bat1.setName("bat1");
   bat1.setInitCharge(5.0 * kWh);
   bat1.setMaxCharge(8.0 * kWh);
   bat1.setMaxChargePower(1.0 * kW);
   bat1.setMaxDischargePower(2.1 * kW);
   bat1.setChargeEfficiency(0.9);
   bat1.setDischargeEfficiency(0.8);
   bat1.setRequestedPower(-0.4 * kW);
   bat1.initialize(ptime(date(2012, Feb, 11), hours(2)));
   message() << "1 Battery charge = " << bat1.getCharge() / kWh << endl;
   bat1.update(bat1.getTime() + hours(3));
   message() << "2 Battery charge = " << bat1.getCharge() / kWh << endl;
   double comp = bat1.getInitCharge() + 
      dSeconds(hours(3)) * bat1.getRequestedPower() /
      bat1.getDischargeEfficiency();
   message() << "comp = " << comp / kWh << endl;
   BOOST_CHECK(bat1.getCharge() == comp);

   bat1.setRequestedPower(1.3 * kW);
   bat1.initialize(ptime(date(2012, Feb, 11), hours(2)));
   message() << "3 Battery charge = " << bat1.getCharge() / kWh << endl;
   bat1.update(bat1.getTime() + hours(3));
   message() << "4 Battery charge = " << bat1.getCharge() / kWh << endl;
   comp = bat1.getInitCharge() + 
      dSeconds(hours(3)) * bat1.getMaxChargePower() *
      bat1.getChargeEfficiency();
   message() << "comp = " << comp / kWh << endl;
   BOOST_CHECK(bat1.getCharge() == comp);

   bat1.setRequestedPower(-1 * kW);
   bat1.initialize(ptime(date(2012, Feb, 11), hours(2)));
   message() << "3 Battery charge = " << bat1.getCharge() / kWh << endl;
   bat1.update(bat1.getTime() + hours(5.5));
   message() << "4 Battery charge = " << bat1.getCharge() / kWh << endl;
   comp = 0.0;
   message() << "comp = " << comp / kWh << endl;
   BOOST_CHECK(bat1.getCharge() == comp);
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
    for (Spline::const_iterator iPtr = spline.begin(); iPtr != spline.end(); ++iPtr)
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
   using namespace boost::gregorian;
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
      message() << i << " " << val << " " << err << endl; 
      if (i > -1 && i < 25)
      {
         BOOST_CHECK(err < 0.005);
      }
   }
}

BOOST_AUTO_TEST_CASE (test_lerp_timeseries)
{
   using namespace boost::gregorian;
   ptime base(date(2013, Apr, 26), hours(0));
   LerpTimeSeries<ptime, Complex> lts;
   lts.addPoint(base + hours(0), Complex(0, 0));
   lts.addPoint(base + hours(1), Complex(3, 1));
   lts.addPoint(base + hours(3), Complex(10, 11));
   for (int i = -1; i <= 4; ++i)
   {
      Complex val = lts.value(base + hours(i));
      message() << i << " " << val << endl; 
   }
   BOOST_CHECK(lts.value(base + hours(-1)) == Complex(0, 0));
   BOOST_CHECK(lts.value(base + hours(0)) == Complex(0, 0));
   BOOST_CHECK(lts.value(base + minutes(30)) == Complex(1.5, 0.5));
   BOOST_CHECK(lts.value(base + hours(1)) == Complex(3, 1));
   BOOST_CHECK(lts.value(base + hours(2)) == Complex(6.5, 6));
   BOOST_CHECK(lts.value(base + hours(3)) == Complex(10, 11));
   BOOST_CHECK(lts.value(base + hours(4) + seconds(1)) == Complex(10, 11));
}

BOOST_AUTO_TEST_CASE (test_stepwise_timeseries)
{
   time_duration base(minutes(5));
   StepwiseTimeSeries<time_duration, double> sts;
   sts.addPoint(base + hours(0), 1.5);
   sts.addPoint(base + hours(1), 2.5);
   sts.addPoint(base + hours(3), 5.5);
   for (int i = -1; i <= 4; ++i)
   {
      double val = sts.value(base + hours(i));
      message() << i << " " << val << endl; 
   }
   message() << endl;
   for (int i = -1; i <= 4; ++i)
   {
      double val = sts.value(base + hours(i) + seconds(1));
      message() << i << " " << val << endl; 
   }
   BOOST_CHECK(sts.value(base + seconds(-1)) == 1.5);
   BOOST_CHECK(sts.value(base + seconds(1)) == 1.5);
   BOOST_CHECK(sts.value(base + hours(1) - seconds(1)) == 1.5);
   BOOST_CHECK(sts.value(base + hours(1) + seconds(1)) == 2.5);
   BOOST_CHECK(sts.value(base + hours(3) + seconds(1)) == 5.5);
}

BOOST_AUTO_TEST_CASE (test_function_timeseries)
{
   FunctionTimeSeries <time_duration, double> 
      fts([] (time_duration td) {return 2 * dSeconds(td);});
   message() << fts.value(seconds(10)+milliseconds(3)) << endl;
   BOOST_CHECK(fts.value(seconds(-1)) == -2.0);
   BOOST_CHECK(fts.value(seconds(3)) == 6.0);
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
      // Reset state of the object, time is at timestamp t_.
      virtual void initializeState(ptime t) override
      {
         nextUpdate_ = t + dt_; 
      }

      // Bring state up to time t_.
      virtual void updateState(ptime t0, ptime t1) override
      {
         message() << "Update state of " << getName() << " from time " 
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
   Model mod;
   TestEventA & a0 = mod.newComponent<TestEventA>("a0", seconds(3), 3);
   TestEventA & a1 = mod.newComponent<TestEventA>("a1", seconds(9), 3);
   a0.dependsOn(a1); 
   mod.validate();
   Simulation sim(mod);
   sim.setStartTime(epoch);
   sim.setStartTime(epoch + seconds(10));
   sim.initialize();

   a0.getEventDidUpdate().addAction(
         [&]() {message() << a1.getName() << " received did update from " << a0.getName() << std::endl;},
         "Test action.");

   sim.doNextUpdate();
   sim.doNextUpdate();
   sim.doNextUpdate();
   sim.doNextUpdate();
   sim.doNextUpdate();
   sim.doNextUpdate();
   sim.doNextUpdate();
   sim.doNextUpdate();
}

static double sinusoidal(double t, double T, double Delta, 
                         double minim, double maxim)
{
   // Sinusoidal function, T is period, Delta is offset.
   return minim + (maxim - minim) * 0.5 * (1.0 + cos(2.0 * pi * (t - Delta) / T));
}

BOOST_AUTO_TEST_CASE (test_simple_building)
{
   ofstream outfile;
   outfile.open("simple_building.out");

   Model mod;
   Simulation sim(mod);

   ptime t0 = epoch;

   auto Te = [&](ptime t){return sinusoidal(dSeconds(t-t0), day, 12*hour, 
         10*K, 28*K);};
   auto dQg = [&](ptime t){return sinusoidal(dSeconds(t-t0), day, 14*hour, 
         40*kW, 60*kW);};

   SimpleBuilding & build1 = mod.newComponent<SimpleBuilding>("build1");
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

   sim.setStartTime(t0);
   sim.setEndTime(t0 + hours(24));
   sim.initialize();
   auto print = [&] () -> void 
   {
      outfile << dSeconds(build1.getTime())/hour << " " << build1.getTb()
           << " " << build1.getPh() << " " << build1.getdQh() << endl;
   };
   print();
   while (sim.doNextUpdate())
   {
      print();
   }
}

BOOST_AUTO_TEST_CASE (test_parser)
{
   Model mod;
   Simulation sim(mod);
   Parser & p = Parser::getGlobalParser();
   p.parse("test_parser.yaml", mod, sim);
   mod.validate();
   sim.initialize();
   const TestComponent * tc = mod.getComponentNamed<TestComponent>("test_component_1");
   message() << "test_component_1 another is " << tc->getAnother()->getName() << std::endl;
   BOOST_CHECK(tc->getAnother()->getName() == "test_component_2");
}

BOOST_AUTO_TEST_CASE (test_sparse_solver)
{
   int n = 5;
   UblasCMatrix<double> a(n, n);
   a(0, 0) = 2.0;
   a(0, 1) = 3.0;
   a(1, 0) = 3.0;
   a(1, 2) = 4.0;
   a(1, 4) = 6.0;
   a(2, 1) = -1.0;
   a(2, 2) = -3.0;
   a(2, 3) = 2.0;
   a(3, 2) = 1.0;
   a(4, 1) = 4.0;
   a(4, 2) = 2.0;
   a(4, 4) = 1.0;

   UblasVector<double> b(n);
   b(0) = 8.0;
   b(1) = 45.0;
   b(2) = -3.0;
   b(3) = 3.0;
   b(4) = 19.0;

   UblasVector<double> x(n);
   KLUSolve(a, b, x);
   message(); for (int i = 0; i < n; ++i) std::cout << x(i) << " ";
   std::cout << endl;
   BOOST_CHECK(std::abs(x(0) - 1.0) < 1e-4);
   BOOST_CHECK(std::abs(x(1) - 2.0) < 1e-4);
   BOOST_CHECK(std::abs(x(2) - 3.0) < 1e-4);
   BOOST_CHECK(std::abs(x(3) - 4.0) < 1e-4);
   BOOST_CHECK(std::abs(x(4) - 5.0) < 1e-4);
}

inline Array2D<Complex, 2, 2> lineY(Complex y)
{
   return {{{y, -y},{-y, y}}};
}


class TestLoad : public ZipToGround
{
   public:
      TestLoad(const std::string & name) : ZipToGround(name), dt_(seconds(0))
      {
         // Empty.
      }

      virtual ptime getValidUntil() const override
      {
         return getTime() + dt_;
      }

      virtual void initializeState(ptime t) override
      {
         Y_ = UblasVector<Complex>(getPhases().size(), czero);
         I_ = UblasVector<Complex>(getPhases().size(), czero);
         S_ = UblasVector<Complex>(getPhases().size(), czero);
      }

      virtual void updateState(ptime t0, ptime t1) override
      {
         for (int i = 0; i < getPhases().size(); ++i)
         {
            Y_(i) = sin(dSeconds(t1)/123.0);
            I_(i) = sin(dSeconds(t1)/300.0) * exp(Complex(0.0, dSeconds(t1)/713.0));
            S_(i) = sin(dSeconds(t1)/60.0);
         }
      }

      time_duration getDt() const
      {
         return dt_;
      }
      void setDt(time_duration dt)
      {
         dt_ = dt;
      }

   private:
      time_duration dt_;
};

BOOST_AUTO_TEST_CASE (test_phases)
{
   Phases p1 = Phase::A | Phase::B;
   BOOST_CHECK(p1.size() == 2);
   BOOST_CHECK(p1[0] == Phase::A);
   BOOST_CHECK(p1[1] == Phase::B);
   Phases p2{str2Phase("SP") | str2Phase("SM") | Phase::SN};
   for (auto pair : p2)
   {
      BOOST_CHECK((pair.second == 0 && pair.first == Phase::SP) ||
                  (pair.second == 1 && pair.first == Phase::SM) ||
                  (pair.second == 2 && pair.first == Phase::SN));
   }
   Phases p3 = p1 | p2;
   BOOST_CHECK(p3.hasPhase(Phase::A));
   BOOST_CHECK(p3.hasPhase(Phase::B));
   BOOST_CHECK(!p3.hasPhase(Phase::C));
   BOOST_CHECK(p3.hasPhase(Phase::SP));
   BOOST_CHECK(p3.hasPhase(Phase::SM));
   BOOST_CHECK(p3.hasPhase(Phase::SN));
   p3 &= Phase::A;
   BOOST_CHECK(p3 == Phase::A);
   Phases p4 = p1 | Phase::C;
   BOOST_CHECK(p1.isSubsetOf(p4));
}

BOOST_AUTO_TEST_CASE (test_network_1p)
{
   Model mod;
   Simulation sim(mod);

   Parser & p = Parser::getGlobalParser();
   p.parse("test_network_1p.yaml", mod, sim);

   Bus * bus1 = mod.getComponentNamed<Bus>("bus_1");
   Bus * bus2 = mod.getComponentNamed<Bus>("bus_2");
   Bus * bus3 = mod.getComponentNamed<Bus>("bus_3");

   TestLoad & tl0 = mod.newComponent<TestLoad>("tl0");
   tl0.setPhases(bus2->getPhases());
   tl0.setDt(seconds(5));
   bus2->addZipToGround(tl0);

   mod.validate();
   sim.initialize();

   Network * network = mod.getComponentNamed<Network>("network_1");
   ofstream outfile;
   outfile.open("test_network_1p.out");
   network->getEventDidUpdate().addAction([&]()
         {
            outfile << dSeconds(sim.getCurrentTime()-sim.getStartTime()) << " " << bus1->getV()(0) 
                    << " " << bus2->getV()(0) << " " << bus3->getV()(0) << std::endl;
         }, "Network updated.");

   while (sim.doNextUpdate())
   {
   }
   outfile.close();
}

BOOST_AUTO_TEST_CASE (test_network_b3p)
{
   Model mod;
   Simulation sim(mod);

   Parser & p = Parser::getGlobalParser();
   p.parse("test_network_b3p.yaml", mod, sim);

   Bus * bus1 = mod.getComponentNamed<Bus>("bus_1");
   Bus * bus2 = mod.getComponentNamed<Bus>("bus_2");
   Bus * bus3 = mod.getComponentNamed<Bus>("bus_3");

   TestLoad & tl0 = mod.newComponent<TestLoad>("tl0");
   tl0.setPhases(bus2->getPhases());
   tl0.setDt(seconds(5));
   bus2->addZipToGround(tl0);

   mod.validate();
   sim.initialize();

   Network * network = mod.getComponentNamed<Network>("network_1");
   ofstream outfile;
   outfile.open("test_network_b3p.out");
   outfile << dSeconds(sim.getCurrentTime()-sim.getStartTime()) << " " << bus1->getV() << " " << bus2->getV() 
      << " " << bus3->getV() << std::endl;
   network->getEventDidUpdate().addAction([&]()
         {
            outfile << dSeconds(sim.getCurrentTime()-sim.getStartTime()) << " " << bus1->getV() << " " << bus2->getV() 
                    << " " << bus3->getV() << std::endl;
         }, "Network updated.");

   while (sim.doNextUpdate())
   {
   }
   outfile.close();
}

BOOST_AUTO_TEST_CASE (test_network_2p_identical)
{
   Model mod;
   Simulation sim(mod);

   Parser & p = Parser::getGlobalParser();
   p.parse("test_network_2p_identical.yaml", mod, sim);

   Bus * bus1 = mod.getComponentNamed<Bus>("bus_1");
   Bus * bus2 = mod.getComponentNamed<Bus>("bus_2");
   Bus * bus3 = mod.getComponentNamed<Bus>("bus_3");

   TestLoad & tl0 = mod.newComponent<TestLoad>("tl0");
   tl0.setPhases(bus2->getPhases());
   tl0.setDt(seconds(5));
   bus2->addZipToGround(tl0);

   mod.validate();
   sim.initialize();

   Network * network = mod.getComponentNamed<Network>("network_1");
   ofstream outfile;
   outfile.open("test_network_2p_identical.out");
   outfile << dSeconds(sim.getCurrentTime()-sim.getStartTime()) << " " 
           << bus1->getV()(0) << " " << bus2->getV()(0) << " " << bus3->getV()(0) << std::endl;
   network->getEventDidUpdate().addAction([&]()
         {
            outfile << dSeconds(sim.getCurrentTime()-sim.getStartTime()) << " " 
                    << bus1->getV()(0) << " " << bus2->getV()(0) << " " << bus3->getV()(0) << std::endl;
         }, "Network updated.");

   while (sim.doNextUpdate())
   {
   }
   outfile.close();
}

BOOST_AUTO_TEST_SUITE_END( )
