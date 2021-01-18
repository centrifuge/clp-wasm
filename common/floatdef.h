#pragma once
#pragma warning(disable : 4477)
//#define printf(...) (void)0

#include <boost/math/special_functions/round.hpp>
#include <boost/multiprecision/cpp_bin_float.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>
namespace mp = boost::multiprecision;
namespace bm = boost::math;
using bm::round;

// using FloatT = double;
// using FloatT = mp::number<mp::cpp_bin_float<64>>;
// using FloatT = mp::number<mp::cpp_bin_float<128>>;
using FloatT = mp::number<mp::cpp_dec_float<100>, mp::et_off>;

#define TOO_SMALL_FLOAT (FloatT(-1.0E50))
#define TOO_BIG_FLOAT (FloatT(1.0E50))

namespace std
{
template <typename T>
struct common_type<T, double>
{
    using type = T;
};
template <typename T>
struct common_type<double, T>
{
    using type = T;
};
template <typename T>
struct common_type<T, int>
{
    using type = T;
};
} // namespace std

inline int floor(const FloatT & x)
{
    return static_cast<int>(x);
}

template <typename T>
FloatT fd(const T x)
{
    return static_cast<FloatT>(x);
}

