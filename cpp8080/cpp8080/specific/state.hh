#pragma once

#include <algorithm>
#include <iomanip>
#include <memory>
#include <ostream>
#include <vector>

#include "cpp8080/util.hh"

namespace cpp8080::specific {

/*------------------------------------------------------------------------------------------------*/

// Order is important: some code "pops" values into the PSW that they didn't push.
struct condition_codes
{
  std::uint8_t cy:1;
  std::uint8_t _pad:1;
  std::uint8_t p:1;
  std::uint8_t _pad2:1;
  std::uint8_t ac:1;
  std::uint8_t _pad3:1;
  std::uint8_t z:1;
  std::uint8_t s:1;
};

/*------------------------------------------------------------------------------------------------*/

template <typename Machine>
class state
{
public:
  
  template <typename InputIterator>
  state(InputIterator first, InputIterator last, std::shared_ptr<Machine> machine)
    : memory(16384, 0)
    , machine_ptr{machine}
  {
    std::copy(first, last, memory.begin());
  }
  
  friend
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

  void
  write_memory(std::uint16_t address, std::uint8_t value)
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

  [[nodiscard]]
  std::uint8_t
  read_memory(std::uint16_t address)
  const
  {
    return memory.at(address);
  }
  
  void
  write_hl(std::uint8_t value)
  {
    const auto offset = (h << 8) | l;
    write_memory(offset, value);
  }

  [[nodiscard]]
  std::uint8_t
  read_hl()
  const
  {
    const auto offset = (h << 8) | l;
    return memory.at(offset);
  }

  void
  flags_zsp(std::uint8_t value)
  noexcept
  {
    cc.z = (value == 0);
    cc.s = (0x80 == (value & 0x80));
    cc.p = even_parity(value);
  }

  void
  logic_flags_a()
  noexcept
  {
    cc.cy = cc.ac = 0;
    cc.z = (a == 0);
    cc.s = (0x80 == (a & 0x80));
    cc.p = even_parity(a);
  }

  void
  push(std::uint8_t high, std::uint8_t low)
  {
    write_memory(sp - 1, high);
    write_memory(sp - 2, low);
    sp -= 2;
  }

  void
  pop(std::uint8_t& high, std::uint8_t& low)
  {
    low = memory.at(sp);
    high = memory.at(sp + 1);
    sp += 2;
  }

  [[nodiscard]]
  std::uint8_t
  op1()
  const
  {
    return memory.at(pc + 0);
  }
  
  [[nodiscard]]
  std::uint8_t
  op2()
  const
  {
    return memory.at(pc + 1);
  }

  Machine&
  machine()
  noexcept
  {
    return *machine_ptr;
  }

  const Machine&
  machine()
  const noexcept
  {
    return *machine_ptr;
  }

public:

  std::uint8_t a;
  std::uint8_t b;
  std::uint8_t c;
  std::uint8_t d;
  std::uint8_t e;
  std::uint8_t h;
  std::uint8_t l;
  std::uint16_t sp;
  std::uint16_t pc;
  condition_codes cc;
  std::uint8_t int_enable;
  
  std::uint64_t cycles;
  std::shared_ptr<Machine> machine_ptr;
  
private:

  std::vector<std::uint8_t> memory;
};

/*------------------------------------------------------------------------------------------------*/

} // namespace cpp8080::specific
