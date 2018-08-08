#pragma once

#include <chrono>
#include <iostream>
#include <memory>
#include <vector>

#include "cpp8080/meta/disassemble.hh"
#include "cpp8080/specific/instructions.hh"
#include "cpp8080/specific/state.hh"
#include "cpp8080/space_invaders_machine.hh"

namespace cpp8080 {

/*------------------------------------------------------------------------------------------------*/

template <typename Machine>
class emulator
{
private:
  
  using instructions = meta::override_instructions<
    specific::instructions_8080,
    typename Machine::overrides
  >;

public:

  emulator(std::shared_ptr<Machine> machine)
    : state_{machine}
    , machine_{machine}
    , which_interrupt_{1}
    , last_timer_{}
    , next_interrupt_{}
  {}

  struct verbose
  {
    std::ostream& os;

    template <typename Instruction>
    void
    pre(const specific::state<Machine>& state, Instruction)
    const
    {
      os
        << std::setfill('0') << std::setw(4) << state.pc << ' '
        << meta::disassemble(state, Instruction{}) << ' ';
    }

    template <typename Instruction>
    void
    post(const specific::state<Machine>& state, Instruction)
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
      if (which_interrupt_ == 1)
      {
        which_interrupt_ = 2;
      }
      else
      {
        which_interrupt_ = 1;
      }
      next_interrupt_ = now + std::chrono::milliseconds{8};
    }

    const auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(now - last_timer_);
    const auto cycles_to_catch_up = 2 * elapsed.count();
    auto cycles = std::uint64_t{0};

    while (cycles_to_catch_up > cycles)
    {
      const auto opcode = state_.read_memory(state_.pc);
      cycles += step(instructions{}, opcode, state_);
//      cycles += step(instructions{}, opcode, state_, verbose{std::cout});
    }
    last_timer_ = now;
  }

private:

  specific::state<Machine> state_;
  std::shared_ptr<Machine> machine_;
  std::uint8_t which_interrupt_;
  std::chrono::time_point<std::chrono::steady_clock> last_timer_;
  std::chrono::time_point<std::chrono::steady_clock> next_interrupt_;
};
 
/*------------------------------------------------------------------------------------------------*/
  
} // namespace cpp8080
