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
    struct done__leaders {};

    struct diam__filtered {};
    struct wave__filtered {};
    struct wav2__filtered {};
    struct wav3__filtered {};
    struct colr__filtered {};
    struct done__filtered {};

    struct diam__correct {};
    struct wave__correct {};
    struct wav2__correct {};
    struct wav3__correct {};
    struct colr__correct {};
    struct done__correct {};
    //! @}
}


//! @brief Finds the minimum value, without any additional knowledge, through alternating colors.
template <typename node_t, typename T>
T color_election_done(node_t& node, trace_t call_point, const T& value) {
    internal::trace_call trace_caller(node.stack_trace, call_point);

    using key_type = tuple<bool,T,int,device_t,bool>;
    constexpr size_t disable = 0;
    constexpr size_t leader  = 1;
    constexpr size_t level   = 2;
    constexpr size_t parent  = 3;
    constexpr size_t done    = 4;

    key_type self_key{false, value, 0, node.uid, false};
    return get<leader>(nbr(node, 0, self_key, [&](field<key_type> nbr_keys) -> key_type {
        key_type const& old_key = self(node, 0, nbr_keys);
        key_type const& parent_key = self(node, 0, nbr_keys, get<parent>(old_key));
        key_type const* pbk = &self_key;
        device_t best_nbr = fold_hood(node, 0, [&](device_t curr, key_type const& key, device_t best){
            if (key < *pbk) {
                pbk = &key;
                return curr;
            } else return best;
        }, nbr_keys, node.uid);
        key_type const& best_nbr_key = *pbk;
        auto same_key_check = [&](key_type const& k, key_type const& h) {
            return get<leader>(k) == get<leader>(h) and get<level>(k) == get<level>(h);
        };
        auto succ_key_check = [&](key_type const& succ, key_type const& prev) {
            return get<leader>(succ) == get<leader>(prev) and get<level>(succ) == get<level>(prev)+1;
        };
        auto better_key_check = [&](key_type const& low, key_type const& hi) {
            return get<leader>(low) < get<leader>(hi) or (get<leader>(low) == get<leader>(hi) and get<level>(low)+1 < get<level>(hi));
        };
        auto true_child_check = [&](key_type const& child) {
            return get<parent>(child) == node.uid and succ_key_check(child, old_key);
        };
        auto false_child_check = [&](key_type const& child) {
            return get<parent>(child) == node.uid and not succ_key_check(child, old_key);
        };
        bool is_true_root = same_key_check(old_key, self_key);
        bool is_true_child = get<leader>(old_key) < value and succ_key_check(old_key, parent_key);
        bool is_false_root = not (is_true_root or is_true_child);
        bool has_false_child = fold_hood(node, 0, [&](key_type const& key, bool holds){
            return holds or false_child_check(key);
        }, nbr_keys, false);
        bool best_nbr_improves = better_key_check(best_nbr_key, old_key);
        if (best_nbr != node.uid and (is_false_root or best_nbr_improves) and (not has_false_child))
            return {true, get<leader>(best_nbr_key), get<level>(best_nbr_key) + 1, best_nbr, false};
        if (is_false_root)
            return self_key;
        bool has_recruit = fold_hood(node, 0, [&](key_type const& key, bool holds){
            return holds or better_key_check(old_key, key);
        }, nbr_keys, false);
        bool has_similar_child = fold_hood(node, 0, [&](key_type const& key, bool holds){
            return holds or (true_child_check(key) and get<disable>(key) == get<disable>(old_key));
        }, nbr_keys, false);
        bool similar_parent = get<disable>(parent_key) == get<disable>(old_key);
        bool all_children_done = fold_hood(node, 0, [&](key_type const& key, bool holds){
            return holds and (get<done>(key) or not true_child_check(key));
        }, nbr_keys, true);
        key_type new_key = old_key;
        get<done>(new_key) = all_children_done and not has_recruit;
        bool color_frozen = is_true_root and get<done>(new_key);
        if (similar_parent and (get<disable>(old_key) or not has_recruit) and (not has_similar_child) and not color_frozen) {
            get<disable>(new_key) = not get<disable>(new_key);
        }
        return new_key;
    }));
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

    device_t diam = diameter_election(CALL, L+2);
    device_t wave = wave_election(CALL);
    device_t wav2 = wave_election(CALL, node.uid, [](int x){ return 2*x+1; });
    device_t wav3 = wave_election(CALL, node.uid, [](int x){ return 3*x+1; });
    device_t colr = color_election(CALL);
    device_t done = color_election_done(CALL, node.uid);

    node.storage(tags::diam__leaders{}) = diam;
    node.storage(tags::wave__leaders{}) = wave;
    node.storage(tags::wav2__leaders{}) = wav2;
    node.storage(tags::wav3__leaders{}) = wav3;
    node.storage(tags::colr__leaders{}) = colr;
    node.storage(tags::done__leaders{}) = done;

    node.storage(tags::diam__correct{}) = diam == perturbation;
    node.storage(tags::wave__correct{}) = wave == perturbation;
    node.storage(tags::wav2__correct{}) = wav2 == perturbation;
    node.storage(tags::wav3__correct{}) = wav3 == perturbation;
    node.storage(tags::colr__correct{}) = colr == perturbation;
    node.storage(tags::done__correct{}) = done == perturbation;

    node.storage(tags::diam__filtered{}) = stabiliser(CALL, diam, 4);
    node.storage(tags::wave__filtered{}) = stabiliser(CALL, wave, 4);
    node.storage(tags::wav2__filtered{}) = stabiliser(CALL, wav2, 4);
    node.storage(tags::wav3__filtered{}) = stabiliser(CALL, wav3, 4);
    node.storage(tags::colr__filtered{}) = stabiliser(CALL, colr, 4);
    node.storage(tags::done__filtered{}) = stabiliser(CALL, done, 4);
}


}


//! @brief Main struct calling `collection_compare`.
MAIN(coordination::election_compare,);


}

#endif // FCPP_ELECTION_COMPARE_H_
