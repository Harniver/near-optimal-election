// Copyright © 2020 Giorgio Audrito. All Rights Reserved.

/**
 * @file election_compare.hpp
 * @brief Implementation of the case study comparing election algorithms.
 */

#ifndef FCPP_ELECTION_COMPARE_H_
#define FCPP_ELECTION_COMPARE_H_

#include "lib/beautify.hpp"
#include "lib/coordination/election.hpp"
#include "lib/coordination/geometry.hpp"


/**
 * @brief Namespace containing all the objects in the FCPP library.
 */
namespace fcpp {


//! @brief Namespace containing the libraries of coordination routines.
namespace coordination {


namespace tags {
    //! @brief The size of the area where devices are located.
    struct area {};

    //! @brief The time when node 0 should remove itself.
    struct die_time {};

    //! @brief The movement speed of devices.
    struct speed {};

    //! @brief Output values.
    //! @{
    struct diam__leaders {};
    struct wave__leaders {};
    struct wav2__leaders {};
    struct wav3__leaders {};
    struct colr__leaders {};

    struct diam__filtered {};
    struct wave__filtered {};
    struct wav2__filtered {};
    struct wav3__filtered {};
    struct colr__filtered {};

    struct diam__correct {};
    struct wave__correct {};
    struct wav2__correct {};
    struct wav3__correct {};
    struct colr__correct {};
    //! @}
}


//! @brief Stabilise a value, accepting changes only after a number of rounds with the same value given by the delay.
FUN(T) T stabiliser(ARGS, T value, int delay) { CODE
    return get<0>(old(CALL, make_tuple(value,value,0), [&](tuple<T,T,int> o) {
        if (value == get<1>(o)) ++get<2>(o);
        else get<2>(o) = 1;
        get<1>(o) = value;
        if (get<2>(o) > delay) get<0>(o) = value;
        return o;
    }));
}

//! @brief Computes several election algorithms for comparing them.
FUN() void election_compare(ARGS) { CODE
    double L = node.storage(tags::area{});
    rectangle_walk(CALL, make_vec(0,0), make_vec(L,2), node.storage(tags::speed{}), 1);
    bool perturbation = node.current_time() >= node.storage(tags::die_time{});
    if (node.uid == 0 and perturbation) node.terminate();

//    device_t diam = diameter_election(CALL, L+2);
    device_t wave = wave_election(CALL);
    device_t wav2 = wave_election(CALL, node.uid, [](int x){ return 2*x+1; });
    device_t wav3 = wave_election(CALL, node.uid, [](int x){ return 3*x+1; });
    device_t colr = color_election(CALL);

//    node.storage(tags::diam__leaders{}) = diam;
    node.storage(tags::wave__leaders{}) = wave;
    node.storage(tags::wav2__leaders{}) = wav2;
    node.storage(tags::wav3__leaders{}) = wav3;
    node.storage(tags::colr__leaders{}) = colr;

//    node.storage(tags::diam__correct{}) = diam == perturbation;
    node.storage(tags::wave__correct{}) = wave == perturbation;
    node.storage(tags::wav2__correct{}) = wav2 == perturbation;
    node.storage(tags::wav3__correct{}) = wav3 == perturbation;
    node.storage(tags::colr__correct{}) = colr == perturbation;

//    node.storage(tags::diam__filtered{}) = stabiliser(CALL, diam, 4);
    node.storage(tags::wave__filtered{}) = stabiliser(CALL, wave, 4);
    node.storage(tags::wav2__filtered{}) = stabiliser(CALL, wav2, 4);
    node.storage(tags::wav3__filtered{}) = stabiliser(CALL, wav3, 4);
    node.storage(tags::colr__filtered{}) = stabiliser(CALL, colr, 4);
}


}


//! @brief Main struct calling `collection_compare`.
MAIN(coordination::election_compare,);


}

#endif // FCPP_ELECTION_COMPARE_H_
