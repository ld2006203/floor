/*
 *  Flo's Open libRary (floor)
 *  Copyright (C) 2004 - 2016 Florian Ziesche
 *  
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; version 2 of the License only.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef __FLOOR_VECTOR_HELPER_HPP__
#define __FLOOR_VECTOR_HELPER_HPP__

#include <type_traits>
#include <limits>
#if !defined(FLOOR_COMPUTE) || defined(FLOOR_COMPUTE_HOST)
#include <cstdint>
#endif
#include <floor/constexpr/const_math.hpp>
using namespace std;

//! the unsigned integer type equivalent/corresponding to the size of T
template <typename T, class disabled_default = void> struct sized_unsigned_eqv;
template <typename T> struct sized_unsigned_eqv<T, typename enable_if<sizeof(T) == 1, void>::type> {
	typedef uint8_t type;
};
template <typename T> struct sized_unsigned_eqv<T, typename enable_if<sizeof(T) == 2, void>::type> {
	typedef uint16_t type;
};
template <typename T> struct sized_unsigned_eqv<T, typename enable_if<sizeof(T) == 4, void>::type> {
	typedef uint32_t type;
};
template <typename T> struct sized_unsigned_eqv<T, typename enable_if<sizeof(T) == 8, void>::type> {
	typedef uint64_t type;
};
template <typename T> struct sized_unsigned_eqv<T, typename enable_if<(sizeof(T) > 8), void>::type> {
	typedef __uint128_t type;
};

//! the signed equivalent to T
template <typename T, class disabled_default = void> struct signed_eqv;
template <> struct signed_eqv<bool> {
	typedef bool type;
};
template <typename fp_type>
struct signed_eqv<fp_type, typename enable_if<is_floating_point<fp_type>::value, void>::type> {
	typedef fp_type type;
};
template <typename int_type>
struct signed_eqv<int_type, typename enable_if<(is_integral<int_type>::value &&
												is_signed<int_type>::value &&
												!is_same<int_type, __int128_t>::value), void>::type> {
	typedef int_type type;
};
template <typename uint_type>
struct signed_eqv<uint_type, typename enable_if<(is_integral<uint_type>::value &&
												 is_unsigned<uint_type>::value &&
												 sizeof(uint_type) <= 8), void>::type> {
	typedef make_signed_t<uint_type> type;
};
template <typename uint_type>
struct signed_eqv<uint_type, typename enable_if<(is_integral<uint_type>::value &&
												 is_unsigned<uint_type>::value &&
												 !is_same<uint_type, __uint128_t>::value &&
												 sizeof(uint_type) == 16), void>::type> {
	typedef __int128_t type;
};
// is_integral is not specialized for __int128_t and __uint128_t
template <typename int128_type>
struct signed_eqv<int128_type, typename enable_if<(is_same<int128_type, __int128_t>::value ||
												   is_same<int128_type, __uint128_t>::value), void>::type> {
	typedef __int128_t type;
};

//! the integral equivalent to T
template <typename T, class disabled_default = void> struct integral_eqv;
template <> struct integral_eqv<bool> {
	typedef bool type;
};
template <typename fp_type>
struct integral_eqv<fp_type, typename enable_if<is_floating_point<fp_type>::value, void>::type> {
	typedef typename sized_unsigned_eqv<fp_type>::type type;
};
template <typename int_type>
struct integral_eqv<int_type, typename enable_if<(is_integral<int_type>::value &&
												  !is_same<int_type, __int128_t>::value &&
												  !is_same<int_type, __uint128_t>::value), void>::type> {
	typedef int_type type;
};
// is_integral is not specialized for __int128_t and __uint128_t
template <typename int128_type>
struct integral_eqv<int128_type, typename enable_if<(is_same<int128_type, __int128_t>::value ||
													 is_same<int128_type, __uint128_t>::value), void>::type> {
	typedef int128_type type;
};

// nan and inf aren't constexpr yet in msvc's stl
#if defined(_MSC_VER)
template <typename T, typename = void> struct nan_helper {
	static constexpr const T scalar_nan { (T)0 };
};
template <typename T> struct nan_helper<T, enable_if_t<is_same<T, float>::value>> {
	static constexpr const T scalar_nan { __builtin_nanf("") };
};
template <typename T> struct nan_helper<T, enable_if_t<is_same<T, double>::value>> {
	static constexpr const T scalar_nan { __builtin_nan("") };
};
template <typename T> struct nan_helper<T, enable_if_t<is_same<T, long double>::value>> {
	static constexpr const T scalar_nan { __builtin_nanl("") };
};
template <typename T, typename = void> struct inf_helper {
	static constexpr const T scalar_inf { (T)0 };
};
template <typename T> struct inf_helper<T, enable_if_t<is_same<T, float>::value>> {
	static constexpr const T scalar_inf { __builtin_huge_valf() };
};
template <typename T> struct inf_helper<T, enable_if_t<is_same<T, double>::value>> {
	static constexpr const T scalar_inf { __builtin_huge_val() };
};
template <typename T> struct inf_helper<T, enable_if_t<is_same<T, long double>::value>> {
	static constexpr const T scalar_inf { __builtin_huge_vall() };
};
#else
template <typename T> struct nan_helper {
	static constexpr const T scalar_nan { numeric_limits<T>::quiet_NaN() };
};
template <typename T> struct inf_helper {
	static constexpr const T scalar_inf { numeric_limits<T>::infinity() };
};
#endif

//! base class
template <typename T> class vector_helper {
public:
	typedef typename integral_eqv<T>::type integral_type;
	
	// these have no implementation in here
	static constexpr T modulo(const T& lhs, const T& rhs);
	static constexpr T sqrt(const T& val);
	static constexpr T rsqrt(const T& val);
	static constexpr T abs(const T& val);
	static constexpr T floor(const T& val);
	static constexpr T ceil(const T& val);
	static constexpr T round(const T& val);
	static constexpr T trunc(const T& val);
	static constexpr T rint(const T& val);
	static constexpr T fractional(const T& val);
	static constexpr T sin(const T& val);
	static constexpr T cos(const T& val);
	static constexpr T tan(const T& val);
	static constexpr T asin(const T& val);
	static constexpr T acos(const T& val);
	static constexpr T atan(const T& val);
	static constexpr T atan2(const T& lhs, const T& rhs);
	static constexpr T exp(const T& val);
	static constexpr T exp2(const T& val);
	static constexpr T log(const T& val);
	static constexpr T log2(const T& val);
	static constexpr T pow(const T& lhs, const T& rhs);
	static constexpr T fma(const T& a, const T& b, const T& c);
	static constexpr T bit_and(const T& lhs, const integral_type& rhs);
	static constexpr T bit_or(const T& lhs, const integral_type& rhs);
	static constexpr T bit_xor(const T& lhs, const integral_type& rhs);
	static constexpr T bit_left_shift(const T& lhs, const integral_type& rhs);
	static constexpr T bit_right_shift(const T& lhs, const integral_type& rhs);
	static constexpr T unary_not(const T& val);
	static constexpr T unary_complement(const T& val);
	
protected:
	// static class
	vector_helper(const vector_helper&) = delete;
	~vector_helper() = delete;
	vector_helper& operator=(const vector_helper&) = delete;
	
};

//! macro that defines a static "func_constexpr" function, taking a single argument with the name "func_name"
#define FLOOR_VH_FUNC_IMPL_1(func_name, func_constexpr, func_impl) \
static func_constexpr scalar_type func_name (const scalar_type& val) { \
	return func_impl; \
}
//! macro that defines a static "func_constexpr" function, taking two arguments with the name "func_name"
#define FLOOR_VH_FUNC_IMPL_2(func_name, func_constexpr, func_impl) \
static func_constexpr scalar_type func_name (const scalar_type& lhs, const scalar_type& rhs) { \
	return func_impl; \
}
//! macro that defines a static "func_constexpr" function, taking two arguments (lhs = scalar_type and rhs = integral_type),
//! with the name "func_name"
#define FLOOR_VH_FUNC_IMPL_2_INT(func_name, func_constexpr, func_impl, ...) \
static func_constexpr scalar_type func_name (const scalar_type& lhs, const integral_type& rhs) { \
	__VA_ARGS__ ; \
	return func_impl; \
}
//! macro that defines a static "func_constexpr" function, taking three arguments with the name "func_name"
#define FLOOR_VH_FUNC_IMPL_3(func_name, func_constexpr, func_impl) \
static func_constexpr scalar_type func_name (const scalar_type& a, const scalar_type& b, const scalar_type& c) { \
	return func_impl; \
}

//! implements the vector_helper class for a specific "vh_type" and it's function implementations
#define FLOOR_VH_IMPL(vh_type, func_impl) \
template <> class vector_helper<vh_type> { \
public: \
	typedef vh_type scalar_type; \
	typedef typename signed_eqv<vh_type>::type signed_type; \
	typedef typename sized_unsigned_eqv<vh_type>::type sized_unsigned_type; \
	typedef typename integral_eqv<vh_type>::type integral_type; \
	static constexpr const scalar_type scalar_zero { (scalar_type)0 }; \
	static constexpr const scalar_type scalar_one { (scalar_type)1 }; \
	static constexpr const scalar_type scalar_nan { nan_helper<scalar_type>::scalar_nan }; \
	static constexpr const scalar_type scalar_inf { inf_helper<scalar_type>::scalar_inf }; \
	func_impl \
\
protected: \
	vector_helper(const vector_helper&) = delete; \
	~vector_helper() = delete; \
	vector_helper& operator=(const vector_helper&) = delete; \
};

// int base types: bool, char, short, int, long long int, unsigned char, unsigned short, unsigned int, unsigned long long int
//              -> bool, int8_t, int16_t, int32_t, int64_t, uint8_t, uint16_t, uint32_t, uint64_t
// float base types: float, double, long double

#define FLOOR_VH_IMPL_DEF_FLOAT(F1, F2, F2_INT, F3) FLOOR_VH_IMPL(float, \
F2(modulo, constexpr, math::__fmod(lhs, rhs)) \
F1(sqrt, constexpr, math::__sqrt(val)) \
F1(rsqrt, constexpr, math::__rsqrt(val)) \
F1(abs, constexpr, math::__abs(val)) \
F1(floor, constexpr, math::__floor(val)) \
F1(ceil, constexpr, math::__ceil(val)) \
F1(round, constexpr, math::__round(val)) \
F1(trunc, constexpr, math::__trunc(val)) \
F1(rint, constexpr, math::__rint(val)) \
F1(fractional, constexpr, math::__fractional(val)) \
F1(sin, constexpr, math::__sin(val)) \
F1(cos, constexpr, math::__cos(val)) \
F1(tan, constexpr, math::__tan(val)) \
F1(asin, constexpr, math::__asin(val)) \
F1(acos, constexpr, math::__acos(val)) \
F1(atan, constexpr, math::__atan(val)) \
F2(atan2, constexpr, math::__atan2(lhs, rhs)) \
F1(exp, constexpr, math::__exp(val)) \
F1(exp2, constexpr, math::__exp2(val)) \
F1(log, constexpr, math::__log(val)) \
F1(log2, constexpr, math::__log2(val)) \
F2(pow, constexpr, math::__pow(lhs, rhs)) \
F3(fma, constexpr, math::__fma(a, b, c)) \
F2_INT(bit_and, , *(float*)&ret, const auto ret = *(uint32_t*)&lhs & rhs) \
F2_INT(bit_or, , *(float*)&ret, const auto ret = *(uint32_t*)&lhs | rhs) \
F2_INT(bit_xor, , *(float*)&ret, const auto ret = *(uint32_t*)&lhs ^ rhs) \
F2_INT(bit_left_shift, , *(float*)&ret, const auto ret = *(uint32_t*)&lhs << rhs) \
F2_INT(bit_right_shift, , *(float*)&ret, const auto ret = *(uint32_t*)&lhs >> rhs) \
F1(unary_not, constexpr, (val == 0.0f ? 0.0f : 1.0f)) \
F1(unary_complement, constexpr, (val < 0.0f ? 1.0f : -1.0f) * (numeric_limits<float>::max() - math::__abs(val))) \
)

#define FLOOR_VH_IMPL_DEF_DOUBLE(F1, F2, F2_INT, F3) FLOOR_VH_IMPL(double, \
F2(modulo, constexpr, math::__fmod(lhs, rhs)) \
F1(sqrt, constexpr, math::__sqrt(val)) \
F1(rsqrt, constexpr, math::__rsqrt(val)) \
F1(abs, constexpr, math::__abs(val)) \
F1(floor, constexpr, math::__floor(val)) \
F1(ceil, constexpr, math::__ceil(val)) \
F1(round, constexpr, math::__round(val)) \
F1(trunc, constexpr, math::__trunc(val)) \
F1(rint, constexpr, math::__rint(val)) \
F1(fractional, constexpr, math::__fractional(val)) \
F1(sin, constexpr, math::__sin(val)) \
F1(cos, constexpr, math::__cos(val)) \
F1(tan, constexpr, math::__tan(val)) \
F1(asin, constexpr, math::__asin(val)) \
F1(acos, constexpr, math::__acos(val)) \
F1(atan, constexpr, math::__atan(val)) \
F2(atan2, constexpr, math::__atan2(lhs, rhs)) \
F1(exp, constexpr, math::__exp(val)) \
F1(exp2, constexpr, math::__exp2(val)) \
F1(log, constexpr, math::__log(val)) \
F1(log2, constexpr, math::__log2(val)) \
F2(pow, constexpr, math::__pow(lhs, rhs)) \
F3(fma, constexpr, math::__fma(a, b, c)) \
F2_INT(bit_and, , *(double*)&ret, const auto ret = *(uint64_t*)&lhs & rhs) \
F2_INT(bit_or, , *(double*)&ret, const auto ret = *(uint64_t*)&lhs | rhs) \
F2_INT(bit_xor, , *(double*)&ret, const auto ret = *(uint64_t*)&lhs ^ rhs) \
F2_INT(bit_left_shift, , *(double*)&ret, const auto ret = *(uint64_t*)&lhs << rhs) \
F2_INT(bit_right_shift, , *(double*)&ret, const auto ret = *(uint64_t*)&lhs >> rhs) \
F1(unary_not, constexpr, (val == 0.0 ? 0.0 : 1.0)) \
F1(unary_complement, constexpr, (val < 0.0 ? 1.0 : -1.0) * (numeric_limits<double>::max() - math::__abs(val))) \
)

#define FLOOR_VH_IMPL_DEF_LDOUBLE(F1, F2, F2_INT, F3) FLOOR_VH_IMPL(long double, \
F2(modulo, constexpr, math::__fmod(lhs, rhs)) \
F1(sqrt, constexpr, math::__sqrt(val)) \
F1(rsqrt, constexpr, math::__rsqrt(val)) \
F1(abs, constexpr, math::__abs(val)) \
F1(floor, constexpr, math::__floor(val)) \
F1(ceil, constexpr, math::__ceil(val)) \
F1(round, constexpr, math::__round(val)) \
F1(trunc, constexpr, math::__trunc(val)) \
F1(rint, constexpr, math::__rint(val)) \
F1(fractional, constexpr, math::__fractional(val)) \
F1(sin, constexpr, math::__sin(val)) \
F1(cos, constexpr, math::__cos(val)) \
F1(tan, constexpr, math::__tan(val)) \
F1(asin, constexpr, math::__asin(val)) \
F1(acos, constexpr, math::__acos(val)) \
F1(atan, constexpr, math::__atan(val)) \
F2(atan2, constexpr, math::__atan2(lhs, rhs)) \
F1(exp, constexpr, math::__exp(val)) \
F1(exp2, constexpr, math::__exp2(val)) \
F1(log, constexpr, math::__log(val)) \
F1(log2, constexpr, math::__log2(val)) \
F2(pow, constexpr, math::__pow(lhs, rhs)) \
F3(fma, constexpr, math::__fma(a, b, c)) \
F2_INT(bit_and, , *(long double*)&ret, const auto ret = *(integral_type*)&lhs & rhs) \
F2_INT(bit_or, , *(long double*)&ret, const auto ret = *(integral_type*)&lhs | rhs) \
F2_INT(bit_xor, , *(long double*)&ret, const auto ret = *(integral_type*)&lhs ^ rhs) \
F2_INT(bit_left_shift, , *(long double*)&ret, const auto ret = *(integral_type*)&lhs << rhs) \
F2_INT(bit_right_shift, , *(long double*)&ret, const auto ret = *(integral_type*)&lhs >> rhs) \
F1(unary_not, constexpr, (val == 0.0L ? 0.0L : 1.0L)) \
F1(unary_complement, constexpr, (val < 0.0L ? 1.0L : -1.0L) * (numeric_limits<long double>::max() - math::__abs(val))) \
)

#define FLOOR_VH_IMPL_DEF_HALF(F1, F2, F2_INT, F3) FLOOR_VH_IMPL(half, \
F2(modulo, constexpr, math::__fmod(lhs, rhs)) \
F1(sqrt, constexpr, math::__sqrt(val)) \
F1(rsqrt, constexpr, math::__rsqrt(val)) \
F1(abs, constexpr, math::__abs(val)) \
F1(floor, constexpr, math::__floor(val)) \
F1(ceil, constexpr, math::__ceil(val)) \
F1(round, constexpr, math::__round(val)) \
F1(trunc, constexpr, math::__trunc(val)) \
F1(rint, constexpr, math::__rint(val)) \
F1(fractional, constexpr, math::__fractional(val)) \
F1(sin, constexpr, math::__sin(val)) \
F1(cos, constexpr, math::__cos(val)) \
F1(tan, constexpr, math::__tan(val)) \
F1(asin, constexpr, math::__asin(val)) \
F1(acos, constexpr, math::__acos(val)) \
F1(atan, constexpr, math::__atan(val)) \
F2(atan2, constexpr, math::__atan2(lhs, rhs)) \
F1(exp, constexpr, math::__exp(val)) \
F1(exp2, constexpr, math::__exp2(val)) \
F1(log, constexpr, math::__log(val)) \
F1(log2, constexpr, math::__log2(val)) \
F2(pow, constexpr, math::__pow(lhs, rhs)) \
F3(fma, constexpr, math::__fma(a, b, c)) \
F2_INT(bit_and, , *(float*)&ret, const auto ret = *(uint32_t*)&lhs & rhs) \
F2_INT(bit_or, , *(float*)&ret, const auto ret = *(uint32_t*)&lhs | rhs) \
F2_INT(bit_xor, , *(float*)&ret, const auto ret = *(uint32_t*)&lhs ^ rhs) \
F2_INT(bit_left_shift, , *(float*)&ret, const auto ret = *(uint32_t*)&lhs << rhs) \
F2_INT(bit_right_shift, , *(float*)&ret, const auto ret = *(uint32_t*)&lhs >> rhs) \
F1(unary_not, constexpr, (val == 0.0f ? 0.0f : 1.0f)) \
F1(unary_complement, constexpr, (val < 0.0f ? 1.0f : -1.0f) * (numeric_limits<float>::max() - math::__abs(val))) \
)

#define FLOOR_VH_IMPL_DEF_INT32(F1, F2, F2_INT, F3) FLOOR_VH_IMPL(int32_t, \
F2(modulo, constexpr, lhs % rhs) \
F1(sqrt, constexpr, (scalar_type)math::__sqrt((const_math::max_rt_fp_type)val)) \
F1(rsqrt, constexpr, val) \
F1(abs, constexpr, const_math::abs(val)) \
F1(floor, constexpr, val) \
F1(ceil, constexpr, val) \
F1(round, constexpr, val) \
F1(trunc, constexpr, val) \
F1(rint, constexpr, val) \
F1(fractional, constexpr, scalar_type(0)) \
F1(sin, constexpr, (scalar_type)math::__sin((const_math::max_rt_fp_type)val)) \
F1(cos, constexpr, (scalar_type)math::__cos((const_math::max_rt_fp_type)val)) \
F1(tan, constexpr, (scalar_type)math::__tan((const_math::max_rt_fp_type)val)) \
F1(asin, constexpr, (scalar_type)math::__asin((const_math::max_rt_fp_type)val)) \
F1(acos, constexpr, (scalar_type)math::__acos((const_math::max_rt_fp_type)val)) \
F1(atan, constexpr, (scalar_type)math::__atan((const_math::max_rt_fp_type)val)) \
F2(atan2, constexpr, (scalar_type)math::__atan2((const_math::max_rt_fp_type)lhs, (const_math::max_rt_fp_type)rhs)) \
F1(exp, constexpr, (scalar_type)math::__exp((const_math::max_rt_fp_type)val)) \
F1(exp2, constexpr, (scalar_type)math::__exp2((const_math::max_rt_fp_type)val)) \
F1(log, constexpr, (scalar_type)math::__log((const_math::max_rt_fp_type)val)) \
F1(log2, constexpr, (scalar_type)math::__log2((const_math::max_rt_fp_type)val)) \
F2(pow, constexpr, (scalar_type)math::__pow((const_math::max_rt_fp_type)lhs, (const_math::max_rt_fp_type)rhs)) \
F3(fma, constexpr, ((a * b) + c)) \
F2_INT(bit_and, constexpr, (lhs & rhs)) \
F2_INT(bit_or, constexpr, (lhs | rhs)) \
F2_INT(bit_xor, constexpr, (lhs ^ rhs)) \
F2_INT(bit_left_shift, constexpr, (lhs << rhs)) \
F2_INT(bit_right_shift, constexpr, (lhs >> rhs)) \
F1(unary_not, constexpr, (!val)) \
F1(unary_complement, constexpr, (~val)) \
)

#define FLOOR_VH_IMPL_DEF_UINT32(F1, F2, F2_INT, F3) FLOOR_VH_IMPL(uint32_t, \
F2(modulo, constexpr, lhs % rhs) \
F1(sqrt, constexpr, (scalar_type)math::__sqrt((const_math::max_rt_fp_type)val)) \
F1(rsqrt, constexpr, val) \
F1(abs, constexpr, val) \
F1(floor, constexpr, val) \
F1(ceil, constexpr, val) \
F1(round, constexpr, val) \
F1(trunc, constexpr, val) \
F1(rint, constexpr, val) \
F1(fractional, constexpr, scalar_type(0)) \
F1(sin, constexpr, (scalar_type)math::__sin((const_math::max_rt_fp_type)val)) \
F1(cos, constexpr, (scalar_type)math::__cos((const_math::max_rt_fp_type)val)) \
F1(tan, constexpr, (scalar_type)math::__tan((const_math::max_rt_fp_type)val)) \
F1(asin, constexpr, (scalar_type)math::__asin((const_math::max_rt_fp_type)val)) \
F1(acos, constexpr, (scalar_type)math::__acos((const_math::max_rt_fp_type)val)) \
F1(atan, constexpr, (scalar_type)math::__atan((const_math::max_rt_fp_type)val)) \
F2(atan2, constexpr, (scalar_type)math::__atan2((const_math::max_rt_fp_type)lhs, (const_math::max_rt_fp_type)rhs)) \
F1(exp, constexpr, (scalar_type)math::__exp((const_math::max_rt_fp_type)val)) \
F1(exp2, constexpr, (scalar_type)math::__exp2((const_math::max_rt_fp_type)val)) \
F1(log, constexpr, (scalar_type)math::__log((const_math::max_rt_fp_type)val)) \
F1(log2, constexpr, (scalar_type)math::__log2((const_math::max_rt_fp_type)val)) \
F2(pow, constexpr, (scalar_type)math::__pow((const_math::max_rt_fp_type)lhs, (const_math::max_rt_fp_type)rhs)) \
F3(fma, constexpr, ((a * b) + c)) \
F2_INT(bit_and, constexpr, (lhs & rhs)) \
F2_INT(bit_or, constexpr, (lhs | rhs)) \
F2_INT(bit_xor, constexpr, (lhs ^ rhs)) \
F2_INT(bit_left_shift, constexpr, (lhs << rhs)) \
F2_INT(bit_right_shift, constexpr, (lhs >> rhs)) \
F1(unary_not, constexpr, (!val)) \
F1(unary_complement, constexpr, (~val)) \
)

#define FLOOR_VH_IMPL_DEF_INT8(F1, F2, F2_INT, F3) FLOOR_VH_IMPL(int8_t, \
F2(modulo, constexpr, lhs % rhs) \
F1(sqrt, constexpr, (scalar_type)math::__sqrt((const_math::max_rt_fp_type)val)) \
F1(rsqrt, constexpr, val) \
F1(abs, constexpr, const_math::abs(val)) \
F1(floor, constexpr, val) \
F1(ceil, constexpr, val) \
F1(round, constexpr, val) \
F1(trunc, constexpr, val) \
F1(rint, constexpr, val) \
F1(fractional, constexpr, scalar_type(0)) \
F1(sin, constexpr, (scalar_type)math::__sin((const_math::max_rt_fp_type)val)) \
F1(cos, constexpr, (scalar_type)math::__cos((const_math::max_rt_fp_type)val)) \
F1(tan, constexpr, (scalar_type)math::__tan((const_math::max_rt_fp_type)val)) \
F1(asin, constexpr, (scalar_type)math::__asin((const_math::max_rt_fp_type)val)) \
F1(acos, constexpr, (scalar_type)math::__acos((const_math::max_rt_fp_type)val)) \
F1(atan, constexpr, (scalar_type)math::__atan((const_math::max_rt_fp_type)val)) \
F2(atan2, constexpr, (scalar_type)math::__atan2((const_math::max_rt_fp_type)lhs, (const_math::max_rt_fp_type)rhs)) \
F1(exp, constexpr, (scalar_type)math::__exp((const_math::max_rt_fp_type)val)) \
F1(exp2, constexpr, (scalar_type)math::__exp2((const_math::max_rt_fp_type)val)) \
F1(log, constexpr, (scalar_type)math::__log((const_math::max_rt_fp_type)val)) \
F1(log2, constexpr, (scalar_type)math::__log2((const_math::max_rt_fp_type)val)) \
F2(pow, constexpr, (scalar_type)math::__pow((const_math::max_rt_fp_type)lhs, (const_math::max_rt_fp_type)rhs)) \
F3(fma, constexpr, ((a * b) + c)) \
F2_INT(bit_and, constexpr, (lhs & rhs)) \
F2_INT(bit_or, constexpr, (lhs | rhs)) \
F2_INT(bit_xor, constexpr, (lhs ^ rhs)) \
F2_INT(bit_left_shift, constexpr, (int8_t)(lhs << rhs)) \
F2_INT(bit_right_shift, constexpr, (int8_t)(lhs >> rhs)) \
F1(unary_not, constexpr, (!val)) \
F1(unary_complement, constexpr, (~val)) \
)

#define FLOOR_VH_IMPL_DEF_UINT8(F1, F2, F2_INT, F3) FLOOR_VH_IMPL(uint8_t, \
F2(modulo, constexpr, lhs % rhs) \
F1(sqrt, constexpr, (scalar_type)math::__sqrt((const_math::max_rt_fp_type)val)) \
F1(rsqrt, constexpr, val) \
F1(abs, constexpr, val) \
F1(floor, constexpr, val) \
F1(ceil, constexpr, val) \
F1(round, constexpr, val) \
F1(trunc, constexpr, val) \
F1(rint, constexpr, val) \
F1(fractional, constexpr, scalar_type(0)) \
F1(sin, constexpr, (scalar_type)math::__sin((const_math::max_rt_fp_type)val)) \
F1(cos, constexpr, (scalar_type)math::__cos((const_math::max_rt_fp_type)val)) \
F1(tan, constexpr, (scalar_type)math::__tan((const_math::max_rt_fp_type)val)) \
F1(asin, constexpr, (scalar_type)math::__asin((const_math::max_rt_fp_type)val)) \
F1(acos, constexpr, (scalar_type)math::__acos((const_math::max_rt_fp_type)val)) \
F1(atan, constexpr, (scalar_type)math::__atan((const_math::max_rt_fp_type)val)) \
F2(atan2, constexpr, (scalar_type)math::__atan2((const_math::max_rt_fp_type)lhs, (const_math::max_rt_fp_type)rhs)) \
F1(exp, constexpr, (scalar_type)math::__exp((const_math::max_rt_fp_type)val)) \
F1(exp2, constexpr, (scalar_type)math::__exp2((const_math::max_rt_fp_type)val)) \
F1(log, constexpr, (scalar_type)math::__log((const_math::max_rt_fp_type)val)) \
F1(log2, constexpr, (scalar_type)math::__log2((const_math::max_rt_fp_type)val)) \
F2(pow, constexpr, (scalar_type)math::__pow((const_math::max_rt_fp_type)lhs, (const_math::max_rt_fp_type)rhs)) \
F3(fma, constexpr, ((a * b) + c)) \
F2_INT(bit_and, constexpr, (lhs & rhs)) \
F2_INT(bit_or, constexpr, (lhs | rhs)) \
F2_INT(bit_xor, constexpr, (lhs ^ rhs)) \
F2_INT(bit_left_shift, constexpr, (uint8_t)(lhs << rhs)) \
F2_INT(bit_right_shift, constexpr, (uint8_t)(lhs >> rhs)) \
F1(unary_not, constexpr, (!val)) \
F1(unary_complement, constexpr, (~val)) \
)

#define FLOOR_VH_IMPL_DEF_INT16(F1, F2, F2_INT, F3) FLOOR_VH_IMPL(int16_t, \
F2(modulo, constexpr, lhs % rhs) \
F1(sqrt, constexpr, (scalar_type)math::__sqrt((const_math::max_rt_fp_type)val)) \
F1(rsqrt, constexpr, val) \
F1(abs, constexpr, const_math::abs(val)) \
F1(floor, constexpr, val) \
F1(ceil, constexpr, val) \
F1(round, constexpr, val) \
F1(trunc, constexpr, val) \
F1(rint, constexpr, val) \
F1(fractional, constexpr, scalar_type(0)) \
F1(sin, constexpr, (scalar_type)math::__sin((const_math::max_rt_fp_type)val)) \
F1(cos, constexpr, (scalar_type)math::__cos((const_math::max_rt_fp_type)val)) \
F1(tan, constexpr, (scalar_type)math::__tan((const_math::max_rt_fp_type)val)) \
F1(asin, constexpr, (scalar_type)math::__asin((const_math::max_rt_fp_type)val)) \
F1(acos, constexpr, (scalar_type)math::__acos((const_math::max_rt_fp_type)val)) \
F1(atan, constexpr, (scalar_type)math::__atan((const_math::max_rt_fp_type)val)) \
F2(atan2, constexpr, (scalar_type)math::__atan2((const_math::max_rt_fp_type)lhs, (const_math::max_rt_fp_type)rhs)) \
F1(exp, constexpr, (scalar_type)math::__exp((const_math::max_rt_fp_type)val)) \
F1(exp2, constexpr, (scalar_type)math::__exp2((const_math::max_rt_fp_type)val)) \
F1(log, constexpr, (scalar_type)math::__log((const_math::max_rt_fp_type)val)) \
F1(log2, constexpr, (scalar_type)math::__log2((const_math::max_rt_fp_type)val)) \
F2(pow, constexpr, (scalar_type)math::__pow((const_math::max_rt_fp_type)lhs, (const_math::max_rt_fp_type)rhs)) \
F3(fma, constexpr, ((a * b) + c)) \
F2_INT(bit_and, constexpr, (lhs & rhs)) \
F2_INT(bit_or, constexpr, (lhs | rhs)) \
F2_INT(bit_xor, constexpr, (lhs ^ rhs)) \
F2_INT(bit_left_shift, constexpr, (int16_t)(lhs << rhs)) \
F2_INT(bit_right_shift, constexpr, (int16_t)(lhs >> rhs)) \
F1(unary_not, constexpr, (!val)) \
F1(unary_complement, constexpr, (~val)) \
)

#define FLOOR_VH_IMPL_DEF_UINT16(F1, F2, F2_INT, F3) FLOOR_VH_IMPL(uint16_t, \
F2(modulo, constexpr, lhs % rhs) \
F1(sqrt, constexpr, (scalar_type)math::__sqrt((const_math::max_rt_fp_type)val)) \
F1(rsqrt, constexpr, val) \
F1(abs, constexpr, val) \
F1(floor, constexpr, val) \
F1(ceil, constexpr, val) \
F1(round, constexpr, val) \
F1(trunc, constexpr, val) \
F1(rint, constexpr, val) \
F1(fractional, constexpr, scalar_type(0)) \
F1(sin, constexpr, (scalar_type)math::__sin((const_math::max_rt_fp_type)val)) \
F1(cos, constexpr, (scalar_type)math::__cos((const_math::max_rt_fp_type)val)) \
F1(tan, constexpr, (scalar_type)math::__tan((const_math::max_rt_fp_type)val)) \
F1(asin, constexpr, (scalar_type)math::__asin((const_math::max_rt_fp_type)val)) \
F1(acos, constexpr, (scalar_type)math::__acos((const_math::max_rt_fp_type)val)) \
F1(atan, constexpr, (scalar_type)math::__atan((const_math::max_rt_fp_type)val)) \
F2(atan2, constexpr, (scalar_type)math::__atan2((const_math::max_rt_fp_type)lhs, (const_math::max_rt_fp_type)rhs)) \
F1(exp, constexpr, (scalar_type)math::__exp((const_math::max_rt_fp_type)val)) \
F1(exp2, constexpr, (scalar_type)math::__exp2((const_math::max_rt_fp_type)val)) \
F1(log, constexpr, (scalar_type)math::__log((const_math::max_rt_fp_type)val)) \
F1(log2, constexpr, (scalar_type)math::__log2((const_math::max_rt_fp_type)val)) \
F2(pow, constexpr, (scalar_type)math::__pow((const_math::max_rt_fp_type)lhs, (const_math::max_rt_fp_type)rhs)) \
F3(fma, constexpr, ((a * b) + c)) \
F2_INT(bit_and, constexpr, (lhs & rhs)) \
F2_INT(bit_or, constexpr, (lhs | rhs)) \
F2_INT(bit_xor, constexpr, (lhs ^ rhs)) \
F2_INT(bit_left_shift, constexpr, (uint16_t)(lhs << rhs)) \
F2_INT(bit_right_shift, constexpr, (uint16_t)(lhs >> rhs)) \
F1(unary_not, constexpr, (!val)) \
F1(unary_complement, constexpr, (~val)) \
)

#define FLOOR_VH_IMPL_DEF_INT64(F1, F2, F2_INT, F3) FLOOR_VH_IMPL(int64_t, \
F2(modulo, constexpr, lhs % rhs) \
F1(sqrt, constexpr, (scalar_type)math::__sqrt((const_math::max_rt_fp_type)val)) \
F1(rsqrt, constexpr, val) \
F1(abs, constexpr, const_math::abs(val)) \
F1(floor, constexpr, val) \
F1(ceil, constexpr, val) \
F1(round, constexpr, val) \
F1(trunc, constexpr, val) \
F1(rint, constexpr, val) \
F1(fractional, constexpr, scalar_type(0)) \
F1(sin, constexpr, (scalar_type)math::__sin((const_math::max_rt_fp_type)val)) \
F1(cos, constexpr, (scalar_type)math::__cos((const_math::max_rt_fp_type)val)) \
F1(tan, constexpr, (scalar_type)math::__tan((const_math::max_rt_fp_type)val)) \
F1(asin, constexpr, (scalar_type)math::__asin((const_math::max_rt_fp_type)val)) \
F1(acos, constexpr, (scalar_type)math::__acos((const_math::max_rt_fp_type)val)) \
F1(atan, constexpr, (scalar_type)math::__atan((const_math::max_rt_fp_type)val)) \
F2(atan2, constexpr, (scalar_type)math::__atan2((const_math::max_rt_fp_type)lhs, (const_math::max_rt_fp_type)rhs)) \
F1(exp, constexpr, (scalar_type)math::__exp((const_math::max_rt_fp_type)val)) \
F1(exp2, constexpr, (scalar_type)math::__exp2((const_math::max_rt_fp_type)val)) \
F1(log, constexpr, (scalar_type)math::__log((const_math::max_rt_fp_type)val)) \
F1(log2, constexpr, (scalar_type)math::__log2((const_math::max_rt_fp_type)val)) \
F2(pow, constexpr, (scalar_type)math::__pow((const_math::max_rt_fp_type)lhs, (const_math::max_rt_fp_type)rhs)) \
F3(fma, constexpr, ((a * b) + c)) \
F2_INT(bit_and, constexpr, (lhs & rhs)) \
F2_INT(bit_or, constexpr, (lhs | rhs)) \
F2_INT(bit_xor, constexpr, (lhs ^ rhs)) \
F2_INT(bit_left_shift, constexpr, (lhs << rhs)) \
F2_INT(bit_right_shift, constexpr, (lhs >> rhs)) \
F1(unary_not, constexpr, (!val)) \
F1(unary_complement, constexpr, (~val)) \
)

#define FLOOR_VH_IMPL_DEF_UINT64(F1, F2, F2_INT, F3) FLOOR_VH_IMPL(uint64_t, \
F2(modulo, constexpr, lhs % rhs) \
F1(sqrt, constexpr, (scalar_type)math::__sqrt((const_math::max_rt_fp_type)val)) \
F1(rsqrt, constexpr, val) \
F1(abs, constexpr, val) \
F1(floor, constexpr, val) \
F1(ceil, constexpr, val) \
F1(round, constexpr, val) \
F1(trunc, constexpr, val) \
F1(rint, constexpr, val) \
F1(fractional, constexpr, scalar_type(0)) \
F1(sin, constexpr, (scalar_type)math::__sin((const_math::max_rt_fp_type)val)) \
F1(cos, constexpr, (scalar_type)math::__cos((const_math::max_rt_fp_type)val)) \
F1(tan, constexpr, (scalar_type)math::__tan((const_math::max_rt_fp_type)val)) \
F1(asin, constexpr, (scalar_type)math::__asin((const_math::max_rt_fp_type)val)) \
F1(acos, constexpr, (scalar_type)math::__acos((const_math::max_rt_fp_type)val)) \
F1(atan, constexpr, (scalar_type)math::__atan((const_math::max_rt_fp_type)val)) \
F2(atan2, constexpr, (scalar_type)math::__atan2((const_math::max_rt_fp_type)lhs, (const_math::max_rt_fp_type)rhs)) \
F1(exp, constexpr, (scalar_type)math::__exp((const_math::max_rt_fp_type)val)) \
F1(exp2, constexpr, (scalar_type)math::__exp2((const_math::max_rt_fp_type)val)) \
F1(log, constexpr, (scalar_type)math::__log((const_math::max_rt_fp_type)val)) \
F1(log2, constexpr, (scalar_type)math::__log2((const_math::max_rt_fp_type)val)) \
F2(pow, constexpr, (scalar_type)math::__pow((const_math::max_rt_fp_type)lhs, (const_math::max_rt_fp_type)rhs)) \
F3(fma, constexpr, ((a * b) + c)) \
F2_INT(bit_and, constexpr, (lhs & rhs)) \
F2_INT(bit_or, constexpr, (lhs | rhs)) \
F2_INT(bit_xor, constexpr, (lhs ^ rhs)) \
F2_INT(bit_left_shift, constexpr, (lhs << rhs)) \
F2_INT(bit_right_shift, constexpr, (lhs >> rhs)) \
F1(unary_not, constexpr, (!val)) \
F1(unary_complement, constexpr, (~val)) \
)

#define FLOOR_VH_IMPL_DEF_SSIZE_T(F1, F2, F2_INT, F3) FLOOR_VH_IMPL(ssize_t, \
F2(modulo, constexpr, lhs % rhs) \
F1(sqrt, constexpr, (scalar_type)math::__sqrt((const_math::max_rt_fp_type)val)) \
F1(rsqrt, constexpr, val) \
F1(abs, constexpr, const_math::abs(val)) \
F1(floor, constexpr, val) \
F1(ceil, constexpr, val) \
F1(round, constexpr, val) \
F1(trunc, constexpr, val) \
F1(rint, constexpr, val) \
F1(fractional, constexpr, scalar_type(0)) \
F1(sin, constexpr, (scalar_type)math::__sin((const_math::max_rt_fp_type)val)) \
F1(cos, constexpr, (scalar_type)math::__cos((const_math::max_rt_fp_type)val)) \
F1(tan, constexpr, (scalar_type)math::__tan((const_math::max_rt_fp_type)val)) \
F1(asin, constexpr, (scalar_type)math::__asin((const_math::max_rt_fp_type)val)) \
F1(acos, constexpr, (scalar_type)math::__acos((const_math::max_rt_fp_type)val)) \
F1(atan, constexpr, (scalar_type)math::__atan((const_math::max_rt_fp_type)val)) \
F2(atan2, constexpr, (scalar_type)math::__atan2((const_math::max_rt_fp_type)lhs, (const_math::max_rt_fp_type)rhs)) \
F1(exp, constexpr, (scalar_type)math::__exp((const_math::max_rt_fp_type)val)) \
F1(exp2, constexpr, (scalar_type)math::__exp2((const_math::max_rt_fp_type)val)) \
F1(log, constexpr, (scalar_type)math::__log((const_math::max_rt_fp_type)val)) \
F1(log2, constexpr, (scalar_type)math::__log2((const_math::max_rt_fp_type)val)) \
F2(pow, constexpr, (scalar_type)math::__pow((const_math::max_rt_fp_type)lhs, (const_math::max_rt_fp_type)rhs)) \
F3(fma, constexpr, ((a * b) + c)) \
F2_INT(bit_and, constexpr, (lhs & rhs)) \
F2_INT(bit_or, constexpr, (lhs | rhs)) \
F2_INT(bit_xor, constexpr, (lhs ^ rhs)) \
F2_INT(bit_left_shift, constexpr, (lhs << rhs)) \
F2_INT(bit_right_shift, constexpr, (lhs >> rhs)) \
F1(unary_not, constexpr, (!val)) \
F1(unary_complement, constexpr, (~val)) \
)

#define FLOOR_VH_IMPL_DEF_SIZE_T(F1, F2, F2_INT, F3) FLOOR_VH_IMPL(size_t, \
F2(modulo, constexpr, lhs % rhs) \
F1(sqrt, constexpr, (scalar_type)math::__sqrt((const_math::max_rt_fp_type)val)) \
F1(rsqrt, constexpr, val) \
F1(abs, constexpr, val) \
F1(floor, constexpr, val) \
F1(ceil, constexpr, val) \
F1(round, constexpr, val) \
F1(trunc, constexpr, val) \
F1(rint, constexpr, val) \
F1(fractional, constexpr, scalar_type(0)) \
F1(sin, constexpr, (scalar_type)math::__sin((const_math::max_rt_fp_type)val)) \
F1(cos, constexpr, (scalar_type)math::__cos((const_math::max_rt_fp_type)val)) \
F1(tan, constexpr, (scalar_type)math::__tan((const_math::max_rt_fp_type)val)) \
F1(asin, constexpr, (scalar_type)math::__asin((const_math::max_rt_fp_type)val)) \
F1(acos, constexpr, (scalar_type)math::__acos((const_math::max_rt_fp_type)val)) \
F1(atan, constexpr, (scalar_type)math::__atan((const_math::max_rt_fp_type)val)) \
F2(atan2, constexpr, (scalar_type)math::__atan2((const_math::max_rt_fp_type)lhs, (const_math::max_rt_fp_type)rhs)) \
F1(exp, constexpr, (scalar_type)math::__exp((const_math::max_rt_fp_type)val)) \
F1(exp2, constexpr, (scalar_type)math::__exp2((const_math::max_rt_fp_type)val)) \
F1(log, constexpr, (scalar_type)math::__log((const_math::max_rt_fp_type)val)) \
F1(log2, constexpr, (scalar_type)math::__log2((const_math::max_rt_fp_type)val)) \
F2(pow, constexpr, (scalar_type)math::__pow((const_math::max_rt_fp_type)lhs, (const_math::max_rt_fp_type)rhs)) \
F3(fma, constexpr, ((a * b) + c)) \
F2_INT(bit_and, constexpr, (lhs & rhs)) \
F2_INT(bit_or, constexpr, (lhs | rhs)) \
F2_INT(bit_xor, constexpr, (lhs ^ rhs)) \
F2_INT(bit_left_shift, constexpr, (lhs << rhs)) \
F2_INT(bit_right_shift, constexpr, (lhs >> rhs)) \
F1(unary_not, constexpr, (!val)) \
F1(unary_complement, constexpr, (~val)) \
)

#define FLOOR_VH_IMPL_DEF_INT128(F1, F2, F2_INT, F3) FLOOR_VH_IMPL(__int128_t, \
F2(modulo, constexpr, lhs % rhs) \
F1(sqrt, constexpr, (scalar_type)math::__sqrt((const_math::max_rt_fp_type)val)) \
F1(rsqrt, constexpr, val) \
F1(abs, constexpr, const_math::abs(val)) \
F1(floor, constexpr, val) \
F1(ceil, constexpr, val) \
F1(round, constexpr, val) \
F1(trunc, constexpr, val) \
F1(rint, constexpr, val) \
F1(fractional, constexpr, scalar_type(0)) \
F1(sin, constexpr, (scalar_type)math::__sin((const_math::max_rt_fp_type)val)) \
F1(cos, constexpr, (scalar_type)math::__cos((const_math::max_rt_fp_type)val)) \
F1(tan, constexpr, (scalar_type)math::__tan((const_math::max_rt_fp_type)val)) \
F1(asin, constexpr, (scalar_type)math::__asin((const_math::max_rt_fp_type)val)) \
F1(acos, constexpr, (scalar_type)math::__acos((const_math::max_rt_fp_type)val)) \
F1(atan, constexpr, (scalar_type)math::__atan((const_math::max_rt_fp_type)val)) \
F2(atan2, constexpr, (scalar_type)math::__atan2((const_math::max_rt_fp_type)lhs, (const_math::max_rt_fp_type)rhs)) \
F1(exp, constexpr, (scalar_type)math::__exp((const_math::max_rt_fp_type)val)) \
F1(exp2, constexpr, (scalar_type)math::__exp2((const_math::max_rt_fp_type)val)) \
F1(log, constexpr, (scalar_type)math::__log((const_math::max_rt_fp_type)val)) \
F1(log2, constexpr, (scalar_type)math::__log2((const_math::max_rt_fp_type)val)) \
F2(pow, constexpr, (scalar_type)math::__pow((const_math::max_rt_fp_type)lhs, (const_math::max_rt_fp_type)rhs)) \
F3(fma, constexpr, ((a * b) + c)) \
F2_INT(bit_and, constexpr, (lhs & rhs)) \
F2_INT(bit_or, constexpr, (lhs | rhs)) \
F2_INT(bit_xor, constexpr, (lhs ^ rhs)) \
F2_INT(bit_left_shift, constexpr, (lhs << rhs)) \
F2_INT(bit_right_shift, constexpr, (lhs >> rhs)) \
F1(unary_not, constexpr, (!val)) \
F1(unary_complement, constexpr, (~val)) \
)

#define FLOOR_VH_IMPL_DEF_UINT128(F1, F2, F2_INT, F3) FLOOR_VH_IMPL(__uint128_t, \
F2(modulo, constexpr, lhs % rhs) \
F1(sqrt, constexpr, (scalar_type)math::__sqrt((const_math::max_rt_fp_type)val)) \
F1(rsqrt, constexpr, val) \
F1(abs, constexpr, val) \
F1(floor, constexpr, val) \
F1(ceil, constexpr, val) \
F1(round, constexpr, val) \
F1(trunc, constexpr, val) \
F1(rint, constexpr, val) \
F1(fractional, constexpr, scalar_type(0)) \
F1(sin, constexpr, (scalar_type)math::__sin((const_math::max_rt_fp_type)val)) \
F1(cos, constexpr, (scalar_type)math::__cos((const_math::max_rt_fp_type)val)) \
F1(tan, constexpr, (scalar_type)math::__tan((const_math::max_rt_fp_type)val)) \
F1(asin, constexpr, (scalar_type)math::__asin((const_math::max_rt_fp_type)val)) \
F1(acos, constexpr, (scalar_type)math::__acos((const_math::max_rt_fp_type)val)) \
F1(atan, constexpr, (scalar_type)math::__atan((const_math::max_rt_fp_type)val)) \
F2(atan2, constexpr, (scalar_type)math::__atan2((const_math::max_rt_fp_type)lhs, (const_math::max_rt_fp_type)rhs)) \
F1(exp, constexpr, (scalar_type)math::__exp((const_math::max_rt_fp_type)val)) \
F1(exp2, constexpr, (scalar_type)math::__exp2((const_math::max_rt_fp_type)val)) \
F1(log, constexpr, (scalar_type)math::__log((const_math::max_rt_fp_type)val)) \
F1(log2, constexpr, (scalar_type)math::__log2((const_math::max_rt_fp_type)val)) \
F2(pow, constexpr, (scalar_type)math::__pow((const_math::max_rt_fp_type)lhs, (const_math::max_rt_fp_type)rhs)) \
F3(fma, constexpr, ((a * b) + c)) \
F2_INT(bit_and, constexpr, (lhs & rhs)) \
F2_INT(bit_or, constexpr, (lhs | rhs)) \
F2_INT(bit_xor, constexpr, (lhs ^ rhs)) \
F2_INT(bit_left_shift, constexpr, (lhs << rhs)) \
F2_INT(bit_right_shift, constexpr, (lhs >> rhs)) \
F1(unary_not, constexpr, (!val)) \
F1(unary_complement, constexpr, (~val)) \
)

#define FLOOR_VH_IMPL_DEF_BOOL(F1, F2, F2_INT, F3) FLOOR_VH_IMPL(bool, \
F2(modulo, constexpr, lhs % rhs) \
F1(sqrt, constexpr, val) \
F1(rsqrt, constexpr, val) \
F1(abs, constexpr, val) \
F1(floor, constexpr, val) \
F1(ceil, constexpr, val) \
F1(round, constexpr, val) \
F1(trunc, constexpr, val) \
F1(rint, constexpr, val) \
F1(fractional, constexpr, false) \
F1(sin, constexpr, val) \
F1(cos, constexpr, val) \
F1(tan, constexpr, val) \
F1(asin, constexpr, val) \
F1(acos, constexpr, val) \
F1(atan, constexpr, val) \
F2(atan2, constexpr, lhs & rhs) \
F1(exp, constexpr, val) \
F1(exp2, constexpr, val) \
F1(log, constexpr, val) \
F1(log2, constexpr, val) \
F2(pow, constexpr, lhs | rhs) \
F3(fma, constexpr, ((a * b) + c)) \
F2_INT(bit_and, constexpr, (lhs & rhs)) \
F2_INT(bit_or, constexpr, (lhs | rhs)) \
F2_INT(bit_xor, constexpr, (lhs ^ rhs)) \
F2_INT(bit_left_shift, constexpr, (lhs << rhs)) \
F2_INT(bit_right_shift, constexpr, (lhs >> rhs)) \
F1(unary_not, constexpr, (!val)) \
F1(unary_complement, constexpr, (~val)) \
)

// input parameters might be unused if the function/operation isn't applicable to the type
FLOOR_PUSH_WARNINGS()
FLOOR_IGNORE_WARNING(unused-parameter)

FLOOR_VH_IMPL_DEF_FLOAT(FLOOR_VH_FUNC_IMPL_1, FLOOR_VH_FUNC_IMPL_2, FLOOR_VH_FUNC_IMPL_2_INT, FLOOR_VH_FUNC_IMPL_3)
#if !defined(FLOOR_COMPUTE_NO_DOUBLE)
FLOOR_VH_IMPL_DEF_DOUBLE(FLOOR_VH_FUNC_IMPL_1, FLOOR_VH_FUNC_IMPL_2, FLOOR_VH_FUNC_IMPL_2_INT, FLOOR_VH_FUNC_IMPL_3)
#endif
#if !defined(FLOOR_COMPUTE) || defined(FLOOR_COMPUTE_HOST)
FLOOR_VH_IMPL_DEF_LDOUBLE(FLOOR_VH_FUNC_IMPL_1, FLOOR_VH_FUNC_IMPL_2, FLOOR_VH_FUNC_IMPL_2_INT, FLOOR_VH_FUNC_IMPL_3)
#endif
#if defined(FLOOR_COMPUTE_METAL)
FLOOR_VH_IMPL_DEF_HALF(FLOOR_VH_FUNC_IMPL_1, FLOOR_VH_FUNC_IMPL_2, FLOOR_VH_FUNC_IMPL_2_INT, FLOOR_VH_FUNC_IMPL_3)
#endif
FLOOR_VH_IMPL_DEF_INT32(FLOOR_VH_FUNC_IMPL_1, FLOOR_VH_FUNC_IMPL_2, FLOOR_VH_FUNC_IMPL_2_INT, FLOOR_VH_FUNC_IMPL_3)
FLOOR_VH_IMPL_DEF_UINT32(FLOOR_VH_FUNC_IMPL_1, FLOOR_VH_FUNC_IMPL_2, FLOOR_VH_FUNC_IMPL_2_INT, FLOOR_VH_FUNC_IMPL_3)
FLOOR_VH_IMPL_DEF_INT8(FLOOR_VH_FUNC_IMPL_1, FLOOR_VH_FUNC_IMPL_2, FLOOR_VH_FUNC_IMPL_2_INT, FLOOR_VH_FUNC_IMPL_3)
FLOOR_VH_IMPL_DEF_UINT8(FLOOR_VH_FUNC_IMPL_1, FLOOR_VH_FUNC_IMPL_2, FLOOR_VH_FUNC_IMPL_2_INT, FLOOR_VH_FUNC_IMPL_3)
FLOOR_VH_IMPL_DEF_INT16(FLOOR_VH_FUNC_IMPL_1, FLOOR_VH_FUNC_IMPL_2, FLOOR_VH_FUNC_IMPL_2_INT, FLOOR_VH_FUNC_IMPL_3)
FLOOR_VH_IMPL_DEF_UINT16(FLOOR_VH_FUNC_IMPL_1, FLOOR_VH_FUNC_IMPL_2, FLOOR_VH_FUNC_IMPL_2_INT, FLOOR_VH_FUNC_IMPL_3)
FLOOR_VH_IMPL_DEF_INT64(FLOOR_VH_FUNC_IMPL_1, FLOOR_VH_FUNC_IMPL_2, FLOOR_VH_FUNC_IMPL_2_INT, FLOOR_VH_FUNC_IMPL_3)
FLOOR_VH_IMPL_DEF_UINT64(FLOOR_VH_FUNC_IMPL_1, FLOOR_VH_FUNC_IMPL_2, FLOOR_VH_FUNC_IMPL_2_INT, FLOOR_VH_FUNC_IMPL_3)
#if !defined(FLOOR_COMPUTE) || defined(FLOOR_COMPUTE_HOST)
FLOOR_VH_IMPL_DEF_INT128(FLOOR_VH_FUNC_IMPL_1, FLOOR_VH_FUNC_IMPL_2, FLOOR_VH_FUNC_IMPL_2_INT, FLOOR_VH_FUNC_IMPL_3)
FLOOR_VH_IMPL_DEF_UINT128(FLOOR_VH_FUNC_IMPL_1, FLOOR_VH_FUNC_IMPL_2, FLOOR_VH_FUNC_IMPL_2_INT, FLOOR_VH_FUNC_IMPL_3)
#endif
FLOOR_VH_IMPL_DEF_BOOL(FLOOR_VH_FUNC_IMPL_1, FLOOR_VH_FUNC_IMPL_2, FLOOR_VH_FUNC_IMPL_2_INT, FLOOR_VH_FUNC_IMPL_3)

#if defined(__APPLE__)
FLOOR_VH_IMPL_DEF_SSIZE_T(FLOOR_VH_FUNC_IMPL_1, FLOOR_VH_FUNC_IMPL_2, FLOOR_VH_FUNC_IMPL_2_INT, FLOOR_VH_FUNC_IMPL_3)
#endif
#if defined(__APPLE__) || (defined(FLOOR_COMPUTE_CUDA) && \
						   defined(PLATFORM_X64) && \
						   !(__clang_major__ == 3 && __clang_minor__ < 7))
FLOOR_VH_IMPL_DEF_SIZE_T(FLOOR_VH_FUNC_IMPL_1, FLOOR_VH_FUNC_IMPL_2, FLOOR_VH_FUNC_IMPL_2_INT, FLOOR_VH_FUNC_IMPL_3)
#endif

FLOOR_POP_WARNINGS()

#endif
