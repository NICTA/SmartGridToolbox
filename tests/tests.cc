#define BOOST_TEST_MODULE test_template
#include <boost/test/included/unit_test.hpp>
#include <cmath>
#include <ostream>
#include <fstream>
#include "Branch.h"
#include "Bus.h"
#include "Component.h"
#include "DCPowerSourceBase.h"
#include "Event.h"
#include "InverterBase.h"
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
#include "ZipToGround.h"
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

      int x() {return x_;}
      void setX(int x) {x_ = x;}

      double Y() {return y_;}
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
      message() << " " << g.nodes()[i]->index() << endl;
   }
   message() << endl;

   message() << "   ";
   for (int i = 0; i < g.size(); ++i)
   {
      std::cout << " " << g.nodes()[i]->index(); 
   }
   std::cout << endl << endl;
   for (const WoNode * nd1 : g.nodes())
   {
      message() << nd1->index() << "   ";
      for (const WoNode * nd2 : g.nodes())
      {
         std::cout << nd1->dominates(*nd2) << " ";
      }
      std::cout << endl;
   }

   BOOST_CHECK(g.nodes()[0]->index() == 3);
   BOOST_CHECK(g.nodes()[1]->index() == 4);
   BOOST_CHECK(g.nodes()[2]->index() == 1);
   BOOST_CHECK(g.nodes()[3]->index() == 0);
   BOOST_CHECK(g.nodes()[4]->index() == 5);
   BOOST_CHECK(g.nodes()[5]->index() == 2);
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

   BOOST_CHECK(mod.components()[0] == &a3);
   BOOST_CHECK(mod.components()[1] == &a4);
   BOOST_CHECK(mod.components()[2] == &a1);
   BOOST_CHECK(mod.components()[3] == &a0);
   BOOST_CHECK(mod.components()[4] == &a5);
   BOOST_CHECK(mod.components()[5] == &a2);
}

BOOST_AUTO_TEST_CASE (test_properties)
{
   TestCompA * tca = new TestCompA("tca0", 3, 0.2);
   const Property<int, PropType::GET> * prop1 = tca->property<int, PropType::GET>("x");
   message() << "Property value: " << prop1->get() << endl;
   BOOST_CHECK(prop1->get() == 3);
   tca->addProperty<double, PropType::BOTH>(
         "y",
         [&](){return tca->Y();}, 
         [&](const double & d){tca->setY(d);});
   Property<double, PropType::BOTH> * prop2 = tca->property<double, PropType::BOTH>("y");
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
   message() << "1 Battery charge = " << bat1.charge() / kWh << endl;
   bat1.update(bat1.time() + hours(3));
   message() << "2 Battery charge = " << bat1.charge() / kWh << endl;
   double comp = bat1.initCharge() + 
      dSeconds(hours(3)) * bat1.requestedPower() /
      bat1.dischargeEfficiency();
   message() << "comp = " << comp / kWh << endl;
   BOOST_CHECK(bat1.charge() == comp);

   bat1.setRequestedPower(1.3 * kW);
   bat1.initialize(ptime(date(2012, Feb, 11), hours(2)));
   message() << "3 Battery charge = " << bat1.charge() / kWh << endl;
   bat1.update(bat1.time() + hours(3));
   message() << "4 Battery charge = " << bat1.charge() / kWh << endl;
   comp = bat1.initCharge() + 
      dSeconds(hours(3)) * bat1.maxChargePower() *
      bat1.chargeEfficiency();
   message() << "comp = " << comp / kWh << endl;
   BOOST_CHECK(bat1.charge() == comp);

   bat1.setRequestedPower(-1 * kW);
   bat1.initialize(ptime(date(2012, Feb, 11), hours(2)));
   message() << "3 Battery charge = " << bat1.charge() / kWh << endl;
   bat1.update(bat1.time() + hours(5.5));
   message() << "4 Battery charge = " << bat1.charge() / kWh << endl;
   comp = 0.0;
   message() << "comp = " << comp / kWh << endl;
   BOOST_CHECK(bat1.charge() == comp);
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

      virtual ptime validUntil() const override
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
         message() << "Update state of " << name() << " from time " 
              << t0 << " to " << t1 << "." << endl;
         state_ = (t1-initTime()).ticks() * ctrl_;
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

   a0.eventDidUpdate().addAction(
         [&]() {message() << a1.name() << " received did update from " << a0.name() << std::endl;},
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
   build1.setCOPCool(3);
   build1.setCOPHeat(4);
   build1.setPmax(30*kW);
   build1.setTs(20*K);
   build1.setTeFunc(Te);
   build1.setdQgFunc(dQg);

   sim.setStartTime(t0);
   sim.setEndTime(t0 + hours(24));
   sim.initialize();
   auto print = [&] () -> void 
   {
      outfile << dSeconds(build1.time())/hour << " " << build1.Tb()
           << " " << build1.Ph() << " " << build1.getdQh() << endl;
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
   Parser & p = Parser::globalParser();
   p.parse("test_parser.yaml", mod, sim);
   mod.validate();
   sim.initialize();
   const TestComponent * tc = mod.componentNamed<TestComponent>("test_component_1");
   message() << "test_component_1 another is " << tc->another()->name() << std::endl;
   BOOST_CHECK(tc->another()->name() == "test_component_2");
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


class TestLoad : public ZipToGroundBase
{
   public:
      TestLoad(const std::string & name) : ZipToGroundBase(name), dt_(seconds(0)) {}

      virtual ptime validUntil() const override
      {
         return time() + dt_;
      }

      virtual UblasVector<Complex> Y() const override 
      {
         return UblasVector<Complex>(phases().size(), sin(dSeconds(time())/123.0));
      }

      virtual UblasVector<Complex> I() const override 
      {
         return UblasVector<Complex>(phases().size(), 
               sin(dSeconds(time())/300.0) * exp(Complex(0.0, dSeconds(time())/713.0)));
      }

      virtual UblasVector<Complex> S() const override 
      {
         return UblasVector<Complex>(phases().size(), sin(dSeconds(time())/60.0));
      }

      time_duration dt() const
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

   Parser & p = Parser::globalParser();
   p.parse("test_network_1p.yaml", mod, sim);

   Bus * bus1 = mod.componentNamed<Bus>("bus_1");
   Bus * bus2 = mod.componentNamed<Bus>("bus_2");
   Bus * bus3 = mod.componentNamed<Bus>("bus_3");

   TestLoad & tl0 = mod.newComponent<TestLoad>("tl0");
   tl0.phases() = bus2->phases();
   tl0.setDt(seconds(5));
   bus2->addZipToGround(tl0);

   mod.validate();
   sim.initialize();

   Network * network = mod.componentNamed<Network>("network_1");
   ofstream outfile;
   outfile.open("test_network_1p.out");
   network->eventDidUpdate().addAction([&]()
         {
            outfile << dSeconds(sim.currentTime()-sim.startTime()) << " " << bus1->V()(0) 
                    << " " << bus2->V()(0) << " " << bus3->V()(0) << std::endl;
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

   Parser & p = Parser::globalParser();
   p.parse("test_network_b3p.yaml", mod, sim);

   Bus * bus1 = mod.componentNamed<Bus>("bus_1");
   Bus * bus2 = mod.componentNamed<Bus>("bus_2");
   Bus * bus3 = mod.componentNamed<Bus>("bus_3");

   TestLoad & tl0 = mod.newComponent<TestLoad>("tl0");
   tl0.phases() = bus2->phases();
   tl0.setDt(seconds(5));
   bus2->addZipToGround(tl0);

   mod.validate();
   sim.initialize();

   Network * network = mod.componentNamed<Network>("network_1");
   ofstream outfile;
   outfile.open("test_network_b3p.out");
   outfile << dSeconds(sim.currentTime()-sim.startTime()) << " " 
           << bus1->V()(0) << " " << bus1->V()(1) << " " << bus1->V()(2)
           << bus2->V()(0) << " " << bus2->V()(1) << " " << bus2->V()(2)
           << bus3->V()(0) << " " << bus3->V()(1) << " " << bus3->V()(2)
           << std::endl;
   network->eventDidUpdate().addAction([&]()
         {
            outfile << dSeconds(sim.currentTime()-sim.startTime()) << " " 
                    << bus1->V()(0) << " " << bus1->V()(1) << " " << bus1->V()(2) << " "
                    << bus2->V()(0) << " " << bus2->V()(1) << " " << bus2->V()(2) << " "
                    << bus3->V()(0) << " " << bus3->V()(1) << " " << bus3->V()(2)
                    << std::endl;
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

   Parser & p = Parser::globalParser();
   p.parse("test_network_2p_identical.yaml", mod, sim);

   Bus * bus1 = mod.componentNamed<Bus>("bus_1");
   Bus * bus2 = mod.componentNamed<Bus>("bus_2");
   Bus * bus3 = mod.componentNamed<Bus>("bus_3");

   TestLoad & tl0 = mod.newComponent<TestLoad>("tl0");
   tl0.phases() = bus2->phases();
   tl0.setDt(seconds(5));
   bus2->addZipToGround(tl0);

   mod.validate();
   sim.initialize();

   Network * network = mod.componentNamed<Network>("network_1");
   ofstream outfile;
   outfile.open("test_network_2p_identical.out");
   outfile << dSeconds(sim.currentTime()-sim.startTime()) << " " 
           << bus1->V()(0) << " " << bus2->V()(0) << " " << bus3->V()(0) << std::endl;
   network->eventDidUpdate().addAction([&]()
         {
            outfile << dSeconds(sim.currentTime()-sim.startTime()) << " " 
                    << bus1->V()(0) << " " << bus2->V()(0) << " " << bus3->V()(0) << std::endl;
         }, "Network updated.");

   while (sim.doNextUpdate())
   {
   }
   outfile.close();
}

class TestDC : public DCPowerSourceBase
{
   public:
      TestDC(const std::string & name) : DCPowerSourceBase(name), dt_(seconds(0)) {}

      virtual ptime validUntil() const override
      {
         return time() + dt_;
      }

      time_duration dt() const
      {
         return dt_;
      }
      void setDt(time_duration dt)
      {
         dt_ = dt;
      }

      virtual double PDC() const override
      {
         return sin(dSeconds(time())/60.0);
      }

   private:
      time_duration dt_;
};

BOOST_AUTO_TEST_CASE (test_networked_dc)
{
   Model mod;
   Simulation sim(mod);

   Parser & p = Parser::globalParser();
   p.parse("test_networked_dc.yaml", mod, sim);

   Bus * bus1 = mod.componentNamed<Bus>("bus_1");
   Bus * bus2 = mod.componentNamed<Bus>("bus_2");
   Bus * bus3 = mod.componentNamed<Bus>("bus_3");
   
   InverterBase * inverter2 = mod.componentNamed<InverterBase>("inverter_bus_2");

   TestDC & tDC = mod.newComponent<TestDC>("tdc");
   tDC.setDt(seconds(5));
   inverter2->addDCPowerSource(tDC);

   mod.validate();
   sim.initialize();

   Network * network = mod.componentNamed<Network>("network_1");
   ofstream outfile;
   outfile.open("test_networked_dc.out");
   outfile << dSeconds(sim.currentTime()-sim.startTime()) << " " 
           << bus1->V()(0) << " " << bus1->V()(1) << " " << bus1->V()(2)
           << bus2->V()(0) << " " << bus2->V()(1) << " " << bus2->V()(2)
           << bus3->V()(0) << " " << bus3->V()(1) << " " << bus3->V()(2)
           << std::endl;
   network->eventDidUpdate().addAction([&]()
         {
            outfile << dSeconds(sim.currentTime()-sim.startTime()) << " " 
                    << bus1->V()(0) << " " << bus1->V()(1) << " " << bus1->V()(2) << " "
                    << bus2->V()(0) << " " << bus2->V()(1) << " " << bus2->V()(2) << " "
                    << bus3->V()(0) << " " << bus3->V()(1) << " " << bus3->V()(2)
                    << std::endl;
         }, "Network updated.");

   while (sim.doNextUpdate())
   {
   }
   outfile.close();
}

BOOST_AUTO_TEST_SUITE_END( )
