/*_  __          _      __  __          _   _
 | |/ /    /\   | |    |  \/  |   /\   | \ | |
 | ' /    /  \  | |    | \  / |  /  \  |  \| |
 |  <    / /\ \ | |    | |\/| | / /\ \ | . ` |
 | . \  / ____ \| |____| |  | |/ ____ \| |\  |
 |_|\_\/_/    \_\______|_|  |_/_/    \_\_| \_|

Kalman Filter for C++
Version 0.1.0
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

#ifndef FCAROUGE_KALMAN_HPP
#define FCAROUGE_KALMAN_HPP

//! @file
//! @brief The main Kalman filter class.

#include "kalman_equation.hpp"
#include "kalman_operator.hpp"

#include <type_traits>

namespace fcarouge
{
//! @brief Kalman filter.
//!
//! @tparam State The type template parameter of the state vector x.
//! @tparam Output The type template parameter of the measurement vector z.
//! @tparam Input The type template parameter of the control u.
//! @tparam Transpose The template template parameter of the transpose functor.
//! @tparam Divide The template template parameter of the division functor.
//! @tparam Identity The template template parameter of the identity functor.
//! @tparam PredictionArguments The variadic type template parameter for
//! additional prediction function parameters. Time, or a delta thereof, is
//! often a prediction parameter. The parameters are propagated to the function
//! objects used to compute the process noise Q, the state transition F, and the
//! control transition G matrices.
template <typename State, typename Output = State, typename Input = State,
          template <typename> typename Transpose = transpose,
          template <typename> typename Symmetrize = symmetrize,
          template <typename, typename> typename Divide = divide,
          template <typename> typename Identity = identity,
          typename... PredictionArguments>
class kalman
{
  public:
  //! @name Public Member Types
  //! @{

  //! @brief Type of the state estimate vector x.
  using state = State;

  //! @brief Type of the observation vector z, or y.
  using output = Output;

  //! @brief Type of the control vector.
  using input = Input;

  //! @brief Type of the estimated covariance matrix.
  using estimate_uncertainty =
      std::invoke_result_t<Divide<State, State>, State, State>;

  //! @brief Type of the process noise covariance matrix.
  using process_noise_uncertainty =
      std::invoke_result_t<Divide<State, State>, State, State>;

  //! @brief Type of the observation noise covariance matrix.
  using observation_noise_uncertainty =
      std::invoke_result_t<Divide<Output, Output>, Output, Output>;

  //! @brief Type of the state transition matrix.
  using state_transition =
      std::invoke_result_t<Divide<State, State>, State, State>;

  //! @brief Type of the observation transition matrix.
  using observation =
      std::invoke_result_t<Divide<Output, State>, Output, State>;

  //! @brief Type of the control transition matrix G, or B.
  using control = std::invoke_result_t<Divide<State, Input>, State, Input>;

  //! @}

  //! @name Public Member Variables
  //! @{

  //! @brief The state estimate vector x.
  state state_x;

  //! @brief The estimate uncertainty, covariance matrix P.
  //!
  //! @details The estimate uncertainty, covariance is also known as Σ.
  estimate_uncertainty estimate_uncertainty_p;

  //! @}

  //! @name Public Member Function Objects
  //! @{

  // Functors could be replaced by variables if data is constant.
  // Functors could be replaced by the standard general-purpose polymorphic
  // function wrappers `std::function` if lambda captures were needed.

  //! @brief Compute observation transition H matrix.
  //!
  //! @details The observation transition H is also known as C.
  observation (*transition_observation_h)() = [] {
    return observation{ Identity<observation>()() };
  };

  //! @brief Compute observation noise R matrix.
  observation_noise_uncertainty (*noise_observation_r)() = [] {
    return observation_noise_uncertainty{};
  };

  //! @brief Compute state transition F matrix.
  //!
  //! @details The state transition F matrix is also known as Φ or A.
  state_transition (*transition_state_f)(const PredictionArguments &...) =
      [](const PredictionArguments &...arguments) {
        static_cast<void>((arguments, ...));
        return state_transition{ Identity<state_transition>()() };
      };

  //! @brief Compute process noise Q matrix.
  process_noise_uncertainty (*noise_process_q)(const PredictionArguments &...) =
      [](const PredictionArguments &...arguments) {
        static_cast<void>((arguments, ...));
        return process_noise_uncertainty{};
      };

  //! @brief Compute control transition G matrix.
  control (*transition_control_g)(const PredictionArguments &...) =
      [](const PredictionArguments &...arguments) {
        static_cast<void>((arguments, ...));
        return control{};
      };

  //! @}

  //! @name Public Member Functions
  //! @{

  template <typename... Outputs>
  inline constexpr void observe(const Outputs &...output_z)
  {
    auto &x{ state_x };
    auto &p{ estimate_uncertainty_p };
    const auto h{ transition_observation_h() };
    const auto r{ noise_observation_r() };
    const auto z{ output{ output_z... } };
    fcarouge::observe<Transpose, Symmetrize, Divide, Identity>(x, p, h, r, z);
  }

  template <typename... Inputs>
  inline constexpr void predict(const PredictionArguments &...arguments,
                                const Inputs &...input_u)
  {
    auto &x{ state_x };
    auto &p{ estimate_uncertainty_p };
    const auto f{ transition_state_f(arguments...) };
    const auto q{ noise_process_q(arguments...) };
    const auto g{ transition_control_g(arguments...) };
    const auto u{ input{ input_u... } };
    fcarouge::predict<Transpose, Symmetrize>(x, p, f, q, g, u);
  }

  inline constexpr void
  predict(const PredictionArguments &...arguments) requires(
      std::is_same_v<Input, void>)
  {
    auto &x{ state_x };
    auto &p{ estimate_uncertainty_p };
    const auto f{ transition_state_f(arguments...) };
    const auto q{ noise_process_q(arguments...) };
    fcarouge::predict<Transpose, Symmetrize>(x, p, f, q);
  }

  //! @}
};

} // namespace fcarouge

#endif // FCAROUGE_KALMAN_HPP
