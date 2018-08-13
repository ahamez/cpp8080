#pragma once

#include <SDL2/SDL.h>

#include "space_invaders.hh"

/*------------------------------------------------------------------------------------------------*/

class display
{
private:

  static constexpr auto multiplier = 2;

public:

  display(const space_invaders&);
  ~display();

  void operator()();

private:

  const space_invaders& machine_;
  SDL_Window* window_;
  SDL_Renderer* renderer_;
};

/*------------------------------------------------------------------------------------------------*/
