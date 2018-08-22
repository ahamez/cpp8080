#pragma once

#include <stdexcept>
#include <type_traits> // enable_if_t
#include <utility>     // as_const

#include "cpp8080/meta/instructions.hh"

namespace cpp8080::meta {

/*------------------------------------------------------------------------------------------------*/

namespace detail {

template<typename Cpu, typename Fn, typename Instruction>
std::uint64_t
execute(Cpu& cpu, Fn&& fn)
{
  fn.pre(std::as_const(cpu), Instruction{});
  const auto cycles = Instruction{}(cpu);
  fn.post(std::as_const(cpu), Instruction{});
  return cycles;
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
  
template <typename Instruction>
struct instruction
{
  static constexpr auto bytes  = Instruction::bytes;
  static constexpr auto cycles = Instruction::cycles;
  static constexpr auto name   = Instruction::name;
  static constexpr auto opcode = Instruction::opcode;

  template <typename Cpu>
  std::uint64_t
  operator()(Cpu& cpu)
  const noexcept(noexcept(Instruction{}(cpu)))
  {
    return impl<Cpu, Instruction>(cpu);
  }

private:

  template <typename Cpu, typename Instruction_>
  std::enable_if_t<Instruction_::constant_instruction::value, std::uint64_t>
  impl(Cpu& cpu)
  const noexcept(noexcept(Instruction{}(cpu)))
  {
    Instruction{}(cpu);
    return Instruction::cycles;
  }

  template <typename Cpu, typename Instruction_>
  std::enable_if_t<not Instruction_::constant_instruction::value, std::uint64_t>
  impl(Cpu& cpu)
  const noexcept(noexcept(Instruction{}(cpu)))
  {
    return Instruction{}(cpu)
         ? Instruction::cycles + Instruction_::constant_instruction::cycles
         : Instruction::cycles;
  }
};

/*------------------------------------------------------------------------------------------------*/

template <bool Constant, std::uint64_t Cycles = 0>
struct constant_instruction
{
  static constexpr auto value = Constant;
  static constexpr auto cycles = Cycles;
};

/*------------------------------------------------------------------------------------------------*/
  
template <
  std::uint8_t Opcode,
  std::uint8_t Cycles,
  std::uint8_t Bytes,
  typename Constant = constant_instruction<true>
  >
struct describe_instruction
{
  static constexpr auto bytes  = Bytes;
  static constexpr auto cycles = Cycles;
  static constexpr auto opcode = Opcode;
  using constant_instruction   = Constant;
};

/*------------------------------------------------------------------------------------------------*/
  
} // namespace cpp8080::meta
