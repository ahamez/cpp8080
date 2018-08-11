#pragma once

#include <iomanip>
#include <ostream>

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

  state(Machine& machine)
    : a{0}
    , b{0}
    , c{0}
    , d{0}
    , e{0}
    , h{0}
    , l{0}
    , sp{0}
    , pc{0}
    , cc{}
    , machine_{machine}
    , interrupt_{false}
    , cycles_{0}
  {}

  friend
  std::ostream&
  operator<<(std::ostream& os, const state& state)
  {
    return os
      << std::resetiosflags(std::ios_base::basefield)
      << " "
      << (state.cc.z == 1 ? "z" : ".")
      << (state.cc.s == 1 ? "s" : ".")
      << (state.cc.p == 1 ? "p" : ".")
      << (state.cc.cy == 1 ? "c" : ".")
      << (state.cc.ac == 1 ? "a" : ".")
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
    machine_.write_memory(address, value);
  }

  [[nodiscard]]
  std::uint8_t
  read_memory(std::uint16_t address)
  const
  {
    return machine_.read_memory(address);
  }

  void
  write_hl(std::uint8_t value)
  {
    const auto offset = hl();
    write_memory(offset, value);
  }

  [[nodiscard]]
  std::uint8_t
  read_hl()
  const
  {
    const auto offset = hl();
    return read_memory(offset);
  }

  [[nodiscard]]
  std::uint16_t
  hl()
  const noexcept
  {
    return (h << 8) | l;
  }

  [[nodiscard]]
  std::uint16_t
  bc()
  const noexcept
  {
    return (b << 8) | c;
  }

  [[nodiscard]]
  std::uint16_t
  de()
  const noexcept
  {
    return (d << 8) | e;
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
  arithmetic_flags(std::uint16_t res)
  noexcept
  {
    cc.cy = (res > 0xff);
    cc.z = ((res & 0xff) == 0);
    cc.s = (0x80 == (res & 0x80));
    cc.p = even_parity(res & 0xff);
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
    low = read_memory(sp);
    high = read_memory(sp + 1);
    sp += 2;
  }

  [[nodiscard]]
  std::uint8_t
  op1()
  const noexcept
  {
    return op1_;
  }
  
  [[nodiscard]]
  std::uint8_t
  op2()
  const noexcept
  {
    return op2_;
  }

  void
  fetch_operands()
  {
    op1_ = read_memory(pc + 1);
    op2_ = read_memory(pc + 2);
  }
  
  [[nodiscard]]
  Machine&
  machine()
  noexcept
  {
    return machine_;
  }

  [[nodiscard]]
  const Machine&
  machine()
  const noexcept
  {
    return machine_;
  }

  void
  enable_interrupt()
  noexcept
  {
    interrupt_ = true;
  }

  void
  disable_interrupt()
  noexcept
  {
    interrupt_ = false;
  }

  [[nodiscard]]
  bool
  interrupt_enabled()
  const noexcept
  {
    return interrupt_;
  }

  void
  interrupt(std::uint8_t interrupt)
  {
    push((pc & 0xff00) >> 8, pc & 0x00ff);
    pc = interrupt;
    disable_interrupt();
  }
  
  void
  increment_cycles(std::uint64_t nb_cycles)
  noexcept
  {
    cycles_ += nb_cycles;
  }
  
  [[nodiscard]]
  std::uint64_t
  cycles()
  const noexcept
  {
    return cycles_;
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
  
private:

  Machine& machine_;
  bool interrupt_;
  std::uint64_t cycles_;

  std::uint8_t op1_;
  std::uint8_t op2_;
};

/*------------------------------------------------------------------------------------------------*/

} // namespace cpp8080::specific