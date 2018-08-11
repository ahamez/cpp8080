#pragma once

#include <algorithm>
#include <cstdint>
#include <sstream>
#include <vector>

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
        const std::uint16_t offset = (state.d<<8) | (state.e);
        auto s = std::string{};
        for (auto i = std::uint16_t{3}; ; ++i)
        {
          const auto c = state.read_memory(offset + i);
          if (c == '$')
          {
            break;
          }
          s.push_back(c);
        }

//        std::cout << s << '\n';
        if (s == " CPU IS OPERATIONAL")
        {
          throw specific::halt{s};
        }
        else
        {
          throw std::runtime_error{s};
        }
      }
      else if (state.c == 2)
      {
        // TODO: print single character stored in register E.
      }
    }
    else
    {
      cpp8080::specific::call_adr{}(state);
    }
  }
};

} // namespace detail

/*------------------------------------------------------------------------------------------------*/

class cpu_diag
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
  cpu_diag(InputIterator first, InputIterator last)
    : state_{*this}
    , memory_(16384, 0)
  {
    std::copy(first, last, memory_.begin());

//    Fix bug of diagnostic???
    memory_[368] = 0x7;

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
    const auto opcode = state_.read_memory(state_.pc);
    step(instructions{}, opcode, state_);
    // cycles += step(instructions{}, opcode, state_, verbose{std::cout});
  }

private:

  specific::state<cpu_diag> state_;
  std::vector<std::uint8_t> memory_;
};

/*------------------------------------------------------------------------------------------------*/

} // namespace cpp8080::machine
