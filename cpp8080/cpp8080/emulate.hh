#pragma once

#include <istream>
#include <vector>

#include "cpp8080/specific/state.hh"
#include "cpp8080/space_invaders_machine.hh"

namespace cpp8080
{

/*------------------------------------------------------------------------------------------------*/

class emulate
{
public:

  emulate(std::istream& rom);

  void
  operator()();

private:

  specific::state<spache_invaders_machine> state_;
};
 
/*------------------------------------------------------------------------------------------------*/
  
} // namespace cpp8080
