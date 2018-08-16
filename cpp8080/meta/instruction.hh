#pragma once

#include <stdexcept>
#include <utility> // as_const

#include "cpp8080/meta/instructions.hh"

namespace cpp8080::meta {

/*------------------------------------------------------------------------------------------------*/

namespace detail {
  
template<typename Cpu, typename Fn, typename Instruction>
std::uint64_t
execute(Cpu& cpu, Fn&& fn)
{
  cpu.fetch_operands();
  fn.pre(std::as_const(cpu), Instruction{});
  cpu.pc += 1;
  Instruction{}(cpu);
  fn.post(std::as_const(cpu), Instruction{});
  // TODO The number of cycles is not fixed for conditional instructions.
  return Instruction::cycles;
}

} // namespace detail
  
template <typename Cpu, typename Fn, typename... Instructions>
std::uint64_t
step(instructions<Instructions...>, std::uint8_t opcode, Cpu& cpu, Fn&& fn)
{
  if (opcode >= sizeof...(Instructions))
  {
    throw std::runtime_error{"Invalid opcode"};
  }
  
  using fun_ptr_type = std::uint64_t (*) (Cpu&, Fn&&);
  static constexpr fun_ptr_type jump_table[] = {&detail::execute<Cpu, Fn, Instructions>...};
  return jump_table[opcode](cpu, std::forward<Fn>(fn));
}

/*------------------------------------------------------------------------------------------------*/

namespace detail {

struct dummy_hook
{
  template <typename Cpu, typename Instruction>
  void pre(const Cpu&, Instruction) const noexcept
  {}
  
  template <typename Cpu, typename Instruction>
  void post(const Cpu&, Instruction) const noexcept
  {}
};
  
} // namespace detail
  
template <typename Cpu, typename... Instructions>
std::uint64_t
step(instructions<Instructions...>, std::uint8_t opcode, Cpu& cpu)
{
  return step(instructions<Instructions...>{}, opcode, cpu, detail::dummy_hook{});
}

/*------------------------------------------------------------------------------------------------*/
  
template <typename Instruction>
struct instruction
{
  static constexpr auto bytes  = Instruction::bytes;
  static constexpr auto cycles = Instruction::cycles;
  static constexpr auto name   = Instruction::name;
  static constexpr auto opcode = Instruction::opcode;

  template <typename Cpu>
  void
  operator()(Cpu& cpu)
  const noexcept(noexcept(Instruction{}(cpu)))
  {
    cpu.increment_cycles(Instruction::cycles);
    Instruction{}(cpu);
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
