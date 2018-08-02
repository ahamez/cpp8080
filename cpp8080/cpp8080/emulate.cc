#include <iostream>
#include <memory>

#include "cpp8080/emulate.hh"
#include "cpp8080/specific/instructions.hh"

namespace cpp8080 {

/*------------------------------------------------------------------------------------------------*/

emulate::emulate(std::istream& rom)
  : state_{
      std::istreambuf_iterator<char>{rom},
      std::istreambuf_iterator<char>{},
      std::make_shared<spache_invaders_machine>()
  }
{}
  
/*------------------------------------------------------------------------------------------------*/
  
struct in_override : meta::describe_instruction<0xdb, 3, 2>
{
  static constexpr auto name = "in";

  void operator()(specific::state<spache_invaders_machine>& state) const
  {
    const auto port = state.op1();
    state.a = state.machine().in(port);
    state.pc += 2;
  }
};

/*------------------------------------------------------------------------------------------------*/

struct out_override : meta::describe_instruction<0xd3, 3, 2>
{
  static constexpr auto name = "out";

  void operator()(specific::state<spache_invaders_machine>& state) const
  {
    const auto port = state.op1();
    state.machine().out(port, state.a);
    state.pc += 2;
  }
};

/*------------------------------------------------------------------------------------------------*/

using instructions = meta::override_instructions<specific::instructions_8080,
  meta::instruction<in_override>,
  meta::instruction<out_override>
>::type;

/*------------------------------------------------------------------------------------------------*/

void
emulate::operator()()
{
  while (true)
  {
    const auto opcode = state_.read_memory(state_.pc);
    state_.pc += 1;
//    step(instructions{}, opcode, state_);
    step(instructions{}, opcode, state_, std::cout);
  }
}
  
/*------------------------------------------------------------------------------------------------*/
  
} // namespace cpp8080
