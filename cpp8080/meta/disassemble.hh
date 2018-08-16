#pragma once

#include <ostream>

namespace cpp8080::meta {

/*------------------------------------------------------------------------------------------------*/
  
namespace detail {
  
template <typename Cpu, typename Instruction, std::uint8_t Bytes>
struct disassemble_impl;
  
template <typename Cpu, typename Instruction>
struct disassemble_impl<Cpu, Instruction, 1>
{
  void
  operator()(std::ostream& os, const Cpu&)
  const noexcept
  {
    os << Instruction::name;
  }
};
  
template <typename Cpu, typename Instruction>
struct disassemble_impl<Cpu, Instruction, 2>
{
  void
  operator()(std::ostream& os, const Cpu& cpu)
  const
  {
    os << Instruction::name << " " << std::hex << "0x" << +cpu.op1();
  }
};
  
template <typename Cpu, typename Instruction>
struct disassemble_impl<Cpu, Instruction, 3>
{
  void
  operator()(std::ostream& os, const Cpu& cpu)
  const
  {
    os << Instruction::name << " " << std::hex << "0x" << +cpu.op2() << " 0x" <<+cpu.op1();
  }
};

/*------------------------------------------------------------------------------------------------*/

template <typename Cpu, typename Instruction>
struct disassemble_dispatch
{
  const Cpu& cpu;
  
  friend
  std::ostream&
  operator<<(std::ostream& os, const disassemble_dispatch& d)
  {
    disassemble_impl<Cpu, Instruction, Instruction::bytes>{}(os, d.cpu);
    return os;
  }
};

} // namespace detail
    
/*------------------------------------------------------------------------------------------------*/
    
template <typename Cpu, typename Instruction>
auto
disassemble(const Cpu& cpu, Instruction)
noexcept
{
  return detail::disassemble_dispatch<Cpu, Instruction>{cpu};
}
    
/*------------------------------------------------------------------------------------------------*/
    
} // namespace cpp8080::meta
