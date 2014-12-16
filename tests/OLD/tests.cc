BOOST_AUTO_TEST_CASE (test_dependencies)
{
   Simulation sim;

   SimBus & a0 = sim.newComponent<SimBus>("bus0", 0, 0.1);
   SimBus & a1 = sim.newComponent<SimBus>("bus1", 1, 0.1);
   SimBus & a2 = sim.newComponent<SimBus>("bus2", 2, 0.1);
   SimBus & a3 = sim.newComponent<SimBus>("bus3", 3, 0.1);
   SimBus & a4 = sim.newComponent<SimBus>("bus4", 4, 0.1);
   SimBus & a5 = sim.newComponent<SimBus>("bus5", 5, 0.1);

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

BOOST_AUTO_TEST_CASE (test_simple_battery)
{
   local_time::time_zone_ptr tz(new local_time::posix_time_zone("UTC0"));
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
   bat1.initialize();
   bat1.update(timeFromLocalTime(posix_time::ptime(gregorian::date(2012, Feb, 11), posix_time::hours(2)), tz));
   Log().message() << "1 Battery charge = " << bat1.charge() / kWh << endl;
   bat1.update(bat1.time() + posix_time::hours(3));
   Log().message() << "2 Battery charge = " << bat1.charge() / kWh << endl;
   double comp = bat1.initCharge() + 
      dSeconds(posix_time::hours(3)) * bat1.requestedPower() /
      bat1.dischargeEfficiency();
   Log().message() << "comp = " << comp / kWh << endl;
   BOOST_CHECK(bat1.charge() == comp);

   bat1.setRequestedPower(1.3 * kW);
   bat1.initialize();
   bat1.update(timeFromLocalTime(posix_time::ptime(gregorian::date(2012, Feb, 11), posix_time::hours(2)), tz));
   Log().message() << "3 Battery charge = " << bat1.charge() / kWh << endl;
   bat1.update(bat1.time() + posix_time::hours(3));
   Log().message() << "4 Battery charge = " << bat1.charge() / kWh << endl;
   comp = bat1.initCharge() + 
      dSeconds(posix_time::hours(3)) * bat1.maxChargePower() *
      bat1.chargeEfficiency();
   Log().message() << "comp = " << comp / kWh << endl;
   BOOST_CHECK(bat1.charge() == comp);

   bat1.setRequestedPower(-1 * kW);
   bat1.initialize();
   bat1.update(timeFromLocalTime(posix_time::ptime(gregorian::date(2012, Feb, 11), posix_time::hours(2)), tz));
   Log().message() << "3 Battery charge = " << bat1.charge() / kWh << endl;
   bat1.update(bat1.time() + posix_time::hours(5) + posix_time::minutes(30));
   Log().message() << "4 Battery charge = " << bat1.charge() / kWh << endl;
   comp = 0.0;
   Log().message() << "comp = " << comp / kWh << endl;
   BOOST_CHECK(bat1.charge() == comp);
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
         startTime_ = time();
         nextUpdate_ = time() + dt_; 
      }

      // Bring state up to time t_.
      virtual void updateState(Time t) override
      {
         Log().message() << "Update state of " << name() << " from time " 
              << time() << " to " << t << "." << endl;
         state_ = (t - startTime_).ticks() * ctrl_;
         nextUpdate_ = t + dt_;
      }

   private:
      Time nextUpdate_;
      Time startTime_;
      int state_;
      time_duration dt_;
      int ctrl_;
};

BOOST_AUTO_TEST_CASE (test_events_and_sync)
{
   Model mod;
   TestEventA & a0 = mod.newComponent<TestEventA>("a0", posix_time::seconds(3), 3);
   TestEventA & a1 = mod.newComponent<TestEventA>("a1", posix_time::seconds(9), 3);
   a0.dependsOn(a1); 
   mod.validate();
   Simulation sim(mod);
   sim.setStartTime(posix_time::seconds(0));
   sim.setEndTime(posix_time::seconds(10));
   sim.initialize();

   a0.didCompleteTimestep().addAction(
         [&]() {Log().message() << a1.name() << " received did update from " << a0.name() << std::endl;},
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

BOOST_AUTO_TEST_CASE (test_sparse_solver)
{
   int n = 5;
   ublas::compressed_matrix<double> a(n, n);
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

   ublas::vector<double> b = makeVec({8.0, 45.0, -3.0, 3.0, 19.0});

   ublas::vector<double> x(n);
   kluSolve(a, b, x);
   Log().message(); for (int i = 0; i < n; ++i) std::cout << x(i) << " ";
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

      virtual ublas::vector<Complex> Y() const override 
      {
         return ublas::vector<Complex>(phases().size(), sin(dSeconds(time()) / 123.0));
      }

      virtual ublas::vector<Complex> I() const override 
      {
         return ublas::vector<Complex>(phases().size(), 
               sin(dSeconds(time()) / 300.0) * exp(Complex(0.0, dSeconds(time()) / 713.0)));
      }

      virtual ublas::vector<Complex> S() const override 
      {
         return ublas::vector<Complex>(phases().size(), sin(dSeconds(time()) / 60.0));
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
   p.parse("test_network_1p.yaml", mod, sim); p.postParse();

   Bus * bus1 = mod.component<Bus>("bus_1");
   Bus * bus2 = mod.component<Bus>("bus_2");
   Bus * bus3 = mod.component<Bus>("bus_3");

   TestLoad & tl0 = mod.newComponent<TestLoad>("tl0", bus2->phases(), posix_time::seconds(5));
   bus2->addZipToGround(tl0);

   mod.validate();
   sim.initialize();

   Network * network = mod.component<Network>("network_1");
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
   p.parse("test_network_b3p.yaml", mod, sim); p.postParse();

   Bus * bus1 = mod.component<Bus>("bus_1");
   Bus * bus2 = mod.component<Bus>("bus_2");
   Bus * bus3 = mod.component<Bus>("bus_3");

   TestLoad & tl0 = mod.newComponent<TestLoad>("tl0", bus2->phases(), posix_time::seconds(5));
   bus2->addZipToGround(tl0);

   mod.validate();
   sim.initialize();

   Network * network = mod.component<Network>("network_1");
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
   p.parse("test_network_2p_identical.yaml", mod, sim); p.postParse();

   Bus * bus1 = mod.component<Bus>("bus_1");
   Bus * bus2 = mod.component<Bus>("bus_2");
   Bus * bus3 = mod.component<Bus>("bus_3");

   TestLoad & tl0 = mod.newComponent<TestLoad>("tl0", bus2->phases(), posix_time::seconds(5));
   bus2->addZipToGround(tl0);

   mod.validate();
   sim.initialize();

   Network * network = mod.component<Network>("network_1");
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

class TestDc : public DcPowerSourceBase
{
   public:
      TestDc(const std::string & name) : DcPowerSourceBase(name), dt_(posix_time::seconds(0)) {}

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

      virtual double PDc() const override
      {
         return sin(dSeconds(time()) / 60.0);
      }

   private:
      time_duration dt_;
};

BOOST_AUTO_TEST_CASE (test_sun)
{
   // Canberra, Australia Lat Long = 35.3075 S, 149.1244 E (-35.3075, 149.1244).
   // Round off to (-35, 149) because reference calculator uses integer latlongs.
   // Take Jan 26 2013, 8:30 AM.
   // This is daylight savings time, UMT +11 hours.
   // http://pveducation.org/pvcdrom/properties-of-sunlight/sun-position-calculator is reference:
   // Values from website are: zenith: 64.47, azimuth: 96.12 (i.e. nearly due east, 0 is north, 90 is east).

   using namespace boost::gregorian;
   local_time::time_zone_ptr tz(new local_time::posix_time_zone("AEST10AEDT,M10.5.0/02,M3.5.0/03"));

   SphericalAngles sunCoords = sunPos(utcTimeFromLocalTime(posix_time::ptime(gregorian::date(2013, Jan, 26), posix_time::hours(8) + posix_time::minutes(30)), tz), 
         {-35.0, 149.0});
   Log().message() << "UTC time:  " << utcTimeFromLocalTime(posix_time::ptime(gregorian::date(2013, Jan, 26), posix_time::hours(8) + posix_time::minutes(30)), tz) 
             << std::endl;
   Log().message() << "Zenith:    " << sunCoords.zenith * 180 / pi << " expected: " << 64.47 << std::endl;
   Log().message() << "Azimuth:   " << sunCoords.azimuth * 180 / pi << " expected: " << 96.12 << std::endl;

   BOOST_CHECK(std::abs(sunCoords.zenith * 180 / pi - 64.47) < 1.25); // 5 minutes error.
   BOOST_CHECK(std::abs(sunCoords.azimuth * 180 / pi - 96.12) < 1.25); // 5 minutes error.

   Model mod;
   Simulation sim(mod);
   Parser & p = Parser::globalParser();
   p.parse("test_sun.yaml", mod, sim); p.postParse();

   RegularUpdateComponent & clock1 = mod.newComponent<RegularUpdateComponent>("clock1");
   clock1.setDt(posix_time::minutes(10));

   ofstream outfile;
   outfile.open("test_sun.out");

   clock1.didCompleteTimestep().addAction([&]() 
         {
            SphericalAngles sunCoords = sunPos(utcTime(clock1.time()), mod.latLong());
            outfile << dSeconds(clock1.time() - sim.startTime()) / (24 * 3600) << " " << sunCoords.zenith << " "
                    << solarPower(sunCoords, {0.0, 0.0}, 1.0) << std::endl;
         }, "clock1 update");

   mod.validate();
   sim.initialize();
   while (sim.doNextUpdate())
   {
   }
   outfile.close();
}

BOOST_AUTO_TEST_CASE (test_solar_pv)
{
   Model mod;
   Simulation sim(mod);
   Parser & p = Parser::globalParser();
   p.parse("test_solar_pv.yaml", mod, sim); p.postParse();

   SolarPv * spv2 = mod.component<SolarPv>("solar_pv_bus_2");
   InverterBase * inv2 = mod.component<InverterBase>("inverter_bus_2");
   Bus * bus2 = mod.component<Bus>("bus_2");
   Network * network = mod.component<Network>("network_1");

   ofstream outfile;
   outfile.open("test_solar_pv.out");

   network->didCompleteTimestep().addAction([&]()
         {
            outfile << dSeconds(sim.currentTime()-sim.startTime()) / 3600 << " " << spv2->PDc() << " " << inv2->S()(0)
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
   p.parse("test_loops.yaml", mod, sim); p.postParse();

   SolarPv * spv2 = mod.component<SolarPv>("solar_pv_bus_2");
   InverterBase * inv2 = mod.component<InverterBase>("inverter_bus_2_0");
   Bus * bus2 = mod.component<Bus>("bus_2_1");
   Network * network = mod.component<Network>("network_1");

   ofstream outfile;
   outfile.open("test_loops.out");

   network->didCompleteTimestep().addAction([&]()
         {
            outfile << dSeconds(sim.currentTime()-sim.startTime()) / 3600 << " " << spv2->PDc() << " " << inv2->S()(0)
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

BOOST_AUTO_TEST_CASE (test_network_overhead)
{
   Model mod;
   Simulation sim(mod);

   Parser & p = Parser::globalParser();
   p.parse("test_network_overhead.yaml", mod, sim); p.postParse();

   Bus * bus1 = mod.component<Bus>("bus_1");
   Bus * bus2 = mod.component<Bus>("bus_2");

   mod.validate();
   sim.initialize();

   Network * network = mod.component<Network>("network_1");
   network->solvePowerFlow();

   Log().message() << bus1->V()(0) << " " << bus1->V()(1) << " " << bus1->V()(2) << std::endl;
   Log().message() << bus2->V()(0) << " " << bus2->V()(1) << " " << bus2->V()(2) <<  std::endl;
   Log().message() << std::endl;

   Log().message() << abs(bus1->V()(0)) << " " << abs(bus1->V()(1)) << " " << abs(bus1->V()(2)) <<  std::endl;
   Log().message() << abs(bus2->V()(0)) << " " << abs(bus2->V()(1)) << " " << abs(bus2->V()(2)) <<  std::endl;
   Log().message() << std::endl;

   Log().message() << bus1->STot()(0) << " " << bus1->STot()(1) << " " << bus1->STot()(2) << std::endl;
   Log().message() << bus2->STot()(0) << " " << bus2->STot()(1) << " " << bus2->STot()(2) <<  std::endl;
   Log().message() << std::endl;
}

static void prepareCDFInput(const std::string & yamlName, const std::string & caseName, bool usePerUnit)
{
   std::fstream yamlFile(yamlName, ios_base::out);
   if (!yamlFile.is_open())
   {
      Log().error() << "Could not open the yaml output file " << yamlName << "." << std::endl;
      SmartGridToolbox::abort();
   }

   yamlFile << "configuration_name:           config_1" << std::endl;
   yamlFile << "start_time:                   2013-01-23 13:13:00" << std::endl;
   yamlFile << "end_time:                     2013-01-23 15:13:00" << std::endl;
   yamlFile << "lat_long:                     [-35.3075, 149.1244] # Canberra, Australia." << std::endl;
   yamlFile << "timezone:                     AEST10AEDT,M10.5.0/02,M3.5.0/03 # Timezone info for Canberra, Australia."
            << std::endl;
   yamlFile << "components:" << std::endl;
   yamlFile << "   cdf:" << std::endl;
   yamlFile << "      input_file:             " << caseName << std::endl;
   yamlFile << "      network_name:           cdf" << std::endl;
   yamlFile << "      default_V_base:         1000 # 1 KV default." << std::endl;
   if (usePerUnit)
   {
      yamlFile << "      use_per_unit:           Y" << std::endl;
   }

   yamlFile.close();
}

static void testCDF(const std::string & baseName, bool usePerUnit)
{
   std::string caseName = baseName + ".cdf";
   std::string yamlName = "test_cdf_" + baseName + ".yaml";
   std::string compareName = "test_cdf_" + baseName + ".compare";

   Model mod;
   Simulation sim(mod);
   Parser & p = Parser::globalParser();
   
   prepareCDFInput(yamlName, caseName, usePerUnit);

   p.parse(yamlName.c_str(), mod, sim); p.postParse();
   mod.validate();
   sim.initialize();
   Network * network = mod.component<Network>("cdf");
   network->solvePowerFlow();

   double SBase;
   ublas::vector<int> iBus;
   ublas::vector<double> VBase;
   ublas::vector<Complex> V;
   ublas::vector<Complex> Sc;
   ublas::vector<Complex> Sg;
   readMPOutput(compareName, usePerUnit, SBase, iBus, VBase, V, Sc, Sg);

   double STol = usePerUnit ? 1e-4 : SBase * 1e-4;

   for (int i = 0; i < iBus.size(); ++i)
   {
      int ib = iBus(i);
      std::string busName = "cdf_bus_" + num2PaddedString5(ib);
      Bus * bus = mod.component<Bus>(busName);
      assert(bus != nullptr);
      double VTol = usePerUnit ? 1e-4 : VBase(i) * 1e-4;

      Log().message() << "V tolerance = " << VTol << std::endl;
      Log().message() << "S tolerance = " << STol << std::endl;
      Log().message() << std::endl;

      Log().message() << setw(24) << std::left << busName
                << setw(24) << std::left << "V"
                << setw(24) << std::left << "Sc"
                << setw(24) << std::left << "Sg"
                << std::endl;
      Log().message() << setw(24) << left << "SGT"
                << setw(24) << left << bus->V()(0)
                << setw(24) << left << bus->Sc()(0)
                << setw(24) << left << bus->Sg()(0)
                << std::endl;
      Log().message() << setw(24) << left << "Matpower"
                << setw(24) << left << V(i) 
                << setw(24) << left << Sc(i)
                << setw(24) << left << Sg(i)
                << std::endl; 
      Log().message() << std::endl;

      BOOST_CHECK(abs(bus->V()(0) - V(i)) < VTol);
      BOOST_CHECK(abs(bus->Sc()(0) - Sc(i)) < STol);
      BOOST_CHECK(abs(bus->Sg()(0) - Sg(i)) < STol);
   }
}

BOOST_AUTO_TEST_CASE (test_cdf_14)
{
   testCDF("ieee14", true);
   testCDF("ieee14", false);
}

BOOST_AUTO_TEST_CASE (test_transformers)
{
   Model mod;
   Simulation sim(mod);

   Parser & p = Parser::globalParser();
   p.parse("test_transformers.yaml", mod, sim); p.postParse();

   mod.validate();
   sim.initialize();

   Bus * bus1 = mod.component<Bus>("bus_1");
   Bus * bus2 = mod.component<Bus>("bus_2");
   Bus * bus3 = mod.component<Bus>("bus_3");

   Network * network = mod.component<Network>("network_1");
   Log().message() << *network << std::endl;
   network->solvePowerFlow();

   Log().message() << bus1->V()(0) << " " << bus1->V()(1) << " " << bus1->V()(2) << std::endl;
   Log().message() << bus2->V()(0) << " " << bus2->V()(1) << " " << bus2->V()(2) <<  std::endl;
   Log().message() << bus3->V()(0) << " " << bus3->V()(1) << " " << bus3->V()(2) <<  std::endl;
   Log().message() << std::endl;

   Log().message() << abs(bus1->V()(0)) << " " << abs(bus1->V()(1)) << " " << abs(bus1->V()(2)) <<  std::endl;
   Log().message() << abs(bus2->V()(0)) << " " << abs(bus2->V()(1)) << " " << abs(bus2->V()(2)) <<  std::endl;
   Log().message() << abs(bus3->V()(0)) << " " << abs(bus3->V()(1)) << " " << abs(bus3->V()(2)) <<  std::endl;
   Log().message() << std::endl;

   Log().message() << bus1->STot()(0) << " " << bus1->STot()(1) << " " << bus1->STot()(2) << std::endl;
   Log().message() << bus2->STot()(0) << " " << bus2->STot()(1) << " " << bus2->STot()(2) <<  std::endl;
   Log().message() << bus3->STot()(0) << " " << bus3->STot()(1) << " " << bus3->STot()(2) <<  std::endl;
   Log().message() << std::endl;
}
