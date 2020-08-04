// Copyright © 2020 Giorgio Audrito. All Rights Reserved.

#include "lib/fcpp.hpp"

#include "fcpp/election_compare.hpp"

using namespace fcpp;
using namespace common::tags;
using namespace component::tags;
using namespace coordination::tags;

constexpr int runs = 100;

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

template <bool is_sync>
DECLARE_OPTIONS(opt,
    synchronised<is_sync>,
    parallel<false>,
    program<main>,
    round_schedule<round_s>,
    exports<
        tuple<device_t, device_t, int>, vec<2>,
        tuple<device_t, int>, tuple<device_t, int, int, int>,
        tuple<bool,device_t,int,device_t>, tuple<bool,device_t,int,device_t,bool>
    >,
    log_schedule<export_s>,
    aggregators<
        diam__leaders,      aggregator::distinct<device_t>,
        wave__leaders,      aggregator::distinct<device_t>,
        wav2__leaders,      aggregator::distinct<device_t>,
        wav3__leaders,      aggregator::distinct<device_t>,
        colr__leaders,      aggregator::distinct<device_t>,
        done__leaders,      aggregator::distinct<device_t>,

        diam__filtered,     aggregator::distinct<device_t>,
        wave__filtered,     aggregator::distinct<device_t>,
        wav2__filtered,     aggregator::distinct<device_t>,
        wav3__filtered,     aggregator::distinct<device_t>,
        colr__filtered,     aggregator::distinct<device_t>,
        done__filtered,     aggregator::distinct<device_t>,

        diam__correct,      aggregator::sum<int>,
        wave__correct,      aggregator::sum<int>,
        wav2__correct,      aggregator::sum<int>,
        wav3__correct,      aggregator::sum<int>,
        colr__correct,      aggregator::sum<int>,
        done__correct,      aggregator::sum<int>
    >,
    tuple_store<
        area,               double,
        die_time,           times_t,
        speed,              double,

        diam__leaders,      device_t,
        wave__leaders,      device_t,
        wav2__leaders,      device_t,
        wav3__leaders,      device_t,
        colr__leaders,      device_t,
        done__leaders,      device_t,

        diam__filtered,     device_t,
        wave__filtered,     device_t,
        wav2__filtered,     device_t,
        wav3__filtered,     device_t,
        colr__filtered,     device_t,
        done__filtered,     device_t,

        diam__correct,      int,
        wave__correct,      int,
        wav2__correct,      int,
        wav3__correct,      int,
        colr__correct,      int,
        done__correct,      int
    >,
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

auto make_parameters(bool is_sync, std::string var = "none") {
    return batch::make_tagged_tuple_sequence(
        batch::arithmetic<seed>(0, runs-1, 1),
        batch::constant<sync>(is_sync),
        batch::arithmetic<speed>(0.25 * (var == "speed"), 0.5 * (var == "speed"), 0.25),
        batch::arithmetic<dens>(10 + 10 * (var != "dens"), 40, 30),
        batch::arithmetic<area>(10 + 10 * (var != "area"), 40, 30),
        batch::stringify<output>("output/raw/experiment", "txt"),
        batch::formula<round_dev>([&](auto const& t){ return is_sync ? 0 : 0.25; }),
        batch::formula<dev_num  >([ ](auto const& t){ return (common::get<dens>(t)*common::get<area>(t)*200)/314; }),
        batch::formula<end_time >([ ](auto const& t){ return common::get<area>(t)*10; }),
        batch::formula<die_time >([ ](auto const& t){ return common::get<area>(t)*5; })
    );
}

template <bool is_sync>
void runner() {
    batch::run(component::batch_simulator<opt<is_sync>>{},
               make_parameters(is_sync),
               make_parameters(is_sync, "speed"),
               make_parameters(is_sync, "dens"),
               make_parameters(is_sync, "area"));
}

int main() {
    runner<true>();
    runner<false>();
    return 0;
}
