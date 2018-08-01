#pragma once

#include <algorithm>
#include <iomanip>
#include <ostream>
#include <vector>

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

struct state
{
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

  template <typename InputIterator>
  state(InputIterator first, InputIterator last)
    : memory(16384, 0)
  {
    std::copy(first, last, memory.begin());
  }
  
  friend
  std::ostream&
  operator<<(std::ostream&, const state&);

  void
  write_memory(std::uint16_t address, std::uint8_t value);

  [[nodiscard]]
  std::uint8_t
  read_memory(std::uint16_t address)
  const;
  
  void
  write_hl(std::uint8_t value);

  [[nodiscard]]
  std::uint8_t
  read_hl()
  const;

  void
  flags_zsp(std::uint8_t value)
  noexcept;

  void
  logic_flags_a()
  noexcept;

  void
  push(std::uint8_t high, std::uint8_t low);

  void
  pop(std::uint8_t& high, std::uint8_t& low);

  [[nodiscard]]
  std::uint8_t
  op1()
  const;
  
  [[nodiscard]]
  std::uint8_t
  op2()
  const;

private:
  
  std::vector<std::uint8_t> memory;
};

/*------------------------------------------------------------------------------------------------*/

} // namespace cpp8080::specific
