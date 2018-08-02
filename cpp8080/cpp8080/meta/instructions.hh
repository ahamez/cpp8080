#pragma once

#include <cstdint>

namespace cpp8080::meta {

/*------------------------------------------------------------------------------------------------*/

template <typename... Is>
struct instructions
{};

/*------------------------------------------------------------------------------------------------*/

template <typename...>
struct cat;

template <typename T, typename... Us>
struct cat<T, instructions<Us...>>
{
  using type = instructions<T, Us...>;
};

/*------------------------------------------------------------------------------------------------*/
  
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

  using type = typename cat<I, next>::type;
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

/*------------------------------------------------------------------------------------------------*/
  
template <typename Instructions, typename... Overrides>
struct override_instructions;

template <typename... Is>
struct override_instructions<instructions<Is...>>
{
  using type = instructions<Is...>;
};

template <typename... Is, typename Override, typename... Overrides>
struct override_instructions<instructions<Is...>, Override, Overrides...>
{
  // Apply current override.
  using overrided = typename override_instruction<Override, Is...>::type;

  // Now apply next override.
  using type = typename override_instructions<overrided, Overrides...>::type;
};

/*------------------------------------------------------------------------------------------------*/
  
} // namespace cpp8080::meta
