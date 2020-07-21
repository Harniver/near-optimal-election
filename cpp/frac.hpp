// Copyright © 2020 Giorgio Audrito. All Rights Reserved.

/**
 * @file frac.hpp
 * @brief Implementation of the frac numeric type representing fractions.
 */

#ifndef CPP_FRAC_H_
#define CPP_FRAC_H_

#include <ostream>


//! @brief Efficient computation of the greatest common divisor.
long long GCD(long long x, long long y) {
    while (y > 0) {
        x %= y;
        if (x==0) return y;
        y %= x;
    }
    return x;
}


//! @brief Numeric type representing fractions (with `long long` numerator and denominator).
class frac {
  public:
    //! @brief construction
    //! @{
    frac(long long n, long long d) : num(n), den(d) {
        reduce();
    }
    
    frac(long long n) : num(n), den(1) {}
    
    frac() : num(0), den(1) {}
    //! @}
    
    //! @brief copy and assignment
    //! @{
    frac(const frac&) = default;
    
    frac(frac&&) = default;
    
    frac& operator=(const frac&) = default;
    
    frac& operator=(frac&&) = default;
    //! @}
    
    //! @brief explicit conversion to double
    explicit operator double() const {
        return double(num) / den;
    }
    
    //! @brief infix numeric operations
    //! @{
    frac operator+=(const frac& o) {
        num = num * o.den + o.num * den;
        den *= o.den;
        reduce();
        return *this;
    }
    
    frac operator-=(const frac& o) {
        num = num * o.den - o.num * den;
        den *= o.den;
        reduce();
        return *this;
    }
    
    frac operator*=(const frac& o) {
        num *= o.num;
        den *= o.den;
        reduce();
        return *this;
    }
    
    frac operator/=(const frac& o) {
        num *= o.den;
        den *= o.num;
        reduce();
        return *this;
    }
    //! @}

    //! @brief 3-way comparison
    long long compare(const frac& o) const {
        return num * o.den - o.num * den;
    }
    
    //! @brief read-only access to the numerator
    long long numerator() const {
        return num;
    }
    
    //! @brief read-only access to the denominator
    long long denominator() const {
        return den;
    }
    
  private:
    //! @brief reduces the fraction
    void reduce() {
        long long g = GCD(num, den);
        num /= g;
        den /= g;
    }
    
    //! @brief numerator and denominator
    long long num, den;
};


//! @brief external comparison operators (to allow casting on first argument)
//! @{
bool operator<(const frac& x, const frac& y) {
    return x.compare(y) < 0;
}

bool operator<=(const frac& x, const frac& y) {
    return x.compare(y) <= 0;
}

bool operator>(const frac& x, const frac& y) {
    return x.compare(y) > 0;
}

bool operator>=(const frac& x, const frac& y) {
    return x.compare(y) >= 0;
}

bool operator==(const frac& x, const frac& y) {
    return x.compare(y) == 0;
}

bool operator!=(const frac& x, const frac& y) {
    return x.compare(y) != 0;
}
//! @}


//! @brief external arithmetic operators (to allow casting on first argument)
//! @{
frac operator+(const frac& x, const frac& y) {
    return frac(x.numerator() * y.denominator() + y.numerator() * x.denominator(), x.denominator() * y.denominator());
}

frac operator-(const frac& x, const frac& y) {
    return frac(x.numerator() * y.denominator() - y.numerator() * x.denominator(), x.denominator() * y.denominator());
}

frac operator*(const frac& x, const frac& y) {
    return frac(x.numerator() * y.numerator(), x.denominator() * y.denominator());
}

frac operator/(const frac& x, const frac& y) {
    return frac(x.numerator() * y.denominator(), x.denominator() * y.numerator());
}
//! @}


//! @brief rounding functions
//! @{
inline long long floor(const frac& f) {
    return f.numerator() / f.denominator();
}

inline long long ceil(const frac& f) {
    return (f.numerator()-1) / f.denominator() + 1;
}

inline long long round(const frac& f) {
    return (f.numerator() + f.denominator()/2) / f.denominator();
}
//! @}


//! @brief printing
std::ostream& operator<<(std::ostream& o, const frac& f) {
    return o << f.numerator() << "/" << f.denominator();
}


#endif // CPP_FRAC_H_
