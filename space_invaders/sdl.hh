#pragma once

#include <utility> // pair
#include <vector>

#include <SDL2/SDL.h>

#include "arcade.hh"
#include "events.hh"

/*------------------------------------------------------------------------------------------------*/

class sdl
  : public arcade
{
public:

  sdl();
  ~sdl();

  [[nodiscard]]
  std::pair<kind, event>
  get_next_event()
  override;

  void
  render_screen(const std::vector<std::uint8_t>&)
  override;

private:

  SDL_Window* window_;
  SDL_Renderer* renderer_;
};

/*------------------------------------------------------------------------------------------------*/
