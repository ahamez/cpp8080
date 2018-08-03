#pragma once

#include <istream>
#include <memory>
#include <vector>

#include "cpp8080/specific/instructions.hh"
#include "cpp8080/specific/state.hh"
#include "cpp8080/space_invaders_machine.hh"

namespace cpp8080
{

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

  void
  operator()()
  {
    while (true)
    {
      const auto opcode = state_.read_memory(state_.pc);
      state_.pc += 1;
      step(instructions{}, opcode, state_);
      //    step(instructions{}, opcode, state_, std::cout);
    }
  }

private:

  specific::state<Machine> state_;
  std::shared_ptr<Machine> machine_;
};
 
/*------------------------------------------------------------------------------------------------*/
  
} // namespace cpp8080
