#pragma once

#include <cstdint>

namespace cpp8080::meta {

/*------------------------------------------------------------------------------------------------*/
  
template <typename... Is>
struct instructions
{};

/*------------------------------------------------------------------------------------------------*/
  
// TODO Is it possible to use C++17 fold expressions?

//template <bool, typename, typename, typename...>
//struct override_instruction_impl;
//
////template <std::uint8_t Opcode, typename Override>
////struct override_instruction_impl<Opcode, Override>;
//
//template <typename Override, typename I, typename... Is>
//struct override_instruction_impl<true, Override, I, instructions<Is...>>
//{
//
//};
//
//template <typename Override, typename Current, typename I, typename... Is>
//struct override_instruction_impl<false, Override, Current, I, instructions<Is...>>
//{
//  using result = instructions<Current, typename override_instruction_impl<Override::opcode == I::opcode, Override, I, Is...>::type;
//
//  using type = typename concat<...>::type;
//};
//
//template <typename Override, typename I, typename... Is>
//struct override_instruction
//{
//  using type = typename override_instruction_impl<Override::opcode == I::opcode, Override, I, instructions<Is...>>::type;
//};
  
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
//  using overrided = typename override_instruction<Override, Is...>::type;
//  using type = typename override_instructions<overrided, Overrides...>::type;

  using type = instructions<Is...>;
};

/*------------------------------------------------------------------------------------------------*/
  
} // namespace cpp8080::meta
