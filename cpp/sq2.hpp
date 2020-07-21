// Copyright © 2020 Giorgio Audrito. All Rights Reserved.

/**
 * @file sq2.hpp
 * @brief Implementation of the sq2 numeric type representing values of the form x + √2 y.
 */

#ifndef CPP_SQ2_H_
#define CPP_SQ2_H_

#include <ostream>


//! @brief √2.
constexpr double SQ2 = 1.414213562373095;


//! @brief Numeric type representing values of the form x + √2 y.
class sq2 {
  public:
    //! @brief construction
    //! @{
    sq2(long long na, long long nb) : a(na), b(nb) {}

    sq2(long long na) : a(na), b(0) {}

    sq2() : a(0), b(0) {}
    //! @}

    //! @brief copy and assignment
    //! @{
    sq2(const sq2&) = default;

    sq2(sq2&&) = default;

    sq2& operator=(const sq2&) = default;

    sq2& operator=(sq2&&) = default;
    //! @}

    //! @brief explicit conversion to double
    explicit operator double() const {
        return double(a) + double(b) * SQ2;
    }

    //! @brief infix numeric operations
    //! @{
    sq2 operator+=(const sq2& o) {
        a += o.a;
        b += o.b;
        return *this;
    }

    sq2 operator-=(const sq2& o) {
        a -= o.a;
        b -= o.b;
        return *this;
    }

    sq2 operator*=(const sq2& o) {
        a = a * o.a + 2 * b * o.b;
        b = b * o.a + a * o.b;
        return *this;
    }

    sq2 operator/=(const sq2& o) {
        long long q = o.a * o.a - 2 * o.b * o.b;
        a = (a * o.a - 2 * b * o.b) / q;
        b = (b * o.a - a * o.b) / q;
        return *this;
    }
    //! @}

    //! @brief 3-way comparison
    double compare(const sq2& o) const {
        if (a > o.a and b > o.b) return 1;
        if (a < o.a and b < o.b) return -1;
        if (a >= o.a and b <= o.b) {
            return (a - o.a)*(a - o.a) - 2*(o.b - b)*(o.b - b);
        }
        return 2*(b - o.b)*(b - o.b) - (o.a - a)*(o.a - a);
    }

    //! @brief read-only access to coefficients
    //! @{
    long long integral() const {
        return a;
    }

    long long irrational() const {
        return b;
    }
    //! @}

  private:
    //! @brief Coefficients representing a + √2 b.
    long long a, b;
};
//! @}


//! @brief external comparison operators (to allow casting on first argument)
//! @{
bool operator<(const sq2& x, const sq2& y) {
    return x.compare(y) < 0;
}

bool operator<=(const sq2& x, const sq2& y) {
    return x.compare(y) <= 0;
}

bool operator>(const sq2& x, const sq2& y) {
    return x.compare(y) > 0;
}

bool operator>=(const sq2& x, const sq2& y) {
    return x.compare(y) >= 0;
}

bool operator==(const sq2& x, const sq2& y) {
    return x.compare(y) == 0;
}

bool operator!=(const sq2& x, const sq2& y) {
    return x.compare(y) != 0;
}
//! @}


//! @brief external arithmetic operators (to allow casting on first argument)
//! @{
sq2 operator+(const sq2& x, const sq2& y) {
    return sq2(x.integral() + y.integral(), x.irrational() + y.irrational());
}

sq2 operator-(const sq2& x, const sq2& y) {
    return sq2(x.integral() - y.integral(), x.irrational() - y.irrational());
}

sq2 operator*(const sq2& x, const sq2& y) {
    return sq2(
        x.integral() * y.integral() + 2 * x.irrational() * y.irrational(),
        x.irrational() * y.integral() + x.integral() * y.irrational()
    );
}

sq2 operator/(const sq2& x, const sq2& y) {
    long long q = y.integral() * y.integral() - 2 * y.irrational() * y.irrational();
    return sq2(
        (x.integral() * y.integral() - 2 * x.irrational() * y.irrational()) / q,
        (x.irrational() * y.integral() - x.integral() * y.irrational()) / q
    );
}
//! @}


//! @brief printing
std::ostream& operator<<(std::ostream& o, const sq2& x) {
    if (x.integral() != 0 or x.irrational() == 0) o << x.integral();
    if (x.integral() != 0 and x.irrational() > 0) o << "+";
    if (x.irrational() != 0) o << x.irrational() << "√2";
    return o;
}


#endif // CPP_SQ2_H_
