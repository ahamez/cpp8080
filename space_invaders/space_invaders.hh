#pragma once

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <sstream>
#include <thread>
#include <vector>

#include "cpp8080/meta/disassemble.hh"
#include "cpp8080/meta/instructions.hh"
#include "cpp8080/specific/instructions.hh"
#include "cpp8080/specific/state.hh"

#include "events.hh"
#include "sdl.hh"

/*------------------------------------------------------------------------------------------------*/

static constexpr auto fps = 60;
static constexpr auto cycles_per_second = 2'000'000; // 2 MHz
static constexpr auto cycles_per_frame = cycles_per_second / fps;

/*------------------------------------------------------------------------------------------------*/

struct in_override : cpp8080::meta::describe_instruction<0xdb, 3, 2>
{
  static constexpr auto name = "in";

  template <typename Machine>
  void operator()(cpp8080::specific::state<Machine>& state) const
  {
    const auto port = state.op1();
    state.a = state.machine().in(port);
    state.pc += 1;
  }
};

struct out_override : cpp8080::meta::describe_instruction<0xd3, 3, 2>
{
  static constexpr auto name = "out";

  template <typename Machine>
  void operator()(cpp8080::specific::state<Machine>& state) const
  {
    const auto port = state.op1();
    state.machine().out(port, state.a);
    state.pc += 1;
  }
};

/*------------------------------------------------------------------------------------------------*/

class space_invaders
{
private:

  using overrides = cpp8080::meta::make_instructions<
    in_override,
    out_override
  >;

  using instructions = cpp8080::meta::override_instructions<
    cpp8080::specific::instructions_8080,
    overrides
  >;

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
      // Mirror RAM. What's the goal of this?
      return; // address -= 0x2000 also works;
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

  //  TODO space_invaders doesn't need to know sdl type.
  void
  operator()(sdl& display)
  {
    auto which_interrupt = std::uint8_t{0x08};
    auto next_interrupt = std::chrono::steady_clock::now() + std::chrono::microseconds{8333};

    for (auto run = true; run;)
    {
      const auto now = std::chrono::steady_clock::now();

      for (auto process_events = true; process_events;)
      {
        switch (const auto [kind, event] = display.get_next_event(); kind)
        {
          case kind::key_up   : key_up(event);   break;
          case kind::key_down : key_down(event); break;
          case kind::other:
            switch (event)
            {
              case event::quit : run = false; break;
              case event::end  : process_events = false; break;
              default          : break;
            }
        }
      }

      if (now > next_interrupt)
      {
        state_.interrupt(which_interrupt);
        which_interrupt = which_interrupt == 0x08 ? 0x10 : 0x08;
        next_interrupt = now + std::chrono::microseconds{8333};
      }

      for (auto cycles = std::uint64_t{0}; cycles <= cycles_per_frame;)
      {
        const auto opcode = state_.read_memory(state_.pc);
        cycles += step(instructions{}, opcode, state_);
      }

      display.render_screen(memory_);

      const auto duration = std::chrono::steady_clock::now() - now;
      std::this_thread::sleep_for(std::chrono::milliseconds{1} - duration);
    }
  }

private:

  void
  key_down(event k)
  noexcept
  {
    switch (k)
    {
      case event::coin  : port1_ |= 0x01; break;
      case event::left  : port1_ |= 0x20; break;
      case event::right : port1_ |= 0x40; break;
      case event::fire  : port1_ |= 0x10; break;
      case event::start_1player : port1_ |= 0x04; break;
      default: break;
    }
  }

  void
  key_up(event k)
  noexcept
  {
    switch (k)
    {
      case event::coin  : port1_ &= ~0x01; break;
      case event::left  : port1_ &= ~0x20; break;
      case event::right : port1_ &= ~0x40; break;
      case event::fire  : port1_ &= ~0x10; break;
      case event::start_1player : port1_ &= ~0x04; break;
      default: break;
    }
  }

private:

  cpp8080::specific::state<space_invaders> state_;
  std::vector<std::uint8_t> memory_;
  std::uint8_t shift0_;
  std::uint8_t shift1_;
  std::uint8_t shift_offset_;
  std::uint8_t port1_;
  std::uint8_t port2_;
};

/*------------------------------------------------------------------------------------------------*/
