#include <iostream>

#include "cpp8080/emulate.hh"
#include "cpp8080/specific/instructions.hh"

namespace cpp8080 {

/*------------------------------------------------------------------------------------------------*/

emulate::emulate(std::istream& rom)
  : state_{std::istreambuf_iterator<char>{rom}, std::istreambuf_iterator<char>{}}
{}

/*------------------------------------------------------------------------------------------------*/

struct in_override : meta::describe_instruction<0xdb, 10, 2>
{
  static constexpr auto name = "in";

  void operator()(specific::state&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0xdb"};
  }
};

/*------------------------------------------------------------------------------------------------*/

struct out_override : meta::describe_instruction<0xd3, 10, 2>
{
  static constexpr auto name = "out";

  void operator()(specific::state&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0xdb"};
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
