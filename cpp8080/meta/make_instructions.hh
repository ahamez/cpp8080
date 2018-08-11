#pragma once

#include "cpp8080/meta/instruction.hh"
#include "cpp8080/meta/instructions.hh"

namespace cpp8080::meta {

/*------------------------------------------------------------------------------------------------*/

namespace detail {
  
template <typename...>
struct make_instructions_impl;
  
template <typename T>
struct make_instructions_impl<T>
{
  using type = instructions<meta::instruction<T>>;
};

template <typename T, typename... Ts>
struct make_instructions_impl<T, Ts...>
{
  using next = typename make_instructions_impl<Ts...>::type;
  using type = typename prepend_instruction<instruction<T>, next>::type;
};

} // namespace detail

/*------------------------------------------------------------------------------------------------*/
  
template <typename... Ts>
using make_instructions = typename detail::make_instructions_impl<Ts...>::type;
  
/*------------------------------------------------------------------------------------------------*/
  
} // namespace cpp8080::meta
