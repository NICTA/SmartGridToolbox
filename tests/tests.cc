// Copyright 2015 National ICT Australia Limited (NICTA)
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#define BOOST_TEST_MODULE test_template

#include <boost/test/included/unit_test.hpp>

#include "../SgtCore/SgtCore.h"
#include "../SgtSim/SgtSim.h"
#include "../SgtSim/WeakOrder.h"

#include <cmath>
#include <ostream>
#include <fstream>
#include <set>
#include <vector>

using namespace arma;
using namespace Sgt;
using namespace std;
using namespace boost::posix_time;
using namespace boost::unit_test;

struct Init
{
    Init()
    {
        unit_test_log.set_threshold_level(boost::unit_test::log_messages);
        BOOST_TEST_MESSAGE("\nTesting " << framework::current_test_case().p_name);
    }
    ~Init()
    {
        BOOST_TEST_MESSAGE("Finished " << framework::current_test_case().p_name << "\n");
    }
};

BOOST_FIXTURE_TEST_SUITE(tests, Init)

BOOST_AUTO_TEST_CASE (test_overhead_compare_carson_1)
{
    // For this test, we compare values for an overhead line examined in one of Kersting's papers:
    // docs/background/Kersting_Carson.pdf.
    Network netw;
    Parser<Network> p;
    p.parse("test_overhead_compare_carson_1.yaml", netw);

    auto oh = netw.branches()["line_1_2"].as<OverheadLine>();

    Mat<Complex> ZPrim = oh->ZPrim();
    Mat<Complex> ZPhase = oh->ZPhase();

    Complex cmp;
    cmp = {1.3369, 1.3331};
    double err00 = abs(ZPhase(0,0) - cmp) / abs(cmp);
    cmp = {0.2102, 0.5778};
    double err01 = abs(ZPhase(0, 1) - cmp) / abs(cmp);
    cmp = {0.2132, 0.5014};
    double err02 = abs(ZPhase(0, 2) - cmp) / abs(cmp);
    cmp = {1.3239, 1.3557};
    double err11 = abs(ZPhase(1, 1) - cmp) / abs(cmp);
    cmp = {0.2067, 0.4591};
    double err12 = abs(ZPhase(1, 2) - cmp) / abs(cmp);
    cmp = {1.3295, 1.3459};
    double err22 = abs(ZPhase(2, 2) - cmp) / abs(cmp);

    BOOST_CHECK(err00 < 0.001);
    BOOST_CHECK(err01 < 0.001);
    BOOST_CHECK(err02 < 0.001);
    BOOST_CHECK(err11 < 0.001);
    BOOST_CHECK(err12 < 0.001);
    BOOST_CHECK(err22 < 0.001);

    BOOST_CHECK(std::abs(ZPhase(0,1) - ZPhase(1,0)) < 1e-6);
    BOOST_CHECK(std::abs(ZPhase(0,2) - ZPhase(2,0)) < 1e-6);
    BOOST_CHECK(std::abs(ZPhase(1,2) - ZPhase(2,1)) < 1e-6);
}

BOOST_AUTO_TEST_CASE (test_overhead_compare_carson_2)
{
    // For this test, we compare values for an overhead line examined in one of Kersting's papers:
    // docs/background/Kersting_Carson.pdf.
    Network netw;
    Parser<Network> p;
    p.parse("test_overhead_compare_carson_2.yaml", netw);

    netw.solvePowerFlow();

    auto bus2 = netw.buses()["bus_2"];

    Complex cmp;
    cmp = polar(14.60660, -0.62 * pi / 180.0);
    double err0 = abs(bus2->V()(0) - cmp) / abs(cmp);
    cmp = polar(14.72669, -121.0 * pi / 180.0);
    double err1 = abs(bus2->V()(1) - cmp) / abs(cmp);
    cmp = polar(14.80137, 119.2 * pi / 180.0);
    double err2 = abs(bus2->V()(2) - cmp) / abs(cmp);

    BOOST_CHECK(err0 < 0.001);
    BOOST_CHECK(err1 < 0.001);
    BOOST_CHECK(err2 < 0.001);
}

BOOST_AUTO_TEST_CASE (test_underground_conc_compare_carson)
{
    // For this test, we compare values for an underground line examined in one of Kersting's papers:
    // docs/background/Kersting_Carson.pdf.
    Network netw;
    Parser<Network> p;
    p.parse("test_underground_conc_compare_carson.yaml", netw);

    netw.solvePowerFlow();

    auto ug = netw.branches()["line_1_2"].as<UndergroundLine>();
    Mat<Complex> ZPhase = ug->ZPhase() * 1609.344; // Convert to ohms per mile.
    Mat<Complex> ZPhaseKersting;
    ZPhaseKersting << Complex(0.7981, 0.4463) << Complex(0.3191, 0.0328) << Complex(0.2849, -0.0143) << endr
                   << Complex(0.3191, 0.0328) << Complex(0.7891, 0.4041) << Complex(0.3191, 0.0328) << endr
                   << Complex(0.2849, -0.0143) << Complex(0.3191, 0.0328) << Complex(0.7981, 0.4463) << endr;
    BOOST_TEST_MESSAGE(ZPhase);
    BOOST_TEST_MESSAGE(ZPhaseKersting);
    double err = norm(ZPhase - ZPhaseKersting, "inf");
    BOOST_TEST_MESSAGE("Err = " << err);
    BOOST_CHECK(err < 0.0005);
}

BOOST_AUTO_TEST_CASE (test_underground_tape_compare_carson)
{
    // For this test, we compare values for an underground line examined in one of Kersting's papers:
    // docs/background/Kersting_Carson.pdf.
    Network netw;
    Parser<Network> p;
    p.parse("test_underground_tape_compare_carson.yaml", netw);

    netw.solvePowerFlow();

    auto ug = netw.branches()["line_1_2"].as<UndergroundLine>();
    Mat<Complex> ZPrim = ug->ZPrim() * 1609.344; // Convert to ohms per mile.
    Mat<Complex> ZPhase = ug->ZPhase() * 1609.344; // Convert to ohms per mile.
    Complex ZPhaseKersting = Complex(1.3219, 0.6743);
    BOOST_TEST_MESSAGE(ZPhase(0));
    BOOST_TEST_MESSAGE(ZPhaseKersting);
    double err = norm(ZPhase - ZPhaseKersting, "inf");
    BOOST_TEST_MESSAGE("Err = " << err);
    BOOST_CHECK(err < 0.0006);
}

#if 0 // TODO: Redo spline due to license issues
BOOST_AUTO_TEST_CASE (test_spline)
{
    Spline spline;
    //Add points to the spline in any order, they're sorted in ascending
    //x later. (If you want to spline a circle you'll need to change the class)
    spline.addPoint(0, 0.0);
    spline.addPoint(40.0 / 255, 0.0);
    spline.addPoint(60.0 / 255, 0.2);
    spline.addPoint(63.0 / 255, 0.05);
    spline.addPoint(80.0 / 255, 0.0);
    spline.addPoint(82.0 / 255, 0.9);
    spline.addPoint(1.0, 1.0);

    {
        //We can extract the original data points by treating the spline as
        //a read-only STL container.
        std::ofstream of("orig.dat");
        for (Spline::const_iterator iPtr = spline.begin(); iPtr != spline.end(); ++iPtr)
            of << iPtr->first << " " << iPtr->second << "\n";
    }

    {
        //A "natural spline" where the second derivatives are set to 0 at the boundaries.

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
    local_time::time_zone_ptr tz(new local_time::posix_time_zone("UTC0"));
    using namespace boost::gregorian;
    Time base = timeFromLocalTime(posix_time::ptime(gregorian::date(2013, Apr, 26), posix_time::hours(0)), tz);
    SplineTimeSeries<Time> sts;
    sts.addPoint(base + posix_time::hours(0), sin(0 * pi / 12));
    sts.addPoint(base + posix_time::hours(4), sin(4 * pi / 12));
    sts.addPoint(base + posix_time::hours(8), sin(8 * pi / 12));
    sts.addPoint(base + posix_time::hours(12), sin(12 * pi / 12));
    sts.addPoint(base + posix_time::hours(16), sin(16 * pi / 12));
    sts.addPoint(base + posix_time::hours(20), sin(20 * pi / 12));
    sts.addPoint(base + posix_time::hours(24), sin(24 * pi / 12));
    for (int i = -1; i <= 25; ++i)
    {
        double val = sts.value(base + posix_time::hours(i));
        double err = std::abs(val - sin(i * pi / 12));
        if (i > -1 && i < 25)
        {
            BOOST_CHECK(err < 0.005);
        }
    }
}
#endif

BOOST_AUTO_TEST_CASE (test_lerp_timeseries)
{
    local_time::time_zone_ptr tz(new local_time::posix_time_zone("UTC0"));
    using namespace boost::gregorian;
    Time base = timeFromLocalTime(posix_time::ptime(gregorian::date(2013, Apr, 26), posix_time::hours(0)), tz);
    LerpTimeSeries<Time, Complex> lts;
    lts.addPoint(base + posix_time::hours(0), Complex(0, 0));
    lts.addPoint(base + posix_time::hours(1), Complex(3, 1));
    lts.addPoint(base + posix_time::hours(3), Complex(10, 11));

    BOOST_CHECK(lts.value(base + posix_time::hours(-1)) == Complex(0, 0));
    BOOST_CHECK(lts.value(base + posix_time::hours(0)) == Complex(0, 0));
    BOOST_CHECK(lts.value(base + posix_time::minutes(30)) == Complex(1.5, 0.5));
    BOOST_CHECK(lts.value(base + posix_time::hours(1)) == Complex(3, 1));
    BOOST_CHECK(lts.value(base + posix_time::hours(2)) == Complex(6.5, 6));
    BOOST_CHECK(lts.value(base + posix_time::hours(3)) == Complex(10, 11));
    BOOST_CHECK(lts.value(base + posix_time::hours(4) + posix_time::seconds(1)) == Complex(10, 11));
}

BOOST_AUTO_TEST_CASE (test_stepwise_timeseries)
{
    time_duration base(posix_time::minutes(5));
    StepwiseTimeSeries<time_duration, double> sts;
    sts.addPoint(base + posix_time::hours(0), 1.5);
    sts.addPoint(base + posix_time::hours(1), 2.5);
    sts.addPoint(base + posix_time::hours(3), 5.5);

    BOOST_CHECK(sts.value(base + posix_time::seconds(-1)) == 1.5);
    BOOST_CHECK(sts.value(base + posix_time::seconds(1)) == 1.5);
    BOOST_CHECK(sts.value(base + posix_time::hours(1) - posix_time::seconds(1)) == 1.5);
    BOOST_CHECK(sts.value(base + posix_time::hours(1) + posix_time::seconds(1)) == 2.5);
    BOOST_CHECK(sts.value(base + posix_time::hours(3) + posix_time::seconds(1)) == 5.5);
}

BOOST_AUTO_TEST_CASE (test_function_timeseries)
{
    FunctionTimeSeries <time_duration, double> fts([] (time_duration td) {return 2 * dSeconds(td);});
    BOOST_CHECK(fts.value(posix_time::seconds(-1)) == -2.0);
    BOOST_CHECK(fts.value(posix_time::seconds(3)) == 6.0);
}

BOOST_AUTO_TEST_CASE (test_matpower)
{
    BOOST_TEST_MESSAGE("Starting matpower tests");
    using namespace Sgt;

    std::vector<std::string> cases =
    {
        "caseSLPQ",
        "caseSLPV",
        "caseSLPQPV",
        "transformer",
        "nesta_case3_lmbd",
        "case4gs",
        "nesta_case4_gs",
        "case5",
        "nesta_case5_pjm",
        "case6ww",
        "nesta_case6_c",
        "nesta_case6_ww",
        "case9",
        "case9Q",
        "case9target",
        "nesta_case9_wscc",
        "case14",
        "case14_shift",
        "nesta_case14_ieee",
        "case24_ieee_rts",
        "nesta_case29_edin",
        "case30",
        "case30Q",
        "case30_all",
        "case30pwl",
        "case_ieee30",
        "nesta_case30_as",
        "nesta_case30_fsr",
        "nesta_case30_ieee",
        "case39",
        "nesta_case39_epri",
        "case57",
        "nesta_case57_ieee",
        "nesta_case73_ieee_rts",
        "case89pegase",
        "nesta_case89_pegase",
        "case118",
        "nesta_case118_ieee",
        "nesta_case162_ieee_dtc",
        "nesta_case189_edin",
        "case300",
        "nesta_case300_ieee",
        "case1354pegase",
        "nesta_case1354_pegase",
        "nesta_case1394sop_eir",
        "nesta_case1397sp_eir",
        "nesta_case1460wp_eir",
        "nesta_case2224_edin",
        "case2383wp",
        "nesta_case2383wp_mp",
        "case2736sp",
        "nesta_case2736sp_mp",
        "case2737sop",
        "nesta_case2737sop_mp",
        "case2746wop",
        "case2746wp",
        "nesta_case2746wop_mp",
        "nesta_case2746wp_mp",
        "case2869pegase",
        "nesta_case2869_pegase",
        "case3012wp", // TODO This fails, probably because SGT doesn't enforce reactive limits. See e.g. bus 70.
        "nesta_case3012wp_mp",
        "case3120sp",
        "nesta_case3120sp_mp",
        "case3375wp", // Can't solve correctly, but then neither can Matpower...
        "nesta_case3375wp_mp", // See above, doesn't converge.
        "case9241pegase",
        "nesta_case9241_pegase"

    };

    std::set<std::string> polExclude = {
        "nesta_case1394sop_eir", // Flat start doesn't converge, neither does matpower.
        "nesta_case1460wp_eir", // Flat start doesn't converge, neither does matpower.
        "case3012wp", // Flat start doesn't converge, neither does matpower.
        "nesta_case3012wp_mp", // Flat start doesn't converge, neither does matpower.
        "case3375wp", // Flat start doesn't converge, neither does matpower.
        "nesta_case3375wp_mp" // Flat start doesn't converge, neither does matpower.
    };
    
    std::set<std::string> rectExclude = {
        "nesta_case300_ieee", // Doesn't converge.
        "case1354pegase", // Doesn't converge.
        "nesta_case1354_pegase", // Doesn't converge.
        "case2869pegase", // Doesn't converge.
        "nesta_case2869_pegase", // Doesn't converge.
        "case3375wp", // Doesn't converge, neither does matpower.
        "nesta_case3375wp_mp", // Doesn't converge, neither does matpower.
        "case9241pegase" // Doesn't converge.
    };

    std::map<std::string, std::pair<double, double>> times;
    for (auto c : cases) times[c] = {-1, -1};

    auto doTest = [&](const decltype(cases)& cs, const std::string& solverType, const std::set<std::string>& exclude)
    {
        for (auto c : cs)
        {
            if (exclude.find(c) != exclude.end())
            {
                continue;
            }

            BOOST_TEST_MESSAGE("Case " << c);

            std::string yamlStr = std::string("--- [{matpower : {input_file : matpower_test_cases/") 
                + c + ".m, default_kV_base : 11}, power_flow_solver : " + solverType + "}]";
            Network nw(100.0);
            nw.setUseFlatStart(true);
            YAML::Node n = YAML::Load(yamlStr);
            Parser<Network> p;
            p.parse(n, nw);

            Stopwatch sw;
            sw.start();
            bool ok = nw.solvePowerFlow();
            sw.stop();

            double& tRef = (solverType == "nr_pol") ? times[c].first : times[c].second;
            if (!ok)
            {
                tRef = -1;
                BOOST_ERROR("Case " << c << " could not be solved.\n");
                continue;
            }

            tRef = sw.cpuSeconds();
            BOOST_TEST_MESSAGE("Solve time = " << sw.cpuSeconds());

            ifstream compareName(std::string("mp_compare/") + c + ".compare");

            int nBadV = 0;
            int nBadS = 0;
            for (auto bus : nw.buses())
            {
                double Vr, Vi, P, Q;
                compareName >> Vr >> Vi >> P >> Q;
                assert(!compareName.eof());

                if (!bus->isInService()) continue;

                Complex V = {Vr, Vi};
                Complex S = {P, Q};
                if (std::abs(V - bus->V()(0) / bus->VBase()) >= 1e-3)
                {
                    std::cout << "Bus V mismatch at bus " << bus->id() << ": " 
                        << V << " : " << bus->V() << std::endl;
                    ++nBadV;
                }
                if (std::abs(S - bus->SGen()(0)) / nw.PBase() >= 1e-3)
                {
                    std::cout << "Bus S mismatch at bus " << bus->id() << ": " 
                        << S << " : " << bus->SGen() << std::endl;
                    ++nBadS;
                }
            }
            BOOST_CHECK_MESSAGE(nBadV == 0, c << ": V doesn't agree with Matpower at " << nBadV << " buses.\n");
            BOOST_CHECK_MESSAGE(nBadS == 0, c << ": S doesn't agree with Matpower at " << nBadS << " buses.\n");
        }
    };

    // doTest(cases, "nr_pol", polExclude);
    doTest(cases, "nr_rect", rectExclude);

    BOOST_TEST_MESSAGE("Times(pol, rect)");
    BOOST_TEST_MESSAGE("----------------");
    for (auto elem : times)
    {
        BOOST_TEST_MESSAGE(setw(32) << elem.first << " => (" << setw(16) << elem.second.first << ", " << setw(16) 
                << elem.second.second << ")");
    }
}

BOOST_AUTO_TEST_CASE (test_const_I)
{
    using namespace Sgt;

    std::string yamlStr(
            "--- [{matpower : {input_file : matpower_test_cases/nesta_case189_edin.m, " "default_kV_base : 11}}]");
    Network nw(100.0);
    nw.setUseFlatStart(true);
    YAML::Node n = YAML::Load(yamlStr);
    Parser<Network> p;
    p.parse(n, nw);

    std::string bus1Id = "bus_174";
    auto bus1 = nw.buses()[bus1Id]; // No existing zip generation. 
    assert(bus1->zips().size() == 0);

    std::string branchId = "branch_194_bus_173_bus_174";
    auto branch = nw.branches()[branchId].as<CommonBranch>();

    auto bus0 = branch->bus0();

    auto zip = std::make_shared<GenericZip>("zip_I_const", Phases{Phase::BAL});
    Complex Ic = std::polar(1.0, 15.0 * pi / 180.0) * 0.789;
    zip->setIConst({Ic});
    nw.addZip(zip, bus1Id);

    // for (std::string solverType : {"nr_pol", "nr_rect"})
    for (std::string solverType : {"nr_rect"})
    {
        BOOST_TEST_MESSAGE("Solver type = " << solverType);
        /*
        auto solver = solverType == "nr_pol" 
            ? std::unique_ptr<PowerFlowSolverInterface>(new PowerFlowNrPolSolver)
            : std::unique_ptr<PowerFlowSolverInterface>(new PowerFlowNrRectSolver);
        */
        auto solver = std::unique_ptr<PowerFlowSolverInterface>(new PowerFlowNrRectSolver);
        nw.setSolver(std::move(solver));
        bool ok = nw.solvePowerFlow();
        BOOST_CHECK(ok == true);

        Complex V0 = bus0->V()(0);
        Complex V1 = bus1->V()(0);
        Complex S1 = bus1->SZip()(0);

        BOOST_CHECK(std::abs(S1 - conj(Ic) * abs(V1)) < 1e-6);

        Complex I1 = -(branch->Y() * Col<Complex>({V0, V1})).eval()(1); // Current entering bus 1 from bus 0.
        BOOST_CHECK(std::abs(Ic * V1 / abs(V1)  - I1) < 1e-6);
    }
}

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

    BOOST_CHECK(g.nodes()[0]->index() == 3);
    BOOST_CHECK(g.nodes()[1]->index() == 0);
    BOOST_CHECK(g.nodes()[2]->index() == 1);
    BOOST_CHECK(g.nodes()[3]->index() == 4);
    BOOST_CHECK(g.nodes()[4]->index() == 5);
    BOOST_CHECK(g.nodes()[5]->index() == 2);
}

BOOST_AUTO_TEST_CASE (test_loops)
{
    Simulation sim;
    Parser<Simulation> p;
    p.parse("test_loops.yaml", sim);
    auto comps = sim.simComponents();
    BOOST_CHECK(comps.size() == 6);
    BOOST_CHECK(comps[0]->id() == "b");
    BOOST_CHECK(comps[1]->id() == "ba");
    BOOST_CHECK(comps[2]->id() == "bb");
    BOOST_CHECK(comps[3]->id() == "c");
    BOOST_CHECK(comps[4]->id() == "ca");
    BOOST_CHECK(comps[5]->id() == "cb");
}

BOOST_AUTO_TEST_CASE (test_properties)
{
    class Foo : public HasProperties
    {
        public:
            SGT_PROPS_INIT(Foo);                                                               

            int four() const {return 4;}
            SGT_PROP_GET(fourProp, four, int);
    };

    class Bar : public Foo
    {
        public:
            SGT_PROPS_INIT(Bar);                                                               
            SGT_PROPS_INHERIT(Foo);          

            const int& x() const {return x_;}
            void setX(const int& x) {x_ = x;}
            SGT_PROP_GET_SET(xProp, x, const int&, setX, const int&);

        public:
            int x_{5};
    };

    Bar bar;
    auto& fourProp = bar.properties()["fourProp"];
    auto& xProp = bar.properties()["xProp"];
    BOOST_CHECK_EQUAL(fourProp.string(bar), "4");
    BOOST_CHECK_EQUAL(fourProp.getAs<int>(bar), 4);
    BOOST_CHECK_EQUAL(xProp.string(bar), "5");
    BOOST_CHECK_EQUAL(xProp.getAs<const int&>(bar), 5);
    xProp.setAs<const int&>(bar, 345);
    BOOST_CHECK_EQUAL(xProp.getAs<const int&>(bar), 345);
    xProp.setFromString(bar, "678");
    BOOST_CHECK_EQUAL(xProp.getAs<const int&>(bar), 678);
};

BOOST_AUTO_TEST_CASE (test_phases_A)
{
    Network netw;
    Parser<Network> p;
    p.parse("test_phases_A.yaml", netw);
    netw.solvePowerFlow();
    auto V0 = netw.buses()[0]->V();
    auto V1 = netw.buses()[1]->V();
    BOOST_CHECK_SMALL(std::abs(V0(0) - V1(1)), 1e-9);
    BOOST_CHECK_SMALL(std::abs(V0(1) - V1(2)), 1e-9);
    BOOST_CHECK_SMALL(std::abs(V0(2) - V1(0)), 1e-9);
}

BOOST_AUTO_TEST_CASE (test_phases_B)
{
    Network netw;
    Parser<Network> p;
    p.parse("test_phases_B.yaml", netw);
    netw.solvePowerFlow();
    auto V0 = netw.buses()[0]->V();
    auto V1 = netw.buses()[1]->V();
    BOOST_CHECK_SMALL(std::abs(V0(0) - V1(2)), 1e-9);
    BOOST_CHECK_SMALL(std::abs(V0(1) - V1(0)), 1e-9);
    BOOST_CHECK_SMALL(std::abs(V0(2) - V1(1)), 1e-9);
}

BOOST_AUTO_TEST_CASE (test_phases_C)
{
    Network netw;
    Parser<Network> p;
    p.parse("test_phases_C.yaml", netw);
    netw.solvePowerFlow();
    auto V0 = netw.buses()[0]->V();
    auto V1 = netw.buses()[1]->V();
    auto S0Gen = netw.buses()[0]->SGen();
    auto S1Zip = netw.buses()[1]->SZip();
    auto y = netw.branches()[0]->Y()(0, 0);
    BOOST_CHECK_SMALL(std::abs(V0(0) - V1(0)), 1e-9);
    BOOST_CHECK_SMALL(std::abs(S0Gen(0)), 1e-9);
    BOOST_CHECK_SMALL(std::abs(S0Gen(1)), 1e-9);
    auto delta = (V0(2) - V1(1));
    auto diff = S0Gen(2) - S1Zip(1, 1) - delta * std::conj(y) * std::conj(delta);
    BOOST_CHECK_SMALL(std::abs(diff), 1e-9);
}

BOOST_AUTO_TEST_CASE (test_vv_transformer)
{
    Network netw;
    Parser<Network> p;
    p.parse("test_vv_transformer.yaml", netw);
    netw.solvePowerFlow();

    auto V0 = netw.buses()[0]->V();
    auto V1 = netw.buses()[1]->V();
    auto V0ab = V0(0) - V0(1);
    auto V0bc = V0(1) - V0(2);
    auto V0ca = V0(2) - V0(0);
    auto V1ab = V1(0) - V1(1);
    auto V1bc = V1(1) - V1(2);
    auto V1ca = V1(2) - V1(0);
    std::cout << std::abs(V0(0)) << " " << std::abs(V0(1)) << " " << std::abs(V0(2)) << std::endl;
    std::cout << std::abs(V1(0)) << " " << std::abs(V1(1)) << " " << std::abs(V1(2)) << std::endl;
    std::cout << std::abs(V0ab) << " " << std::abs(V0bc) << " " << std::abs(V0ca) << std::endl;
    std::cout << std::abs(V1ab) << " " << std::abs(V1bc) << " " << std::abs(V1ca) << std::endl;
    std::cout << std::abs(V0ab/V1ab) << " " << std::abs(V0bc/V1bc) << " " << std::abs(V0ca/V1ca) << std::endl;
    BOOST_CHECK_CLOSE(std::abs(V0ab/V1ab), 0.95, 1e-6);
    BOOST_CHECK_CLOSE(std::abs(V0bc/V1bc), 1.05, 1e-6);
}

BOOST_AUTO_TEST_CASE (test_load_model_a)
{
    Network netw;
    Parser<Network> p;
    p.parse("test_load_model_a.yaml", netw);
    netw.solvePowerFlow();

    Complex y{0.1, -0.1};
    Complex s{0.1, 0.05};

    const auto& bus1 = *netw.buses()["bus_1"];
    const auto& bus2 = *netw.buses()["bus_2"];

    const Col<Complex>& V1 = bus1.V();
    const Col<Complex>& V2 = bus2.V();

    BOOST_CHECK_SMALL(norm(V1 - bus1.VNom()), 1e-9);

    {
        Complex IA = y * (V1(0) - V2(0));
        Complex IAB = conj(s/(V2(0) - V2(1)));
        Complex ICA = conj(s/(V2(2) - V2(0)));
        BOOST_CHECK_SMALL(abs(IA - IAB + ICA), 1e-9);
    }
   
    {
        Complex IB = y * (V1(1) - V2(1));
        Complex IBC = conj(s/(V2(1) - V2(2)));
        Complex IAB = conj(s/(V2(0) - V2(1)));
        BOOST_CHECK_SMALL(abs(IB - IBC + IAB), 1e-9);
    }
   
    {
        Complex IC = y * (V1(2) - V2(2));
        Complex ICA = conj(s/(V2(2) - V2(0)));
        Complex IBC = conj(s/(V2(1) - V2(2)));
        BOOST_CHECK_SMALL(abs(IC - ICA + IBC), 1e-9);
    }
}

BOOST_AUTO_TEST_CASE (test_load_model_b)
{
    Network netw;
    Parser<Network> p;
    p.parse("test_load_model_b.yaml", netw);
    netw.solvePowerFlow();

    Complex y{0.1, -0.1};
    Complex s{0.1, 0.05};

    const auto& bus1 = *netw.buses()["bus_1"];
    const auto& bus2 = *netw.buses()["bus_2"];

    const Col<Complex>& V1 = bus1.V();
    const Col<Complex>& V2 = bus2.V();

    BOOST_CHECK_SMALL(norm(V1 - bus1.VNom()), 1e-9);

    {
        Complex IA = y * (V1(0) - V2(0));
        Complex IAB = conj(s/(V2(0) - V2(1)));
        BOOST_CHECK_SMALL(abs(IA - IAB), 1e-9);
    }
   
    {
        Complex IB = y * (V1(1) - V2(1));
        Complex IAB = conj(s/(V2(0) - V2(1)));
        BOOST_CHECK_SMALL(abs(IB + IAB), 1e-9);
    }
   
    {
        Complex IC = y * (V1(2) - V2(2));
        BOOST_CHECK_SMALL(abs(IC), 1e-9);
    }
}

BOOST_AUTO_TEST_CASE (test_load_model_c)
{
    Network netw;
    Parser<Network> p;
    p.parse("test_load_model_c.yaml", netw);
    netw.solvePowerFlow();

    Complex y{0.1, -0.1};
    Complex i{0.1, 0.05};

    const auto& bus1 = *netw.buses()["bus_1"];
    const auto& bus2 = *netw.buses()["bus_2"];

    const Col<Complex>& V1 = bus1.V();
    const Col<Complex>& V2 = bus2.V();

    BOOST_CHECK_SMALL(norm(V1 - bus1.VNom()), 1e-8);

    {
        Complex IA = y * (V1(0) - V2(0));
        Complex IAB = i * (V2(0) - V2(1)) / abs(V2(0) - V2(1));
        BOOST_CHECK_SMALL(abs(IA - IAB), 1e-8);
    }
   
    {
        Complex IB = y * (V1(1) - V2(1));
        Complex IAB = i * (V2(0) - V2(1)) / abs(V2(0) - V2(1));
        BOOST_CHECK_SMALL(abs(IB + IAB), 1e-8);
    }
   
    {
        Complex IC = y * (V1(2) - V2(2));
        BOOST_CHECK_SMALL(abs(IC), 1e-8);
    }
}

BOOST_AUTO_TEST_CASE (test_include)
{
    Network netw;
    Parser<Network> p;
    p.parse("test_include_A.yaml", netw);
    std::cout << netw << std::endl;
    BOOST_CHECK(netw.buses().size() == 2);
    BOOST_CHECK(netw.branches().size() == 1);
    BOOST_CHECK(netw.gens().size() == 1);
}

BOOST_AUTO_TEST_SUITE_END()
