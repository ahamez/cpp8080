#pragma once

#include <SDL2/SDL.h>

#include "space_invaders.hh"

/*------------------------------------------------------------------------------------------------*/

class sdl
{
private:

  static constexpr auto multiplier = 2;

public:

  sdl(space_invaders&);
  ~sdl();

  bool process_events();
  void render_screen();

private:

  space_invaders& machine_;
  SDL_Window* window_;
  SDL_Renderer* renderer_;
};

/*------------------------------------------------------------------------------------------------*/
