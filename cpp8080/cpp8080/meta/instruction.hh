#pragma once

#include <ostream>
#include <stdexcept>

#include "cpp8080/meta/disassemble.hh"
#include "cpp8080/meta/instructions.hh"

namespace cpp8080::meta {

/*------------------------------------------------------------------------------------------------*/
  
template<typename State, typename Instruction>
void
execute(State& state)
noexcept(noexcept(Instruction{}(state)))
{
  Instruction{}(state);
}

/*------------------------------------------------------------------------------------------------*/
  
template<typename State, typename Instruction>
void
execute_verbose(State& state, std::ostream& os)
noexcept(noexcept(execute<State, Instruction>(state)))
{
  disassemble<State, Instruction, Instruction::bytes>{}(os, state);
  os << '\n';
  execute<State, Instruction>(state);
}

/*------------------------------------------------------------------------------------------------*/
  
template <typename State, typename... Instructions>
void
step(instructions<Instructions...>, std::uint8_t opcode, State& state)
{
  if (opcode >= sizeof...(Instructions))
  {
    throw std::runtime_error{"Invalid opcode"};
  }
  
  using fun_ptr_type = void (*) (State&);
  static constexpr fun_ptr_type jump_table[] = {&execute<State, Instructions>...};
  jump_table[opcode](state);
}

/*------------------------------------------------------------------------------------------------*/
  
template <typename State, typename... Instructions>
void
step(instructions<Instructions...>, std::uint8_t opcode, State& state, std::ostream& os)
{
  if (opcode >= sizeof...(Instructions))
  {
    throw std::runtime_error{"Invalid opcode"};
  }
  
  using fun_ptr_type = void (*) (State&, std::ostream&);
  static constexpr fun_ptr_type jump_table[] = {&execute_verbose<State, Instructions>...};
  jump_table[opcode](state, os);
}

/*------------------------------------------------------------------------------------------------*/
  
template <typename Instruction>
struct instruction
{
  static constexpr auto opcode = Instruction::opcode;
  static constexpr auto name   = Instruction::name;
  static constexpr auto bytes  = Instruction::bytes;

  template <typename State>
  void
  operator()(State& state)
  const noexcept(noexcept(Instruction{}(state)))
  {
    state.cycles += Instruction::cycles;
    Instruction{}(state);
  }
};

/*------------------------------------------------------------------------------------------------*/
  
template <std::uint8_t Opcode, std::uint8_t Cycles, std::uint8_t Bytes>
struct describe_instruction
{
  static constexpr auto opcode = Opcode;
  static constexpr auto cycles = Cycles;
  static constexpr auto bytes  = Bytes;
};

/*------------------------------------------------------------------------------------------------*/
  
} // namespace cpp8080::meta
