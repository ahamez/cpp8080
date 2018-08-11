#pragma once

#include <ostream>

namespace cpp8080::meta {

/*------------------------------------------------------------------------------------------------*/
  
namespace detail {
  
template <typename State, typename Instruction, std::uint8_t Bytes>
struct disassemble_impl;
  
template <typename State, typename Instruction>
struct disassemble_impl<State, Instruction, 1>
{
  void
  operator()(std::ostream& os, const State&)
  const noexcept
  {
    os << Instruction::name;
  }
};
  
template <typename State, typename Instruction>
struct disassemble_impl<State, Instruction, 2>
{
  void
  operator()(std::ostream& os, const State& state)
  const
  {
    os << Instruction::name << " " << std::hex << "0x" << +state.op1();
  }
};
  
template <typename State, typename Instruction>
struct disassemble_impl<State, Instruction, 3>
{
  void
  operator()(std::ostream& os, const State& state)
  const
  {
    os << Instruction::name << " " << std::hex << "0x" << +state.op2() << " 0x" <<+state.op1();
  }
};

/*------------------------------------------------------------------------------------------------*/

template <typename State, typename Instruction>
struct disassemble_dispatch
{
  const State& state;
  
  friend
  std::ostream&
  operator<<(std::ostream& os, const disassemble_dispatch& d)
  {
    disassemble_impl<State, Instruction, Instruction::bytes>{}(os, d.state);
    return os;
  }
};

} // namespace detail
    
/*------------------------------------------------------------------------------------------------*/
    
template <typename State, typename Instruction>
auto
disassemble(const State& state, Instruction)
noexcept
{
  return detail::disassemble_dispatch<State, Instruction>{state};
}
    
/*------------------------------------------------------------------------------------------------*/
    
} // namespace cpp8080::meta
