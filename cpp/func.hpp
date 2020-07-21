// Copyright © 2020 Giorgio Audrito. All Rights Reserved.

/**
 * @file func.hpp
 * @brief Implementation of the func type representing a function guiding leader election.
 */

#ifndef CPP_FUNC_H_
#define CPP_FUNC_H_

#include <algorithm>
#include <cassert>
#include <ostream>
#include <vector>

#include "frac.hpp"
#include "max_deque.hpp"
#include "sq2.hpp"


//! @brief √2 approximated.
constexpr double SS    = 1.414213562373095;

//! @brief √2 exact.
const     sq2    S(0,1);

//! @brief Function guiding leader election.
class func {
  public:
    //! @brief fills the function until error or success
    func(frac mk) : MK(mk) {
        int is = 0;
        for (int x=0; ; ++x) {
            cs.push_back(nextconv(is));
            int y = maxallowed(x);
            if (not emplace(x, y)) break;
            assert(is < (int)xs.size());
            deltas.push_back(x+1 - (S-1)*(y+1));
            // xs[is] = g^-1(x)
            while (ys[is] < x+1) {
                for (int i=xs[is]+1; i<=xs[is+1]; ++i) deltas.pop_front();
                ++is;
            }
            // xs[is] = g^-1(x+1)
            sq2 d = std::max(xs[is] - (S-1)*(x+1), deltas.top());
            if (x + 1 > xlimit(double(d))) {
                // generation ends with success
                alpha = (1-d) * (S+1);
                break;
            }
        }
    }
    
    //! @brief direct application of function
    int dir(int x) const {
        if (x > xs.back()) return double((1+S)*x + alpha);
        int i = upper_bound(xs.begin(), xs.end(), x) - xs.begin();
        return ys[i-1];
    }
    
    //! @brief inverse application of function
    int inv(int y) const {
        if (y > ys.back()) return std::max((int)ceil(double((y-alpha)*(S-1))), xs.back()+1);
        int i = lower_bound(ys.begin(), ys.end(), y) - ys.begin();
        return xs[i];
    }

    //! @brief pure stabilisation time (from clean starting configuration)
    int convergence(int x) const {
        if (x < (int)cs.size()) return cs[x];
        if (x == 0) return 1;
        int z = inv(x);
        return convergence(z) + z + x + 1;
    }

    //! @brief recovery time after leader change
    int recovery(int x) const {
        return convergence(x) + dir(x);
    }
    
    //! @brief ideal recovery time after leader change
    inline int ideal(int x) const {
        return 2*x + 1;
    }
    
    //! @brief actual competitiveness achieved
    frac competitiveness() const {
        return K;
    }
    
    //! @brief offset for asymptotic behaviour
    sq2 offset() const {
        return alpha;
    }
    
    //! @brief number of items manually defined
    size_t size() const {
        return xs.back()+1;
    }
    
  private:
    //! @brief inserts a pair for which func(x) = y (possibly updating backwards to ensure monotonicity)
    bool emplace(int x, int y) {
        if (x >= y) {
            y = x+1;
            K = std::max(K, frac(cs[x] + y, ideal(x)));
            assert(K >= MK);
            return false; // generation ends with failure
        }
        frac nK(cs[x] + y, ideal(x));
        if (nK > K) {
            K = nK;
            if (double(K) > S+1) {
                af = double(2*K) - SS;
                bf = 2*SS * ((SS-1)*double(K) - 1);
            }
        }
        while (not ys.empty() and ys.back() >= y) {
            x = xs.back();
            xs.pop_back();
            ys.pop_back();
            // no need to pop from deltas, since the upcoming one will be larger anyways
        }
        xs.push_back(x);
        ys.push_back(y);
        return true;
    }
    
    //! @brief maximum y allowed for an x, given values dir(z) <= x
    inline int maxallowed(int x) const {
        return ceil(MK * ideal(x) - convergence(x)) - 1;
    }
    
    //! @brief next convergence time, given is minimum such that ys[is] >= cs.size()
    inline int nextconv(int is) const {
        int x = cs.size();
        if (x == 0) return 1;
        int z = xs[is];
        return cs[z] + z + x + 1;
    }
    
    //! @brief limit for success
    inline double xlimit(double delta) const {
        return std::max( (af * delta + 2+SS) / bf, (delta-1) / (2-SS) );
    }

    //! @brief competitiveness strictly below MK is required
    frac MK;
    //! @brief competitiveness actually achieved
    frac K = 1;
    //! @brief numerator for computing x0 given delta (depends on K)
    double af = SS+2;
    //! @brief denominator for computing x0 given delta (depends on K)
    double bf = 0;
    //! @brief deltas of last generated elements
    max_deque<sq2> deltas;
    //! @brief alpha for generating elements beyond end
    sq2 alpha;
    //! @brief custom values xs -> ys of the function
    std::vector<int> xs, ys;
    //! @brief convergence times
    std::vector<int> cs;
};


//! @brief printing
std::ostream& operator<<(std::ostream& o, const func& g) {
    for (int x=0; x<200; ++x)
        o << "g(" << x << ") = " << g.dir(x) << (x%10 == 9 ? "\n" : "\t");
    return o;
}


#endif // CPP_FUNC_H_
