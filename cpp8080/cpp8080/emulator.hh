#pragma once

#include <iostream>
#include <istream>
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

  emulator(std::istream& rom, std::shared_ptr<Machine> machine)
    : state_{
        std::istreambuf_iterator<char>{rom},
        std::istreambuf_iterator<char>{},
        machine}
    , machine_{machine}
  {}
  
  struct verbose
  {
    template <typename Instruction>
    void
    pre(const specific::state<Machine>& state, Instruction)
    const
    {
      std::cout
        << std::setfill('0') << std::setw(4) << state.pc << ' '
        << meta::disassemble(state, Instruction{}) << ' ';
    }

    template <typename Instruction>
    void
    post(const specific::state<Machine>& state, Instruction)
    const
    {
      std::cout << state << '\n';
    }
  };
  
  void
  operator()()
  {
    while (true)
    {
      const auto opcode = state_.read_memory(state_.pc);
      state_.pc += 1;
//      step(instructions{}, opcode, state_);
      step(instructions{}, opcode, state_, verbose{});
    }
  }

private:

  specific::state<Machine> state_;
  std::shared_ptr<Machine> machine_;
};
 
/*------------------------------------------------------------------------------------------------*/
  
} // namespace cpp8080
