/*  __          _      __  __          _   _
| |/ /    /\   | |    |  \/  |   /\   | \ | |
| ' /    /  \  | |    | \  / |  /  \  |  \| |
|  <    / /\ \ | |    | |\/| | / /\ \ | . ` |
| . \  / ____ \| |____| |  | |/ ____ \| |\  |
|_|\_\/_/    \_\______|_|  |_/_/    \_\_| \_|

Kalman Filter
Version 0.3.0
https://github.com/FrancoisCarouge/Kalman

SPDX-License-Identifier: Unlicense

This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <https://unlicense.org> */

#ifndef FCAROUGE_UTILITY_HPP
#define FCAROUGE_UTILITY_HPP

//! @file
//! @brief The collection of utilities supporting the library
//!
//! @details Definitions and documentation of supporting concepts and types.

#include "internal/utility.hpp"

namespace fcarouge {
//! @name Concepts
//! @{
//! @brief Arithmetic concept.
//!
//! @details Any integer or floating point type.
template <typename Type>
concept arithmetic = internal::arithmetic<Type>;

//! @brief Algebraic concept.
//!
//! @details Not an arithmetic type.
//!
//! @todo Is the implementation and definition of an algebraic concept poor,
//! incorrect, or incomplete?
template <typename Type>
concept algebraic = internal::algebraic<Type>;

//! @brief Eigen3 algebraic concept.
//!
//! @details A third party Eigen3 algebraic concept.
template <typename Type>
concept eigen = internal::eigen<Type>;
//! @}

//! @name Types
//! @{
//! @brief Tuple-like pack type.
//!
//! @details An alternative to tuple-like types.
template <typename... Types> using pack = internal::pack<Types...>;

//! @brief Tuple-like empty pack type.
//!
//! @details A `pack` type with no composed types.
using empty_pack = internal::empty_pack;

//! @brief Unpack the first type of the type template parameter pack.
template <typename... Types> using first_t = internal::first_t<Types...>;

//! @brief Unpack the first value of the non-type template parameter pack.
template <auto... Values>
inline constexpr auto first_v{internal::first_v<Values...>};

//! @brief The matrix type satisfying `X * Row = Column`.
//!
//! @details The resulting type of a matrix division. The resulting matrix type
//! has as many rows as the `Row` matrix, respectively for columns as the
//! `Column` matrix.
template <typename Numerator, typename Denominator>
using quotient = internal::quotient<Numerator, Denominator>;
//! @}

//! @name Functions
//! @{
//! @brief A user-definable algebraic division solution.
//!
//! @details Implemented for known libraries. User-definable in other cases.
//! There exists several ways to find  `X` in  `X = lhs * rhs^-1` for different
//! tradeoffs. The user provides their implementation. Often, matrix inversion
//! is avoided by solving `X * rhs = lhs` for `rhs` through a decomposer.
template <typename Numerator, algebraic Denominator>
constexpr auto operator/(const Numerator &lhs, const Denominator &rhs)
    -> quotient<Numerator, Denominator>;
//! @}

//! @name Algebraic Named Values
//! @{
//! @brief The identity matrix.
//!
//! @details User-defined.
template <typename Type = double>
inline constexpr Type identity_v{internal::not_implemented<Type>{
    "Implement the linear algebra identity matrix for this type."}};

//! @brief The singleton identity matrix specialization.
template <arithmetic Arithmetic>
inline constexpr Arithmetic identity_v<Arithmetic>{1};

//! @brief The zero matrix.
//!
//! @details User-defined.
template <typename Type = double>
inline constexpr Type zero_v{internal::not_implemented<Type>{
    "Implement the linear algebra zero matrix for this type."}};

//! @brief The singleton zero matrix specialization.
template <arithmetic Arithmetic>
inline constexpr Arithmetic zero_v<Arithmetic>{0};
//! @}
} // namespace fcarouge

#endif // FCAROUGE_UTILITY_HPP
