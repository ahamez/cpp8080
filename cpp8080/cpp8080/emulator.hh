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
  
  using instructions = typename meta::override_instructions<
    specific::instructions_8080,
    typename Machine::overrides
  >::type;
  
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
    operator()(const specific::state<Machine>& state, Instruction)
    const
    {
      meta::disassemble<specific::state<Machine>, Instruction, Instruction::bytes>{}(std::cout, state);
      std::cout << '\n';
    }
  };
  
  void
  operator()()
  {
    while (true)
    {
      const auto opcode = state_.read_memory(state_.pc);
      state_.pc += 1;
      step(instructions{}, opcode, state_);
//      step(instructions{}, opcode, state_, verbose{});
    }
  }

private:

  specific::state<Machine> state_;
  std::shared_ptr<Machine> machine_;
};
 
/*------------------------------------------------------------------------------------------------*/
  
} // namespace cpp8080
