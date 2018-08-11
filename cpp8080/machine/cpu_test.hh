#pragma once

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <ostream>
#include <sstream>
#include <vector>

#include "cpp8080/meta/disassemble.hh"
#include "cpp8080/meta/instructions.hh"
#include "cpp8080/specific/halt.hh"
#include "cpp8080/specific/instructions.hh"

namespace cpp8080::machine {

/*------------------------------------------------------------------------------------------------*/

namespace detail{
struct call_adr
  : meta::describe_instruction<
      cpp8080::specific::call_adr::opcode,
      cpp8080::specific::call_adr::cycles,
      cpp8080::specific::call_adr::bytes>
{
  static constexpr auto name = "call_adr";

  template <typename Machine>
  void operator()(specific::state<Machine>& state) const
  {
    if (((state.op2() << 8) | state.op1()) == 5)
    {
      if (state.c == 9)
      {
        const std::uint16_t offset = state.de();
        auto s = std::string{};
        for (auto i = std::uint16_t{0}; ; ++i)
        {
          const auto c = state.read_memory(offset + i);
          if (c == '$')
          {
            break;
          }
          s.push_back(c);
        }

        std::cout << s << '\n';
      }
      else if (state.c == 2)
      {
        std::cout << state.e;
      }
    }
    else
    {
      cpp8080::specific::call_adr{}(state);
    }
  }
};

/*------------------------------------------------------------------------------------------------*/

struct verbose
{
  std::ostream& os;

  template <typename Machine, typename Instruction>
  void
  pre(const specific::state<Machine>& state, Instruction)
  const
  {
    os
      << std::setfill('0') << std::setw(4) << state.pc << ' '
      << meta::disassemble(state, Instruction{}) << ' ';
  }

  template <typename Machine, typename Instruction>
  void
  post(const specific::state<Machine>& state, Instruction)
  const
  {
    os << state << std::endl;
  }
};

} // namespace detail

/*------------------------------------------------------------------------------------------------*/

class cpu_test
{
public:

  using overrides = meta::make_instructions<
    detail::call_adr
  >;

  using instructions = meta::override_instructions<
    specific::instructions_8080,
    overrides
  >;

public:

  template <typename InputIterator>
  cpu_test(InputIterator first, InputIterator last)
    : state_{*this}
    , memory_(65536, 0)
  {
    std::copy(first, last, memory_.begin() + 0x100);
    state_.pc = 0x100;

//    Skip DAA test
    memory_[0x59c] = 0xc3;
    memory_[0x59d] = 0xc2;
    memory_[0x59e] = 0x05;
  }

  void
  write_memory(std::uint16_t address, std::uint8_t value)
  {
    memory_.at(address) = value;
  }

  [[nodiscard]]
  std::uint8_t
  read_memory(std::uint16_t address)
  const
  {
    return memory_.at(address);
  }

  void
  operator()()
  {
    while (true)
    {
      const auto opcode = state_.read_memory(state_.pc);
      step(instructions{}, opcode, state_);
//     step(instructions{}, opcode, state_, detail::verbose{std::cout})
      if (state_.pc == 0)
      {
        std::cout << '\n';
        break;
      }
    }
  }

private:

  specific::state<cpu_test> state_;
  std::vector<std::uint8_t> memory_;
};

/*------------------------------------------------------------------------------------------------*/

} // namespace cpp8080::machine
