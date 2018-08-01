#include "cpp8080/specific/state.hh"

/*------------------------------------------------------------------------------------------------*/

namespace {

[[nodiscard]] bool
even_parity(std::uint8_t x)
noexcept
{
  x ^= x >> 4;
  x ^= x >> 2;
  x ^= x >> 1;
  return (~x) & 1;
}

} // end unnamed namespace
  
namespace cpp8080::specific {

/*------------------------------------------------------------------------------------------------*/

std::ostream&
operator<<(std::ostream& os, const state& state)
{
  return os
    << std::hex
    << std::setfill('0') << std::setw(4)
    << state.pc
    << std::resetiosflags(std::ios_base::basefield)
    << " "
    << (state.cc.z == 1 ? "z" : ".")
    << (state.cc.s == 1 ? "s" : ".")
    << (state.cc.p == 1 ? "p" : ".")
    << (state.cc.cy == 1 ? "cy" : ".")
    << (state.cc.ac == 1 ? "ac" : ".")
    << std::hex
    << "  A $" << std::setfill('0') << std::setw(2) << +state.a
    << " B $"  << std::setfill('0') << std::setw(2) << +state.b
    << " C $"  << std::setfill('0') << std::setw(2) << +state.c
    << " D $"  << std::setfill('0') << std::setw(2) << +state.d
    << " E $"  << std::setfill('0') << std::setw(2) << +state.e
    << " H $"  << std::setfill('0') << std::setw(2) << +state.h
    << " L $"  << std::setfill('0') << std::setw(2) << +state.l
    << " SP "  << std::setfill('0') << std::setw(4) << +state.sp
    ;
}

/*------------------------------------------------------------------------------------------------*/
  
void
state::write_memory(std::uint16_t address, std::uint8_t value)
{
  if (address < 0x2000)
  {
    throw std::runtime_error{"Attempt to write ROM"};
  }

  if (address >= 0x4000)
  {
    throw std::runtime_error{"Attempt to write outside of RAM"};
  }

  memory[address] = value;
}

/*------------------------------------------------------------------------------------------------*/
  
std::uint8_t
state::read_memory(std::uint16_t address)
const
{
  return memory.at(address);
}
  
/*------------------------------------------------------------------------------------------------*/
  
void
state::write_hl(std::uint8_t value)
{
  const auto offset = (h << 8) | l;
  write_memory(offset, value);
}

/*------------------------------------------------------------------------------------------------*/
  
std::uint8_t
state::read_hl()
const
{
  const auto offset = (h << 8) | l;
  return memory.at(offset);
}

/*------------------------------------------------------------------------------------------------*/
  
void
state::flags_zsp(std::uint8_t value)
noexcept
{
  cc.z = (value == 0);
  cc.s = (0x80 == (value & 0x80));
  cc.p = even_parity(value);
}

/*------------------------------------------------------------------------------------------------*/
  
void
state::logic_flags_a()
noexcept
{
  cc.cy = cc.ac = 0;
  cc.z = (a == 0);
  cc.s = (0x80 == (a & 0x80));
  cc.p = even_parity(a);
}

/*------------------------------------------------------------------------------------------------*/
  
void
state::push(std::uint8_t high, std::uint8_t low)
{
  write_memory(sp - 1, high);
  write_memory(sp - 2, low);
  sp -= 2;
}

/*------------------------------------------------------------------------------------------------*/
  
void
state::pop(std::uint8_t& high, std::uint8_t& low)
{
  low = memory.at(sp);
  high = memory.at(sp + 1);
  sp += 2;
}

/*------------------------------------------------------------------------------------------------*/
  
std::uint8_t
state::op1()
const
{
  return memory.at(pc + 0);
}

/*------------------------------------------------------------------------------------------------*/
  
std::uint8_t
state::op2()
const
{
  return memory.at(pc + 1);
}

/*------------------------------------------------------------------------------------------------*/

} // namespace cpp8080::specific
