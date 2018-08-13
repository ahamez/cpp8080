#pragma once

#include <utility> // pair
#include <vector>

#include <SDL2/SDL.h>

#include "events.hh"

/*------------------------------------------------------------------------------------------------*/

class sdl
{
public:

  sdl();
  ~sdl();

  [[nodiscard]]
  std::pair<kind, event>
  get_next_event();

  void
  render_screen(const std::vector<std::uint8_t>&);

private:

  SDL_Window* window_;
  SDL_Renderer* renderer_;
};

/*------------------------------------------------------------------------------------------------*/
