#pragma once

#include <stdexcept>
#include <utility> // as_const

#include "cpp8080/meta/instructions.hh"

namespace cpp8080::meta {

/*------------------------------------------------------------------------------------------------*/

namespace detail {
  
template<typename State, typename Fn, typename Instruction>
std::uint64_t
execute(State& state, Fn&& fn)
{
  state.fetch_operands();
  fn.pre(std::as_const(state), Instruction{});
  state.pc += 1;
  Instruction{}(state);
  fn.post(std::as_const(state), Instruction{});
  return Instruction::cycles;
}

} // namespace detail
  
template <typename State, typename Fn, typename... Instructions>
std::uint64_t
step(instructions<Instructions...>, std::uint8_t opcode, State& state, Fn&& fn)
{
  if (opcode >= sizeof...(Instructions))
  {
    throw std::runtime_error{"Invalid opcode"};
  }
  
  using fun_ptr_type = std::uint64_t (*) (State&, Fn&&);
  static constexpr fun_ptr_type jump_table[] = {&detail::execute<State, Fn, Instructions>...};
  return jump_table[opcode](state, std::forward<Fn>(fn));
}

/*------------------------------------------------------------------------------------------------*/

namespace detail {

struct dummy_hook
{
  template <typename State, typename Instruction>
  void pre(const State&, Instruction) const noexcept
  {}
  
  template <typename State, typename Instruction>
  void post(const State&, Instruction) const noexcept
  {}
};
  
} // namespace detail
  
template <typename State, typename... Instructions>
std::uint64_t
step(instructions<Instructions...>, std::uint8_t opcode, State& state)
{
  return step(instructions<Instructions...>{}, opcode, state, detail::dummy_hook{});
}

/*------------------------------------------------------------------------------------------------*/
  
template <typename Instruction>
struct instruction
{
  static constexpr auto bytes  = Instruction::bytes;
  static constexpr auto cycles = Instruction::cycles;
  static constexpr auto name   = Instruction::name;
  static constexpr auto opcode = Instruction::opcode;

  template <typename State>
  void
  operator()(State& state)
  const noexcept(noexcept(Instruction{}(state)))
  {
    state.increment_cycles(Instruction::cycles);
    Instruction{}(state);
  }
};

/*------------------------------------------------------------------------------------------------*/
  
template <std::uint8_t Opcode, std::uint8_t Cycles, std::uint8_t Bytes>
struct describe_instruction
{
  static constexpr auto bytes  = Bytes;
  static constexpr auto cycles = Cycles;
  static constexpr auto opcode = Opcode;
};

/*------------------------------------------------------------------------------------------------*/
  
} // namespace cpp8080::meta
