#pragma once

#include <iomanip>
#include <ostream>

#include "cpp8080/meta/disassemble.hh"
#include "cpp8080/specific/state.hh"

namespace cpp8080::util {

/*------------------------------------------------------------------------------------------------*/

struct verbose
{
  std::ostream& os;

  template <typename Machine, typename Instruction>
  void
  pre(const specific::state<Machine>& state, Instruction)
  const
  {
    os
      << std::setfill('0') << std::setw(4) << state.pc << ' '
      << meta::disassemble(state, Instruction{}) << ' ';
  }

  template <typename Machine, typename Instruction>
  void
  post(const specific::state<Machine>& state, Instruction)
  const
  {
    os << state << std::endl;
  }
};


/*------------------------------------------------------------------------------------------------*/

} // namespace cpp8080::util
