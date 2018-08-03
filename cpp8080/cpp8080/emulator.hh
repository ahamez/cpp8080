#pragma once

#include <istream>
#include <vector>

#include "cpp8080/specific/state.hh"
#include "cpp8080/space_invaders_machine.hh"

namespace cpp8080
{

/*------------------------------------------------------------------------------------------------*/

class emulator
{
public:

  emulator(std::istream& rom);

  void
  operator()();

private:

  // TODO: make `emulate` generic to the machine
  specific::state<space_invaders_machine> state_;
};
 
/*------------------------------------------------------------------------------------------------*/
  
} // namespace cpp8080
