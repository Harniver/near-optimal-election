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
    struct wave__leaders {};
    struct colr__leaders {};
    struct fwav__leaders {};
    struct fcol__leaders {};

    struct wave__correct {};
    struct colr__correct {};
    struct fwav__correct {};
    struct fcol__correct {};

    struct wave__spurious {};
    struct colr__spurious {};
    struct fwav__spurious {};
    struct fcol__spurious {};
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

    device_t wave = wave_election(CALL);
    device_t colr = color_election(CALL);
    device_t fwav = stabiliser(CALL, wave, 4);
    device_t fcol = stabiliser(CALL, colr, 4);

    node.storage(tags::wave__leaders{}) = wave;
    node.storage(tags::colr__leaders{}) = colr;
    node.storage(tags::fwav__leaders{}) = fwav;
    node.storage(tags::fcol__leaders{}) = fcol;

    node.storage(tags::wave__correct{}) = wave == perturbation;
    node.storage(tags::colr__correct{}) = colr == perturbation;
    node.storage(tags::fwav__correct{}) = fwav == perturbation;
    node.storage(tags::fcol__correct{}) = fcol == perturbation;

    node.storage(tags::wave__spurious{}) = wave > perturbation;
    node.storage(tags::colr__spurious{}) = colr > perturbation;
    node.storage(tags::fwav__spurious{}) = fwav > perturbation;
    node.storage(tags::fcol__spurious{}) = fcol > perturbation;
}


}


//! @brief Main struct calling `collection_compare`.
MAIN(coordination::election_compare,);


}

#endif // FCPP_ELECTION_COMPARE_H_
