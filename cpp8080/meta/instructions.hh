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
  
// If overridden opcode is not found, a compilation error will occur.
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

template <typename Override, typename Current, typename I, typename... Is>
struct override_instruction_impl<false, Override, instructions<Current, I, Is...>>
{
  using next = typename override_instruction_impl<
    Override::opcode == I::opcode,
    Override,
    instructions<I, Is...>
  >::type;

  using type = typename prepend_instruction<Current, next>::type;
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
struct override_instructions_impl<instructions<Is...>, instructions<>>
{
  using type = instructions<Is...>;
};

template <typename... Is, typename Override, typename... Overrides>
struct override_instructions_impl<instructions<Is...>, instructions<Override, Overrides...>>
{
  // Apply current override.
  using overrided = typename override_instruction<Override, Is...>::type;

  // Now apply next override.
  using type = typename override_instructions_impl<overrided, instructions<Overrides...>>::type;
};
  
} // namespace detail

/*------------------------------------------------------------------------------------------------*/

template <typename Instructions, typename Overrides>
using override_instructions =
  typename detail::override_instructions_impl<Instructions, Overrides>::type;
  
/*------------------------------------------------------------------------------------------------*/
  
} // namespace cpp8080::meta
