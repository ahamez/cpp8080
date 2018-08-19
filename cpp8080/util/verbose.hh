#pragma once

#include <iomanip>
#include <ostream>

#include "cpp8080/meta/disassemble.hh"
#include "cpp8080/specific/cpu.hh"

namespace cpp8080::util {

/*------------------------------------------------------------------------------------------------*/

struct verbose
{
  std::ostream& os;

  template <typename Machine, typename Instruction>
  void
  pre(const specific::cpu<Machine>& cpu, Instruction)
  const
  {
    os
      << std::setfill('0') << std::setw(4) << cpu.pc() << ' '
      << meta::disassemble(cpu, Instruction{}) << ' ';
  }

  template <typename Machine, typename Instruction>
  void
  post(const specific::cpu<Machine>& cpu, Instruction)
  const
  {
    os << cpu << std::endl;
  }
};


/*------------------------------------------------------------------------------------------------*/

} // namespace cpp8080::util
