// Copyright © 2020 Giorgio Audrito. All Rights Reserved.

#include "lib/fcpp.hpp"

#include "fcpp/election_compare.hpp"

using namespace fcpp;
using namespace component::tags;
using namespace coordination::tags;

#define DEVICE_NUM  1000
#define AREA        1000
#define SPEED       0
#define END_TIME    200
#define DIE_TIME    100

using spawn_s = sequence::multiple<distribution::constant<times_t, 0>, DEVICE_NUM>;

using round_s = sequence::periodic<distribution::interval_t<times_t, 0, 1>, distribution::weibull_t<times_t, 100, 25, 100>, distribution::constant<times_t, END_TIME>>;

using export_s = sequence::periodic<distribution::constant<times_t, 0>, distribution::constant<times_t, 1>, distribution::constant<times_t, END_TIME>>;

using rectangle_d = distribution::vec<distribution::interval_t<double, 0, AREA>, distribution::interval_t<double, 0, AREA>>;

DECLARE_OPTIONS(opt,
    program<main>,
    round_schedule<round_s>,
    exports<
        tuple<device_t, device_t, int>, vec<2>,
        tuple<device_t, int>, tuple<device_t, int, int, int>, tuple<bool,device_t,int,device_t>
    >,
    log_schedule<export_s>,
    aggregators<
        diam_s1,    aggregator::distinct<device_t>,
        wave_s1,    aggregator::distinct<device_t>,
        wav2_s1,    aggregator::distinct<device_t>,
        wav3_s1,    aggregator::distinct<device_t>,
        colr_s1,    aggregator::distinct<device_t>,

        diam_s2,    aggregator::distinct<device_t>,
        wave_s2,    aggregator::distinct<device_t>,
        wav2_s2,    aggregator::distinct<device_t>,
        wav3_s2,    aggregator::distinct<device_t>,
        colr_s2,    aggregator::distinct<device_t>,

        diam_s4,    aggregator::distinct<device_t>,
        wave_s4,    aggregator::distinct<device_t>,
        wav2_s4,    aggregator::distinct<device_t>,
        wav3_s4,    aggregator::distinct<device_t>,
        colr_s4,    aggregator::distinct<device_t>,

        diam_s8,    aggregator::distinct<device_t>,
        wave_s8,    aggregator::distinct<device_t>,
        wav2_s8,    aggregator::distinct<device_t>,
        wav3_s8,    aggregator::distinct<device_t>,
        colr_s8,    aggregator::distinct<device_t>
    >,
    tuple_store<
        area,       double,
        die,        times_t,
        speed,      double,

        diam_s1,    device_t,
        wave_s1,    device_t,
        wav2_s1,    device_t,
        wav3_s1,    device_t,
        colr_s1,    device_t,

        diam_s2,    device_t,
        wave_s2,    device_t,
        wav2_s2,    device_t,
        wav3_s2,    device_t,
        colr_s2,    device_t,

        diam_s4,    device_t,
        wave_s4,    device_t,
        wav2_s4,    device_t,
        wav3_s4,    device_t,
        colr_s4,    device_t,

        diam_s8,    device_t,
        wave_s8,    device_t,
        wav2_s8,    device_t,
        wav3_s8,    device_t,
        colr_s8,    device_t
    >,
    spawn_schedule<spawn_s>,
    init<
        x,          rectangle_d,
        area,       distribution::constant<double, AREA>,
        die,        distribution::constant<times_t, DIE_TIME>,
        speed,      distribution::constant<double, SPEED>
    >,
    connector<connect::fixed<100>>
);

int main() {
    batch::run(component::batch_simulator<opt>{});
    return 0;
}
