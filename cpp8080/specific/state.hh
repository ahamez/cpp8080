#pragma once

#include <iomanip>
#include <ostream>

#include "cpp8080/specific/instructions.hh"
#include "cpp8080/specific/state_fwd.hh"
#include "cpp8080/util/parity.hh"

namespace cpp8080::specific {

/*------------------------------------------------------------------------------------------------*/

struct flag_bits
{
  bool cy;
  bool p;
  bool ac;
  bool z;
  bool s;
};

/*------------------------------------------------------------------------------------------------*/

template <typename Machine>
class state
{
private:

  using instructions = meta::override_instructions<
    instructions_8080,
    typename Machine::overrides
  >;

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
    , flags{}
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
      << (state.flags.z ? "z" : ".")
      << (state.flags.s ? "s" : ".")
      << (state.flags.p ? "p" : ".")
      << (state.flags.cy ? "c" : ".")
      << (state.flags.ac ? "a" : ".")
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

  std::uint64_t
  step()
  {
    const auto opcode = read_memory(pc);
    return meta::step(instructions{}, opcode, *this);
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
    if (interrupt_enabled())
    {
      push((pc & 0xff00) >> 8, pc & 0x00ff);
      pc = interrupt;
      disable_interrupt();
    }
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

  [[nodiscard]]
  std::uint8_t
  inr(std::uint8_t value)
  noexcept
  {
    const std::uint8_t res = value + 1;
    flags.ac = (res & 0x0f) == 0;
    flags.z = res == 0;
    flags.s = (res & 0b10000000) != 0;
    flags.p = util::parity(res);
    return res;
  }

  [[nodiscard]]
  std::uint8_t
  dcr(std::uint8_t value)
  noexcept
  {
    const std::uint8_t res = value - 1;
    flags.ac = not ((res & 0x0f) == 0x0f);
    flags.z = res == 0;
    flags.s = (res & 0b10000000) != 0;
    flags.p = util::parity(res);
    return res;
  }

  void
  add(std::uint8_t& reg, std::uint8_t val, bool carry)
  noexcept
  {
    const std::uint16_t res = reg + val + carry;
    flags.z  = (res & 0xff) == 0;
    flags.s  = (res & 0b10000000) != 0;
    flags.cy = (res & 0b100000000) != 0;
    flags.ac = (reg ^ res ^ val) & 0x10;
    flags.p  = util::parity(res & 0xff);
    reg = res & 0xff;
  }

  void
  sub(std::uint8_t& reg, std::uint8_t val, bool carry)
  noexcept
  {
    const std::int16_t res = reg - val - carry;
    flags.z = (res & 0xff) == 0;
    flags.s = (res & 0b10000000) != 0;
    flags.cy = (res & 0b100000000) != 0;
    flags.ac = ~(reg ^ res ^ val) & 0x10;
    flags.p = util::parity(res & 0xff);
    reg = res & 0xff;
  }

  void
  ora(std::uint8_t val)
  noexcept
  {
    a |= val;
    flags.cy = false;
    flags.ac = false;
    flags.z = a == 0;
    flags.s = (a & 0x80) != 0;
    flags.p = util::parity(a);
  }

  void
  ana(std::uint8_t val)
  noexcept
  {
    const std::uint8_t res = a & val;
    flags.cy = false;
    flags.ac = ((a | val) & 0x08) != 0;
    flags.z = res == 0;
    flags.s = (res & 0x80) != 0;
    flags.p = util::parity(res);
    a = res;
  }

  void
  xra(std::uint8_t val)
  noexcept
  {
    a ^= val;
    flags.cy = false;
    flags.ac = false;
    flags.z = a == 0;
    flags.s = (a & 0x80) != 0;
    flags.p = util::parity(a);
  }

  void
  cmp(std::uint8_t val)
  noexcept
  {
    const std::int16_t res = a - val;
    flags.cy = (res & 0b100000000) != 0;
    flags.ac = ~(a ^ res ^ val) & 0x10;
    flags.z = (res & 0xff) == 0;
    flags.s = (res & 0x80) != 0;
    flags.p = util::parity(res);
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
  flag_bits flags;
  
private:

  Machine& machine_;
  bool interrupt_;
  std::uint64_t cycles_;

  std::uint8_t op1_;
  std::uint8_t op2_;
};

/*------------------------------------------------------------------------------------------------*/

} // namespace cpp8080::specific
