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

    //! @brief The movement speed of devices.
    struct speed {};

    //! @brief Output values.
    //! @{
    struct diam_s1 {};
    struct wave_s1 {};
    struct wav2_s1 {};
    struct wav3_s1 {};
    struct colr_s1 {};

    struct diam_s2 {};
    struct wave_s2 {};
    struct wav2_s2 {};
    struct wav3_s2 {};
    struct colr_s2 {};

    struct diam_s4 {};
    struct wave_s4 {};
    struct wav2_s4 {};
    struct wav3_s4 {};
    struct colr_s4 {};

    struct diam_s8 {};
    struct wave_s8 {};
    struct wav2_s8 {};
    struct wav3_s8 {};
    struct colr_s8 {};
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
    rectangle_walk(CALL, make_vec(0,0), make_vec(L,L), node.storage(tags::speed{}), 1);
    
    device_t diam = diameter_election(CALL, L*0.015);
    device_t wave = wave_election(CALL);
    device_t wav2 = wave_election(CALL, node.uid, [](int x){ return 2*x+1; });
    device_t wav3 = wave_election(CALL, node.uid, [](int x){ return 3*x+1; });
    device_t colr = color_election(CALL);
    
    node.storage(tags::diam_s1{}) = diam;
    node.storage(tags::wave_s1{}) = wave;
    node.storage(tags::wav2_s1{}) = wav2;
    node.storage(tags::wav3_s1{}) = wav3;
    node.storage(tags::colr_s1{}) = colr;
    
    node.storage(tags::diam_s2{}) = stabiliser(CALL, diam, 2);
    node.storage(tags::wave_s2{}) = stabiliser(CALL, wave, 2);
    node.storage(tags::wav2_s2{}) = stabiliser(CALL, wav2, 2);
    node.storage(tags::wav3_s2{}) = stabiliser(CALL, wav3, 2);
    node.storage(tags::colr_s2{}) = stabiliser(CALL, colr, 2);
    
    node.storage(tags::diam_s4{}) = stabiliser(CALL, diam, 4);
    node.storage(tags::wave_s4{}) = stabiliser(CALL, wave, 4);
    node.storage(tags::wav2_s4{}) = stabiliser(CALL, wav2, 4);
    node.storage(tags::wav3_s4{}) = stabiliser(CALL, wav3, 4);
    node.storage(tags::colr_s4{}) = stabiliser(CALL, colr, 4);
    
    node.storage(tags::diam_s8{}) = stabiliser(CALL, diam, 8);
    node.storage(tags::wave_s8{}) = stabiliser(CALL, wave, 8);
    node.storage(tags::wav2_s8{}) = stabiliser(CALL, wav2, 8);
    node.storage(tags::wav3_s8{}) = stabiliser(CALL, wav3, 8);
    node.storage(tags::colr_s8{}) = stabiliser(CALL, colr, 8);
}


}


//! @brief Main struct calling `collection_compare`.
MAIN(coordination::election_compare,);


}

#endif // FCPP_ELECTION_COMPARE_H_
