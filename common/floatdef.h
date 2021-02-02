#pragma once
#pragma warning(disable : 4477)

//#include <boost/multiprecision/cpp_bin_float.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>
namespace mp = boost::multiprecision;

// using FloatT = mp::number<mp::cpp_bin_float<64>>;
// using FloatT = mp::number<mp::cpp_bin_float<128>>;
using FloatT = mp::number<mp::cpp_dec_float<100>, mp::et_off>;

#define TOO_SMALL_FLOAT (FloatT("-1.0E100"))
#define TOO_BIG_FLOAT (FloatT("+1.0E100"))

#define OneE27 (FloatT("1E90"))

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

