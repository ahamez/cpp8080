#pragma once

#include <ostream>

namespace cpp8080::meta {

/*------------------------------------------------------------------------------------------------*/
  
template <typename State, typename Instruction, std::uint8_t Bytes>
struct disassemble;
  
template <typename State, typename Instruction>
struct disassemble<State, Instruction, 1>
{
  void
  operator()(std::ostream& os, const State&)
  const noexcept
  {
    os << Instruction::name;
  }
};
  
template <typename State, typename Instruction>
struct disassemble<State, Instruction, 2>
{
  void
  operator()(std::ostream& os, const State& state)
  const
  {
    os << Instruction::name << " " << std::hex << "0x" << +state.op1();
  }
};
  
template <typename State, typename Instruction>
struct disassemble<State, Instruction, 3>
{
  void
  operator()(std::ostream& os, const State& state)
  const
  {
    os << Instruction::name << " " << std::hex << "0x" << +state.op1() << " 0x" << +state.op2();
  }
};

/*------------------------------------------------------------------------------------------------*/
  
} // namespace cpp8080::meta
