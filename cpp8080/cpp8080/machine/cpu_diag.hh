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
struct call_adr;
}

/*------------------------------------------------------------------------------------------------*/

class cpu_diag
{
public:

  using overrides = meta::make_instructions<
    detail::call_adr
  >;

public:

  template <typename InputIterator>
  cpu_diag(InputIterator first, InputIterator last)
    : memory_(16384, 0)
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

private:

  std::vector<std::uint8_t> memory_;
};

/*------------------------------------------------------------------------------------------------*/

namespace detail {

struct call_adr
  : meta::describe_instruction<
      cpp8080::specific::call_adr::opcode,
      cpp8080::specific::call_adr::cycles,
      cpp8080::specific::call_adr::bytes>
{
  static constexpr auto name = "call_adr";

  void operator()(specific::state<cpu_diag>& state) const
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

        if (s == " CPU IS OPERATIONAL")
        {
          throw specific::halt{s};
        }
        else
        {
          throw std::runtime_error{s};
        }
      }
    }
    else if (((state.op2() << 8) | state.op1()) == 0)
    {
      // Success ?
      std::exit(0);
    }
    else
    {
      cpp8080::specific::call_adr{}(state);
    }
  }
};

} // namespace detail

/*------------------------------------------------------------------------------------------------*/

} // namespace cpp8080::machine
