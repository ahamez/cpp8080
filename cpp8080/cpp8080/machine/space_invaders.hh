#pragma once

#include <algorithm>
#include <cstdint>
#include <sstream>
#include <vector>

#include "cpp8080/meta/instructions.hh"

namespace cpp8080::machine {

/*------------------------------------------------------------------------------------------------*/

namespace detail{
struct in_override;
struct out_override;
}

/*------------------------------------------------------------------------------------------------*/

class space_invaders
{
public:

  enum class key {coin, left, right, fire, start};

  using overrides = meta::make_instructions<
    detail::in_override,
    detail::out_override
  >;

public:

  template <typename InputIterator>
  space_invaders(InputIterator first, InputIterator last)
    : memory_(16384, 0)
    , shift0_{0}
    , shift1_{0}
    , shift_offset_{0}
    , port1_{1 << 3}
    , port2_{0}
  {
    std::copy(first, last, memory_.begin());
  }

  [[nodiscard]]
  std::uint8_t
  in(std::uint8_t port)
  const noexcept
  {
    switch (port)
    {
      case 1:
        return port1_;

      case 2:
        return port2_;
        
      case 3:
      {
        const std::uint16_t v = (shift1_ << 8) | shift0_;
        return (v >> (8 - shift_offset_)) & 0xff;
      }
        
      default:
        throw std::runtime_error{"Unknwown IN port"};
    }
  }

  void
  out(std::uint8_t port, std::uint8_t value)
  noexcept
  {
    switch (port)
    {
      case 2:
        shift_offset_ = value & 0x07;
        break;

      case 3:
        // play sound;
        break;

      case 4:
        shift0_ = shift1_;
        shift1_ = value;
        break;

      case 5:
        // play sound;
        break;

      case 6:
        break;

      default:
        throw std::runtime_error{"Unknwown OUT port"};
    }
  }

  void
  key_down(key k)
  noexcept
  {
    switch (k)
    {
      case key::coin  : port1_ |= 0x01; break;
      case key::left  : port1_ |= 0x20; break;
      case key::right : port1_ |= 0x40; break;
      case key::fire  : port1_ |= 0x10; break;
      case key::start : port1_ |= 0x04; break;
    }
  }
  
  void
  key_up(key k)
  noexcept
  {
    switch (k)
    {
      case key::coin  : port1_ &= ~0x01; break;
      case key::left  : port1_ &= ~0x20; break;
      case key::right : port1_ &= ~0x40; break;
      case key::fire  : port1_ &= ~0x10; break;
      case key::start : port1_ &= ~0x04; break;
    }
  }

  void
  write_memory(std::uint16_t address, std::uint8_t value)
  {
    if (address < 0x2000)
    {
      auto ss = std::stringstream{};
      ss << std::hex << "Attempt to write 0x" << +value << " in ROM at 0x" << +address;
      throw std::runtime_error{ss.str()};
    }

    if (address >= 0x4000)
    {
      auto ss = std::stringstream{};
      ss << std::hex << "Attempt to write 0x" << +value << " outside of RAM at 0x" << +address;
      throw std::runtime_error{ss.str()};
    }

    memory_[address] = value;
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
  std::uint8_t shift0_;
  std::uint8_t shift1_;
  std::uint8_t shift_offset_;
  std::uint8_t port1_;
  std::uint8_t port2_;
};

/*------------------------------------------------------------------------------------------------*/

namespace detail {

struct in_override : meta::describe_instruction<0xdb, 3, 2>
{
  static constexpr auto name = "in";

  void operator()(specific::state<space_invaders>& state) const
  {
    const auto port = state.op1();
    state.a = state.machine().in(port);
    state.pc += 1;
  }
};
  
struct out_override : meta::describe_instruction<0xd3, 3, 2>
{
  static constexpr auto name = "out";

  void operator()(specific::state<space_invaders>& state) const
  {
    const auto port = state.op1();
    state.machine().out(port, state.a);
    state.pc += 1;
  }
};
  
} // namespace detail

/*------------------------------------------------------------------------------------------------*/

} // namespace cpp8080::machine
