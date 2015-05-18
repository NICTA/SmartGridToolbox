#define BOOST_TEST_MODULE test_template

#include "../SgtCore/SgtCore.h"
#include "../SgtSim/SgtSim.h"
#include "../SgtSim/WeakOrder.h"

#include <boost/test/included/unit_test.hpp>

#include <cmath>
#include <ostream>
#include <fstream>
#include <strstream>

using namespace Sgt;
using namespace std;
using namespace boost::posix_time;
using namespace boost::unit_test;

struct Init
{
    Init()
    {
        unit_test_log.set_threshold_level(boost::unit_test::log_messages);
        BOOST_MESSAGE("\nTesting " << framework::current_test_case().p_name);
    }
    ~Init()
    {
        BOOST_MESSAGE("Finished " << framework::current_test_case().p_name << "\n");
    }
};

BOOST_FIXTURE_TEST_SUITE(tests, Init)

BOOST_AUTO_TEST_CASE (test_overhead_compare_carson_1)
{
    // For this test, we compare values for an overhead line examined in one of Kersting's papers:
    // docs/background/Kersting_Carson.pdf.
    Network netw("network");
    Parser<Network> p;
    p.parse("test_overhead_compare_carson_1.yaml", netw);

    auto oh = std::dynamic_pointer_cast<OverheadLine>(netw.branch("line_1_2"));

    arma::Mat<Complex> ZPrim = oh->ZPrim();
    arma::Mat<Complex> ZPhase = oh->ZPhase();

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
    Network netw("network");
    Parser<Network> p;
    p.parse("test_overhead_compare_carson_2.yaml", netw);

    netw.solvePowerFlow();

    auto oh = std::dynamic_pointer_cast<OverheadLine>(netw.branch("line_1_2"));

    auto bus1 = netw.bus("bus_1");
    auto bus2 = netw.bus("bus_2");

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

BOOST_AUTO_TEST_CASE (test_underground_compare_carson)
{
    // For this test, we compare values for an underground line examined in one of Kersting's papers:
    // docs/background/Kersting_Carson.pdf.
    Network netw("network");
    Parser<Network> p;
    p.parse("test_underground_compare_carson.yaml", netw);

    netw.solvePowerFlow();

    auto ug = std::dynamic_pointer_cast<UndergroundLine>(netw.branch("line_1_2"));
    arma::Mat<Complex> ZPhase = ug->ZPhase() * 1609.344; // Convert to ohms per mile.
    arma::Mat<Complex> ZPhaseKersting;
    ZPhaseKersting << Complex(0.7981, 0.4463) << Complex(0.3191, 0.0328) << Complex(0.2849, -0.0143) << arma::endr
                   << Complex(0.3191, 0.0328) << Complex(0.7891, 0.4041) << Complex(0.3191, 0.0328) << arma::endr
                   << Complex(0.2849, -0.0143) << Complex(0.3191, 0.0328) << Complex(0.7981, 0.4463) << arma::endr;
    std::cout << ZPhase << std::endl;
    std::cout << ZPhaseKersting << std::endl;
    double err = arma::norm(ZPhase - ZPhaseKersting, "inf");
    std::cout << "err = " << err << std::endl;
    BOOST_CHECK(err < 0.0005);
}

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
    BOOST_MESSAGE("Starting matpower tests");
    using namespace Sgt;

    std::vector<std::string> cases =
    {
        "case118.m"
        "case14.m"
        "case14_shift.m"
        "case2383wp.m"
        "case24_ieee_rts.m"
        "case2736sp.m"
        "case2737sop.m"
        "case2746wop.m"
        "case2746wp.m
        "case30.m"
        "case300.m"
        "case3012wp.m"
        "case30Q.m"
        "case30_all.m"
        "case30pwl.m"
        "case3120sp.m"
        "case3375wp.m"
        "case39.m"
        "case4gs.m"
        "case5.m"
        "case57.m"
        "case6ww.m"
        "case9.m"
        "case9Q.m"
        "case9target.m"
        "caseSLPQ.m"
        "caseSLPQPV.m"
        "caseSLPV.m"
        "case_ieee30.m"
        "nesta_case118_ieee.m"
        "nesta_case1394sop_eir.m"
        "nesta_case1397sp_eir.m"
        "nesta_case1460wp_eir.m"
        "nesta_case14_ieee.m"
        "nesta_case162_ieee_dtc.m"
        "nesta_case189_edin.m"
        "nesta_case2224_edin.m"
        "nesta_case2383wp_mp.m"
        "nesta_case24_ieee_rts.m"
        "nesta_case2736sp_mp.m"
        "nesta_case2737sop_mp.m"
        "nesta_case2746wop_mp.m"
        "nesta_case2746wp_mp.m"
        "nesta_case29_edin.m"
        "nesta_case300_ieee.m"
        "nesta_case3012wp_mp.m"
        "nesta_case30_as.m"
        "nesta_case30_fsr.m"
        "nesta_case30_ieee.m"
        "nesta_case3120sp_mp.m"
        "nesta_case3375wp_mp.m"
        "nesta_case39_epri.m"
        "nesta_case3_lmbd.m"
        "nesta_case4_gs.m"
        "nesta_case57_ieee.m"
        "nesta_case5_pjm.m"
        "nesta_case6_c.m"
        "nesta_case6_ww.m"
        "nesta_case73_ieee_rts.m"
        "nesta_case9241_pegase.m"
        "nesta_case9_wscc.m"





        "caseSLPQ",
        "caseSLPQPV",
        "caseSLPV",
        "case4gs",
        "case5",
        "case6ww",
        "case9",
        "case9Q",
        "case9target",
        "case14",
        "case14_shift",
        "case24_ieee_rts",
        "case30",
        "case30Q",
        "case30_all",
        "case30pwl",
        "case_ieee30",
        "case39",
        "case57",
        "case118",
        "case300",
        "case2383wp",
        "case2736sp",
        "case2737sop",
        "case2746wop",
        "case2746wp",
        // "case3012wp", // TODO This fails, probably because SGT doesn't enforce reactive limits. See e.g. bus 70.
        "case3120sp",
        // "case3375wp", // Can't solve correctly, but then neither can Matpower...
        "nesta_case9241_pegase"
    };

    for (auto c : cases)
    {
        BOOST_MESSAGE("Case " << c);

        std::string yamlStr =
            std::string("--- [{matpower : {input_file : matpower_test_cases/") + c + ".m, default_kV_base : 11}}]";
        Network nw("network", 100.0);
        YAML::Node n = YAML::Load(yamlStr);
        Sgt::Parser<Network> p;
        p.parse(n, nw);
        nw.solvePowerFlow();

        ifstream compareName(std::string("mp_compare/") + c + ".compare");

        for (auto bus : nw.busses())
        {
            double Vr, Vi, P, Q;
            compareName >> Vr >> Vi >> P >> Q;
            Complex V = {Vr, Vi};
            Complex S = {P, Q};
            BOOST_ASSERT(!compareName.eof());
            BOOST_CHECK_MESSAGE(std::abs(V - bus->V()(0) / bus->VBase()) < 1e-3,
                                "V doesn't agree with Matpower at " << bus->type() << " bus " << bus->id() << "\n"
                                << "    " << bus->V()(0) / bus->VBase() << " : " << V);
            BOOST_CHECK_MESSAGE(std::abs(S - bus->SGen()(0) - bus->SConst()(0)) / nw.PBase() < 1e-3,
                                "S doesn't agree with Matpower at " << bus->type() << " bus " << bus->id() << "\n"
                                << "    " << (bus->SGen()(0) + bus->SConst()(0)) << " : " << S);
        }
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

BOOST_AUTO_TEST_SUITE_END()
