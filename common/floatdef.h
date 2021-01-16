#pragma once

#include <boost/multiprecision/cpp_bin_float.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>

// using float_type = boost::multiprecision::number<boost::multiprecision::cpp_dec_float<100>>;
// using FloatT = double;
// using FloatT = boost::multiprecision::number<boost::multiprecision::cpp_bin_float<64>>;
using FloatT = boost::multiprecision::number<boost::multiprecision::cpp_bin_float<128>>;
// using FloatT = boost::multiprecision::number<boost::multiprecision::cpp_dec_float<100>>;

#include <boost/math/special_functions/round.hpp>
namespace mp = boost::multiprecision;
namespace bm = boost::math;
using bm::round;

inline int floor(const FloatT x)
{
    return static_cast<int>(x);
}

inline FloatT fd(double x)
{
    return static_cast<FloatT>(x);
}