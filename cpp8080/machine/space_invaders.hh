#pragma once

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <sstream>
#include <vector>

#include "cpp8080/meta/disassemble.hh"
#include "cpp8080/meta/instructions.hh"
#include "cpp8080/specific/instructions.hh"
#include "cpp8080/specific/state.hh"

namespace cpp8080::machine {

/*------------------------------------------------------------------------------------------------*/

namespace detail{

struct in_override : meta::describe_instruction<0xdb, 3, 2>
{
  static constexpr auto name = "in";

  template <typename Machine>
  void operator()(specific::state<Machine>& state) const
  {
    const auto port = state.op1();
    state.a = state.machine().in(port);
    state.pc += 1;
  }
};

struct out_override : meta::describe_instruction<0xd3, 3, 2>
{
  static constexpr auto name = "out";

  template <typename Machine>
  void operator()(specific::state<Machine>& state) const
  {
    const auto port = state.op1();
    state.machine().out(port, state.a);
    state.pc += 1;
  }
};

} // namespace detail

/*------------------------------------------------------------------------------------------------*/

class space_invaders
{
private:

  using overrides = meta::make_instructions<
    detail::in_override,
    detail::out_override
  >;

  using instructions = meta::override_instructions<
    specific::instructions_8080,
    overrides
  >;

public:

  enum class key {coin, left, right, fire, start_1player};

public:

  template <typename InputIterator>
  space_invaders(InputIterator first, InputIterator last)
    : state_{*this}
    , memory_(16384, 0)
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
      case key::start_1player : port1_ |= 0x04; break;
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
      case key::start_1player : port1_ &= ~0x04; break;
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
  struct verbose
  {
    std::ostream& os;

    template <typename Instruction>
    void
    pre(const specific::state<space_invaders>& state, Instruction)
    const
    {
      os
      << std::setfill('0') << std::setw(4) << state.pc << ' '
      << meta::disassemble(state, Instruction{}) << ' ';
    }

    template <typename Instruction>
    void
    post(const specific::state<space_invaders>& state, Instruction)
    const
    {
      os << state << std::endl;
    }
  };

  void
  start()
  {
    last_timer_ = std::chrono::steady_clock::now();
    next_interrupt_ = last_timer_ + std::chrono::milliseconds{16};
    which_interrupt_ = 1;
  }

  void
  operator()()
  {
    const auto now = std::chrono::steady_clock::now();

    if (state_.interrupt_enabled() and now > next_interrupt_)
    {
      state_.interrupt(which_interrupt_);
      which_interrupt_ = which_interrupt_ == 0x08
                       ? 0x10
                       : 0x08;
      next_interrupt_ = now + std::chrono::milliseconds{8};
    }

    const auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(now - last_timer_);
    const auto cycles_to_catch_up = 2 * elapsed.count();
    auto cycles = std::uint64_t{0};

    while (cycles_to_catch_up > cycles)
    {
      const auto opcode = state_.read_memory(state_.pc);
      cycles += step(instructions{}, opcode, state_);
      // cycles += step(instructions{}, opcode, state_, verbose{std::cout});
    }
    last_timer_ = now;
  }

private:

  specific::state<space_invaders> state_;
  std::vector<std::uint8_t> memory_;
  std::uint8_t shift0_;
  std::uint8_t shift1_;
  std::uint8_t shift_offset_;
  std::uint8_t port1_;
  std::uint8_t port2_;
  std::uint8_t which_interrupt_;
  std::chrono::time_point<std::chrono::steady_clock> last_timer_;
  std::chrono::time_point<std::chrono::steady_clock> next_interrupt_;
};

/*------------------------------------------------------------------------------------------------*/

} // namespace cpp8080::machine
