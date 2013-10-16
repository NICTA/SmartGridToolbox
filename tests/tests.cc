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
#include "RegularUpdateComponent.h"
#include "SimpleBattery.h"
#include "SimpleBuilding.h"
#include "Simulation.h"
#include "SparseSolver.h"
#include "SolarPV.h"
#include "Sun.h"
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
   // Order should be 3, (0, 1, 4), 5, 2.
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
   BOOST_CHECK(g.nodes()[1]->index() == 0);
   BOOST_CHECK(g.nodes()[2]->index() == 1);
   BOOST_CHECK(g.nodes()[3]->index() == 4);
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
   BOOST_CHECK(mod.components()[1] == &a0);
   BOOST_CHECK(mod.components()[2] == &a1);
   BOOST_CHECK(mod.components()[3] == &a4);
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
   time_zone_ptr tz(new posix_time_zone("UTC0"));
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
   bat1.initialize(timeFromLocalTime(ptime(date(2012, Feb, 11), hours(2)), tz));
   message() << "1 Battery charge = " << bat1.charge() / kWh << endl;
   bat1.update(bat1.time() + hours(3));
   message() << "2 Battery charge = " << bat1.charge() / kWh << endl;
   double comp = bat1.initCharge() + 
      dSeconds(hours(3)) * bat1.requestedPower() /
      bat1.dischargeEfficiency();
   message() << "comp = " << comp / kWh << endl;
   BOOST_CHECK(bat1.charge() == comp);

   bat1.setRequestedPower(1.3 * kW);
   bat1.initialize(timeFromLocalTime(ptime(date(2012, Feb, 11), hours(2)), tz));
   message() << "3 Battery charge = " << bat1.charge() / kWh << endl;
   bat1.update(bat1.time() + hours(3));
   message() << "4 Battery charge = " << bat1.charge() / kWh << endl;
   comp = bat1.initCharge() + 
      dSeconds(hours(3)) * bat1.maxChargePower() *
      bat1.chargeEfficiency();
   message() << "comp = " << comp / kWh << endl;
   BOOST_CHECK(bat1.charge() == comp);

   bat1.setRequestedPower(-1 * kW);
   bat1.initialize(timeFromLocalTime(ptime(date(2012, Feb, 11), hours(2)), tz));
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
   time_zone_ptr tz(new posix_time_zone("UTC0"));
   using namespace boost::gregorian;
   Time base = timeFromLocalTime(ptime(date(2013, Apr, 26), hours(0)), tz);
   SplineTimeSeries<Time> sts;
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
   time_zone_ptr tz(new posix_time_zone("UTC0"));
   using namespace boost::gregorian;
   Time base = timeFromLocalTime(ptime(date(2013, Apr, 26), hours(0)), tz);
   LerpTimeSeries<Time, Complex> lts;
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

      virtual time_duration validUntil() const override
      {
         return nextUpdate_;
      }

   private:
      // Reset state of the object, time is at timestamp t_.
      virtual void initializeState() override
      {
         nextUpdate_ = startTime() + dt_; 
      }

      // Bring state up to time t_.
      virtual void updateState(Time t0, Time t1) override
      {
         message() << "Update state of " << name() << " from time " 
              << t0 << " to " << t1 << "." << endl;
         state_ = (t1-startTime()).ticks() * ctrl_;
         nextUpdate_ = t1 + dt_;
      }

   private:
      Time nextUpdate_;
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
   sim.setStartTime(seconds(0));
   sim.setEndTime(seconds(10));
   sim.initialize();

   a0.didCompleteTimestep().addAction(
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
   time_zone_ptr tz(new posix_time_zone("UTC0"));
   ofstream outfile;
   outfile.open("simple_building.out");

   Model mod;
   Simulation sim(mod);

   Time t0 = seconds(0);

   auto Te = [&](Time t){return sinusoidal(dSeconds(t-t0), day, 12*hour, 
         10*K, 28*K);};
   auto dQg = [&](Time t){return sinusoidal(dSeconds(t-t0), day, 14*hour, 
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
      TestLoad(const std::string & name, Phases phases, Time dt) : ZipToGroundBase(name, phases), dt_(dt) {}

      virtual Time validUntil() const override
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

   TestLoad & tl0 = mod.newComponent<TestLoad>("tl0", bus2->phases(), seconds(5));
   bus2->addZipToGround(tl0);

   mod.validate();
   sim.initialize();

   Network * network = mod.componentNamed<Network>("network_1");
   ofstream outfile;
   outfile.open("test_network_1p.out");
   network->didCompleteTimestep().addAction([&]()
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

   TestLoad & tl0 = mod.newComponent<TestLoad>("tl0", bus2->phases(), seconds(5));
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
   network->didCompleteTimestep().addAction([&]()
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

   TestLoad & tl0 = mod.newComponent<TestLoad>("tl0", bus2->phases(), seconds(5));
   bus2->addZipToGround(tl0);

   mod.validate();
   sim.initialize();

   Network * network = mod.componentNamed<Network>("network_1");
   ofstream outfile;
   outfile.open("test_network_2p_identical.out");
   outfile << dSeconds(sim.currentTime()-sim.startTime()) << " " 
           << bus1->V()(0) << " " << bus2->V()(0) << " " << bus3->V()(0) << std::endl;
   network->didCompleteTimestep().addAction([&]()
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

      virtual Time validUntil() const override
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
   network->didCompleteTimestep().addAction([&]()
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

BOOST_AUTO_TEST_CASE (test_sun_1)
{

}

BOOST_AUTO_TEST_CASE (test_sun)
{
   // Canberra, Australia Lat Long = 35.3075 S, 149.1244 E (-35.3075, 149.1244).
   // Round off to (-35, 149) because reference calculator uses integer latlongs.
   // Take Jan 26 2013, 8:30 AM.
   // This is daylight savings time, UMT +11 hours.
   // http://pveducation.org/pvcdrom/properties-of-sunlight/sun-position-calculator is reference:
   // Values from website are: zenith: 64.47, azimuth: 96.12 (i.e. nearly due east, 0 is north, 90 is east).

   using namespace boost::gregorian;
   time_zone_ptr tz(new posix_time_zone("AEST10AEDT,M10.5.0/02,M3.5.0/03"));

   SphericalAngles sunCoords = sunPos(utcTimeFromLocalTime(ptime(date(2013, Jan, 26), hours(8) + minutes(30)), tz), 
         {-35.0, 149.0});
   message() << "UTC time:  " << utcTimeFromLocalTime(ptime(date(2013, Jan, 26), hours(8) + minutes(30)), tz) 
             << std::endl;
   message() << "Zenith:    " << sunCoords.zenith * 180 / pi << " expected: " << 64.47 << std::endl;
   message() << "Azimuth:   " << sunCoords.azimuth * 180 / pi << " expected: " << 96.12 << std::endl;

   BOOST_CHECK(std::abs(sunCoords.zenith * 180 / pi - 64.47) < 1.25); // 5 minutes error.
   BOOST_CHECK(std::abs(sunCoords.azimuth * 180 / pi - 96.12) < 1.25); // 5 minutes error.

   Model mod;
   Simulation sim(mod);
   Parser & p = Parser::globalParser();
   p.parse("test_sun.yaml", mod, sim);

   RegularUpdateComponent & clock1 = mod.newComponent<RegularUpdateComponent>("clock1");
   clock1.setDt(minutes(10));

   ofstream outfile;
   outfile.open("test_sun.out");

   clock1.didCompleteTimestep().addAction([&]() 
         {
            SphericalAngles sunCoords = sunPos(utcTime(clock1.time()), mod.latLong());
            outfile << dSeconds(clock1.time() - sim.startTime())/(24 * 3600) << " " << sunCoords.zenith << " "
                    << solarPower(sunCoords, {0.0, 0.0}, 1.0) << std::endl;
         }, "clock1 update");

   mod.validate();
   sim.initialize();
   while (sim.doNextUpdate())
   {
   }
   outfile.close();
}

BOOST_AUTO_TEST_CASE (test_solar_PV)
{
   Model mod;
   Simulation sim(mod);
   Parser & p = Parser::globalParser();
   p.parse("test_solar_PV.yaml", mod, sim);

   SolarPV * spv2 = mod.componentNamed<SolarPV>("solar_PV_bus_2");
   InverterBase * inv2 = mod.componentNamed<InverterBase>("inverter_bus_2");
   Bus * bus2 = mod.componentNamed<Bus>("bus_2");
   Network * network = mod.componentNamed<Network>("network_1");

   ofstream outfile;
   outfile.open("test_solar_PV.out");

   network->didCompleteTimestep().addAction([&]()
         {
            outfile << dSeconds(sim.currentTime()-sim.startTime())/3600 << " " << spv2->PDC() << " " << inv2->S()(0)
                    << " " << bus2->V()(0) << std::endl;
         }, "Network updated.");
   mod.validate();
   sim.initialize();

   while (sim.doNextUpdate())
   {
   }
   outfile.close();
}

BOOST_AUTO_TEST_CASE (test_loops)
{
   Model mod;
   Simulation sim(mod);
   Parser & p = Parser::globalParser();
   p.parse("test_loops.yaml", mod, sim);

   SolarPV * spv2 = mod.componentNamed<SolarPV>("solar_PV_bus_2");
   InverterBase * inv2 = mod.componentNamed<InverterBase>("inverter_bus_2_0");
   Bus * bus2 = mod.componentNamed<Bus>("bus_2_1");
   Network * network = mod.componentNamed<Network>("network_1");

   ofstream outfile;
   outfile.open("test_loops.out");

   network->didCompleteTimestep().addAction([&]()
         {
            outfile << dSeconds(sim.currentTime()-sim.startTime())/3600 << " " << spv2->PDC() << " " << inv2->S()(0)
                    << " " << bus2->V()(0) << std::endl;
            std::cout << "timestep " << sim.currentTime()-sim.startTime() << std::endl;
         }, "Network updated.");
   mod.validate();
   sim.initialize();

   while (sim.doNextUpdate())
   {
   }
   outfile.close();
}

static void prepareMPInput(const std::string & yamlName, const std::string & caseName)
{
   std::fstream yamlFile(yamlName, ios_base::out);
   if (!yamlFile.is_open())
   {
      error() << "Could not open the yaml outputfile file " << yamlName << "." << std::endl;
      SmartGridToolbox::abort();
   }

   yamlFile << "global:" << std::endl;
   yamlFile << "   configuration_name:        config_1" << std::endl;
   yamlFile << "   start_time:                2013-01-23 13:13:00" << std::endl;
   yamlFile << "   end_time:                  2013-01-23 15:13:00" << std::endl;
   yamlFile << "   lat_long:                  [-35.3075, 149.1244] # Canberra, Australia." << std::endl;
   yamlFile << "   timezone:                  AEST10AEDT,M10.5.0/02,M3.5.0/03 # Timezone info for Canberra, Australia."
            << std::endl;
   yamlFile << "objects: # And these are the actual objects." << std::endl;
   yamlFile << "   matpower:" << std::endl;
   yamlFile << "      input_file:             " << caseName << std::endl;
   yamlFile << "      network_name:           matpower" << std::endl;
   yamlFile << "      use_per_unit:           N" << std::endl;

   yamlFile.close();
}

static void readMPOutput(const std::string & fileName, double & SBase, UblasVector<int> & iBus,
      UblasVector<double> & VBase, UblasVector<Complex> & V, UblasVector<Complex> & Sc, UblasVector<Complex> & Sg)
{
   std::fstream infile(fileName);
   if (!infile.is_open())
   {
      error() << "Could not open the matpower compare file " << fileName << "." << std::endl;
      SmartGridToolbox::abort();
   }
   infile >> SBase;
   int nBus;
   infile >> nBus;
   message() << "Matpower output: nBus = " << nBus << std::endl;
   iBus.resize(nBus, false);
   VBase.resize(nBus, false);
   V.resize(nBus, false);
   Sc.resize(nBus, false);
   Sg.resize(nBus, false);
   for (int i = 0; i < nBus; ++i)
   {
      int ib;
      double Vb;
      double Vr; double Vi;
      double Scr; double Sci;
      double Sgr; double Sgi;
      infile >> ib >> Vb >> Vr >> Vi >> Scr >> Sci >> Sgr >> Sgi;
      iBus(i) = ib;
      VBase(i) = Vb;
      V(i) = Complex(Vr, Vi);
      Sc(i) = Complex(Scr, Sci);
      Sg(i) = Complex(Sgr, Sgi);
   }
}

static void testMatpower(const std::string & baseName)
{
   std::string caseName = baseName + ".m";
   std::string yamlName = "test_mp_" + baseName + ".yaml";
   std::string compareName = "test_mp_" + baseName + ".compare";

   Model mod;
   Simulation sim(mod);
   Parser & p = Parser::globalParser();
   
   prepareMPInput(yamlName, caseName);

   p.parse(yamlName.c_str(), mod, sim);
   mod.validate();
   sim.initialize();
   Network * network = mod.componentNamed<Network>("matpower");
   network->solvePowerFlow();

   double SBase;
   UblasVector<int> iBus;
   UblasVector<double> VBase;
   UblasVector<Complex> V;
   UblasVector<Complex> Sc;
   UblasVector<Complex> Sg;
   readMPOutput(compareName, SBase, iBus, VBase, V, Sc, Sg);

   double STol = SBase * 1e-4;

   for (int i = 0; i < iBus.size(); ++i)
   {
      int ib = iBus(i);
      std::string busName = "matpower_bus_" + std::to_string(ib);
      Bus * bus = mod.componentNamed<Bus>(busName);
      assert(bus != nullptr);
      double VTol = VBase(i) * 1e-4;

      message() << "V tolerance = " << VTol << std::endl;
      message() << "S tolerance = " << STol << std::endl;
      message() << std::endl;

      message() << setw(24) << std::left << busName
                << setw(24) << std::left << "V"
                << setw(24) << std::left << "Sc"
                << setw(24) << std::left << "Sg"
                << std::endl;
      message() << setw(24) << left << "SGT"
                << setw(24) << left << bus->V()(0)
                << setw(24) << left << bus->Sc()(0)
                << setw(24) << left << bus->SGen()(0)
                << std::endl;
      message() << setw(24) << left << "Matpower"
                << setw(24) << left << V(i) 
                << setw(24) << left << Sc(i)
                << setw(24) << left << Sg(i)
                << std::endl; 
      message() << std::endl;

      BOOST_CHECK(abs(bus->V()(0) - V(i)) < VTol);
      BOOST_CHECK(abs(bus->Sc()(0) - Sc(i)) < STol);
      BOOST_CHECK(abs(bus->SGen()(0) - Sg(i)) < STol);
   }
}

BOOST_AUTO_TEST_CASE (test_mp_SLPQ)
{
   testMatpower("caseSLPQ");
}

BOOST_AUTO_TEST_CASE (test_mp_SLPV)
{
   testMatpower("caseSLPV");
}

BOOST_AUTO_TEST_CASE (test_mp_SLPQPV)
{
   testMatpower("caseSLPQPV");
}

BOOST_AUTO_TEST_CASE (test_mp_4gs)
{
   testMatpower("case4gs");
}

BOOST_AUTO_TEST_CASE (test_mp_6ww)
{
   testMatpower("case6ww");
}

BOOST_AUTO_TEST_CASE (test_mp_9)
{
   testMatpower("case9");
}

BOOST_AUTO_TEST_CASE (test_mp_trans3)
{
   testMatpower("case_trans3");
}

BOOST_AUTO_TEST_CASE (test_mp_14)
{
   testMatpower("case14");
}

BOOST_AUTO_TEST_CASE (test_mp_14_shift)
{
   testMatpower("case14_shift");
}

BOOST_AUTO_TEST_SUITE_END( )
