#pragma once

#include <utility> // pair
#include <vector>

#include "events.hh"

/*------------------------------------------------------------------------------------------------*/

class arcade
{
public:

  virtual ~arcade() {};

  [[nodiscard]]
  virtual
  std::pair<kind, event>
  get_next_event()= 0;

  virtual
  void
  render_screen(const std::vector<std::uint8_t>&) = 0;
};

/*------------------------------------------------------------------------------------------------*/
