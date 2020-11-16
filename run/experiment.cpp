// Copyright © 2020 Giorgio Audrito. All Rights Reserved.

#include "lib/fcpp.hpp"

#include "fcpp/election_compare.hpp"

using namespace fcpp;
using namespace common::tags;
using namespace component::tags;
using namespace coordination::tags;

constexpr int runs = 1000;

struct sync {};      // whether it is synchronous           = true, false
struct dens {};      // average density                     = 10, 20, 30
//     area          // number of hops                      = 10, 20, 40
//     speed         // maximum movement speed              = 0, 0.25, 0.5

struct round_dev {}; // standard deviation in round length  = sync ? 0 : 0.25
struct dev_num {};   // total number of devices             = dens*area*2/π
struct end_time {};  // time for end simulation             = 10*area
//     die_time      // time for disruption                 = 5*area

                     // total complexity of simulation      = (2*dens*area)^2

using d0 = distribution::constant_n<times_t, 0>;
using d1 = distribution::constant_n<times_t, 1>;
using d2 = distribution::constant_n<times_t, 2>;

template <bool is_sync>
using spawn_s = sequence::multiple<
    distribution::constant_i<size_t, dev_num>,
    distribution::interval_n<times_t, 0, is_sync ? 0 : 1>,
    is_sync
>;

using round_s = sequence::periodic<
    distribution::weibull<d1, d0, void, round_dev>,
    distribution::weibull<d1, d0, void, round_dev>,
    distribution::constant_i<times_t, end_time>
>;

using export_s = sequence::periodic<d0, d1, distribution::constant_i<times_t, end_time>>;

using rectangle_d = distribution::rect<d0, d0, distribution::constant_i<double, area>, d2>;

using aggregator_t = aggregators<
        leaders<wave>,      aggregator::distinct<device_t>,
        leaders<colr>,      aggregator::distinct<device_t>,
        leaders<fwav>,      aggregator::distinct<device_t>,
        leaders<fcol>,      aggregator::distinct<device_t>,

        correct<wave>,      aggregator::sum<int>,
        correct<colr>,      aggregator::sum<int>,
        correct<fwav>,      aggregator::sum<int>,
        correct<fcol>,      aggregator::sum<int>,

        spurious<wave>,     aggregator::sum<int>,
        spurious<colr>,     aggregator::sum<int>,
        spurious<fwav>,     aggregator::sum<int>,
        spurious<fcol>,     aggregator::sum<int>
    >;

template <typename xvar, template<class> class yvar, typename bucket, typename aggr>
using plot_t = plot::split<xvar, plot::values<aggregator_t, common::type_sequence<aggr>, plot::unit<yvar>>, bucket>;
template <typename xvar, typename bucket, typename aggr>
using plot_row_t = plot::join<plot_t<xvar, leaders, bucket, aggr>, plot_t<xvar, correct, bucket, aggr>, plot_t<xvar, spurious, bucket, aggr>>;
template <typename xvar, typename fvar, typename bucket = std::ratio<0>, typename aggr = aggregator::mean<double>>
using plot_page_t = plot::filter<fvar, filter::equal<0>, plot::split<sync, plot_row_t<xvar, bucket, aggr>>>;
using plotter_t = plot::join<plot_page_t<plot::time, speed>, plot_page_t<plot::time, speed, std::ratio<5>>, plot::filter<plot::time, filter::above<100>, plot_page_t<speed, sync>>, plot_page_t<speed, sync>>;


template <bool is_sync>
DECLARE_OPTIONS(opt,
    synchronised<is_sync>,
    parallel<false>,
    program<main>,
    retain<metric::retain<2>>,
    round_schedule<round_s>,
    exports<
        tuple<device_t, device_t, int>, vec<2>,
        tuple<device_t, int>, tuple<device_t, int, int, int>,
        tuple<bool,device_t,int,device_t>, tuple<bool,device_t,int,device_t,bool>
    >,
    log_schedule<export_s>,
    aggregator_t,
    tuple_store<
        area,               double,
        die_time,           times_t,
        speed,              double,

        leaders<wave>,      device_t,
        leaders<colr>,      device_t,
        leaders<fwav>,      device_t,
        leaders<fcol>,      device_t,

        correct<wave>,      int,
        correct<colr>,      int,
        correct<fwav>,      int,
        correct<fcol>,      int,

        spurious<wave>,     int,
        spurious<colr>,     int,
        spurious<fwav>,     int,
        spurious<fcol>,     int
    >,
    extra_info<sync, int, speed, double>,
    plot_type<plotter_t>,
    spawn_schedule<spawn_s<is_sync>>,
    init<
        x,          rectangle_d,
        area,       distribution::constant_i<double, area>,
        speed,      distribution::constant_i<double, speed>,
        round_dev,  distribution::constant_i<double, round_dev>,
        die_time,   distribution::constant_i<times_t, die_time>,
        end_time,   distribution::constant_i<times_t, end_time>
    >,
    connector<connect::fixed<>>
);

plotter_t P;

auto make_parameters(bool is_sync, int runs, std::string var = "none") {
    return batch::make_tagged_tuple_sequence(
        batch::arithmetic<seed>(0, runs-1, 1),
        batch::constant<sync>(is_sync),
        batch::arithmetic<speed>(0.05 * (var == "speed"), 1.001 * (var == "speed"), 0.05),
        batch::arithmetic<dens>(10 + 10 * (var != "dens"), 40, 30),
        batch::arithmetic<area>(10 + 10 * (var != "area"), 40, 30),
        batch::stringify<output>("output/raw/experiment", "txt"),
        batch::constant<plotter>(&P),
        batch::formula<round_dev>([&](auto const& t){ return is_sync ? 0 : 0.25; }),
        batch::formula<dev_num  >([ ](auto const& t){ return (common::get<dens>(t)*common::get<area>(t)*200)/314; }),
        batch::formula<end_time >([ ](auto const& t){ return common::get<area>(t)*10; }),
        batch::formula<die_time >([ ](auto const& t){ return common::get<area>(t)*5; })
    );
}

int main() {
    batch::run(component::batch_simulator<opt<true>>{},
               make_parameters(true, runs*5));
    batch::run(component::batch_simulator<opt<false>>{},
               make_parameters(false, runs*5),
               make_parameters(false, runs, "speed"));
    std::cout << plot::file("experiment", P.build());
    return 0;
}
