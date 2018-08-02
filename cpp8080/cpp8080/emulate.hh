#pragma once

#include <istream>
#include <vector>

#include "cpp8080/specific/state.hh"

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

  specific::state state_;
};
 
/*------------------------------------------------------------------------------------------------*/
  
} // namespace cpp8080
