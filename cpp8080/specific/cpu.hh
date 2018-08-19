#pragma once

#include <iomanip>
#include <ostream>
#include <tuple>

#include "cpp8080/specific/instructions.hh"
#include "cpp8080/specific/cpu_fwd.hh"
#include "cpp8080/util/parity.hh"
#include "cpp8080/util/verbose.hh"

namespace cpp8080::specific {

/*------------------------------------------------------------------------------------------------*/

template <typename Machine>
class cpu
{
private:

  struct flag_bits
  {
    bool cy;
    bool p;
    bool ac;
    bool z;
    bool s;
  };

  using instructions = meta::override_instructions<
    instructions_8080,
    typename Machine::overrides
  >;

public:

  cpu(Machine& machine)
    : a{0}
    , b{0}
    , c{0}
    , d{0}
    , e{0}
    , h{0}
    , l{0}
    , sp{0}
    , flags{}
    , machine_{machine}
    , interrupt_{false}
    , cycles_{0}
    , pc_{}
  {}

  friend
  std::ostream&
  operator<<(std::ostream& os, const cpu& cpu)
  {
    return os
      << std::resetiosflags(std::ios_base::basefield)
      << " "
      << (cpu.flags.z ? "z" : ".")
      << (cpu.flags.s ? "s" : ".")
      << (cpu.flags.p ? "p" : ".")
      << (cpu.flags.cy ? "c" : ".")
      << (cpu.flags.ac ? "a" : ".")
      << std::hex
      << "  A $" << std::setfill('0') << std::setw(2) << +cpu.a
      << " B $"  << std::setfill('0') << std::setw(2) << +cpu.b
      << " C $"  << std::setfill('0') << std::setw(2) << +cpu.c
      << " D $"  << std::setfill('0') << std::setw(2) << +cpu.d
      << " E $"  << std::setfill('0') << std::setw(2) << +cpu.e
      << " H $"  << std::setfill('0') << std::setw(2) << +cpu.h
      << " L $"  << std::setfill('0') << std::setw(2) << +cpu.l
      << " SP "  << std::setfill('0') << std::setw(4) << +cpu.sp
      ;
  }

  std::uint64_t
  step()
  {
    const auto opcode = memory_read_byte(pc_);
    pc_ += 1;
    const auto cycles = meta::step(instructions{}, opcode, *this);
    increment_cycles(cycles);
    return cycles;
  }

  std::uint64_t
  step(std::ostream& os)
  {
    const auto opcode = memory_read_byte(pc_);
    pc_ += 1;
    const auto cycles = meta::step(instructions{}, opcode, *this, cpp8080::util::verbose{os});
    increment_cycles(cycles);
    return cycles;
  }

  void
  memory_write_byte(std::uint16_t address, std::uint8_t value)
  {
    machine_.memory_write_byte(address, value);
  }

  [[nodiscard]]
  std::uint8_t
  memory_read_byte(std::uint16_t address)
  const
  {
    return machine_.memory_read_byte(address);
  }

  void
  write_hl(std::uint8_t value)
  {
    memory_write_byte(hl(), value);
  }

  [[nodiscard]]
  std::uint8_t
  read_hl()
  const
  {
    const auto offset = hl();
    return memory_read_byte(offset);
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
    memory_write_byte(sp - 1, high);
    memory_write_byte(sp - 2, low);
    sp -= 2;
  }

  [[nodiscard]]
  std::tuple<std::uint8_t, std::uint8_t>
  pop()
  {
    const auto low = memory_read_byte(sp);
    const auto high = memory_read_byte(sp + 1);
    sp += 2;
    return {high, low};
  }

  [[nodiscard]]
  std::uint16_t
  pop_word()
  {
    const auto low = memory_read_byte(sp);
    const auto high = memory_read_byte(sp + 1);
    sp += 2;
    return low | (high << 8);
  }

  void
  call(std::uint16_t addr)
  {
    push((pc_ >> 8) & 0x00ff, pc_ & 0x00ff);
    pc_ = addr;
  }

  [[nodiscard]]
  bool
  conditional_call(std::uint16_t addr, bool condition)
  {
    if (condition)
    {
      call(addr);
      return true;
    }
    else
    {
      return false;
    }
  }

  void
  jump(std::uint16_t addr)
  {
    pc_ = addr;
  }

  void
  conditional_jump(std::uint16_t addr, bool condition)
  {
    if (condition)
    {
      jump(addr);
    }
  }

  void
  ret()
  {
    pc_ = pop_word();
  }

  [[nodiscard]]
  bool
  conditional_ret(bool condition)
  {
    if (condition)
    {
      ret();
      return true;
    }
    else
    {
      return false;
    }
  }

  [[nodiscard]]
  std::uint8_t
  op1()
  {
    const auto op1 = memory_read_byte(pc_);
    pc_ += 1;
    return op1;
  }

  [[nodiscard]]
  std::tuple<std::uint8_t, std::uint8_t>
  operands()
  {
    const auto op1 = memory_read_byte(pc_ + 0);
    const auto op2 = memory_read_byte(pc_ + 1);
    pc_ += 2;
    return {op1, op2};
  }

  [[nodiscard]]
  std::uint16_t
  operands_word()
  {
    const auto [op1, op2] = operands();
    return op1 | (op2 << 8);
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

  [[nodiscard]]
  std::uint16_t
  pc()
  const noexcept
  {
    return pc_;
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
      call(interrupt);
      disable_interrupt();
      increment_cycles(11);
    }
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
  adda(std::uint8_t val, bool carry)
  noexcept
  {
    const std::uint16_t res = a + val + carry;
    flags.z  = (res & 0xff) == 0;
    flags.s  = (res & 0b10000000) != 0;
    flags.cy = (res & 0b100000000) != 0;
    flags.ac = (a ^ res ^ val) & 0x10;
    flags.p  = util::parity(res & 0xff);
    a = res & 0xff;
  }

  void
  suba(std::uint8_t val, bool carry)
  noexcept
  {
    const std::int16_t res = a - val - carry;
    flags.z = (res & 0xff) == 0;
    flags.s = (res & 0b10000000) != 0;
    flags.cy = (res & 0b100000000) != 0;
    flags.ac = ~(a ^ res ^ val) & 0x10;
    flags.p = util::parity(res & 0xff);
    a = res & 0xff;
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

private:

  void
  increment_cycles(std::uint64_t nb_cycles)
  noexcept
  {
    cycles_ += nb_cycles;
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
  flag_bits flags;
  
private:

  Machine& machine_;
  bool interrupt_;
  std::uint64_t cycles_;

  std::uint16_t pc_;
};

/*------------------------------------------------------------------------------------------------*/

} // namespace cpp8080::specific
