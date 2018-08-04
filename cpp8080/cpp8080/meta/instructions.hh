#pragma once

#include <cstdint>

namespace cpp8080::meta {

/*------------------------------------------------------------------------------------------------*/

template <typename... Is>
struct instructions
{};

/*------------------------------------------------------------------------------------------------*/

namespace detail {
  
template <typename...>
struct prepend_instruction;

template <typename T, typename... Us>
struct prepend_instruction<T, instructions<Us...>>
{
  using type = instructions<T, Us...>;
};

} // namespace detail

/*------------------------------------------------------------------------------------------------*/

namespace detail {
  
// If overridden opcode is not found, a compilation error will occurs.
template <bool SameOpcode, typename Override, typename... Is>
struct override_instruction_impl;

template <typename Override>
struct override_instruction_impl<true, Override, instructions<>>
{
  using type = instructions<Override>;
};

template <typename Override, typename I, typename... Is>
struct override_instruction_impl<true, Override, instructions<I, Is...>>
{
  using type = instructions<Override, Is...>;
};

template <typename Override, typename I, typename... Is>
struct override_instruction_impl<false, Override, instructions<I, Is...>>
{
  using next = typename override_instruction_impl<
    Override::opcode == I::opcode,
    Override,
    instructions<Is...>
  >::type;

  using type = typename prepend_instruction<I, next>::type;
};

template <typename Override, typename I, typename... Is>
struct override_instruction
{
  using type = typename override_instruction_impl<
    Override::opcode == I::opcode,
    Override,
    instructions<I, Is...>
  >::type;
};

} // namespace detail
  
/*------------------------------------------------------------------------------------------------*/
  
namespace detail {
  
template <typename Instructions, typename... Overrides>
struct override_instructions_impl;

template <typename... Is>
struct override_instructions_impl<instructions<Is...>>
{
  using type = instructions<Is...>;
};

template <typename... Is, typename Override, typename... Overrides>
struct override_instructions_impl<instructions<Is...>, Override, Overrides...>
{
  // Apply current override.
  using overrided = typename override_instruction<Override, Is...>::type;

  // Now apply next override.
  using type = typename override_instructions_impl<overrided, Overrides...>::type;
};
  
} // namespace detail
  
template <typename Instructions, typename Overrides>
struct override_instructions;

template <typename... Is, typename... Overrides>
struct override_instructions<instructions<Is...>, instructions<Overrides...>>
{
  using type = typename detail::override_instructions_impl<instructions<Is...>, Overrides...>::type;
};

/*------------------------------------------------------------------------------------------------*/
  
} // namespace cpp8080::meta
