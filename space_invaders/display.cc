#include <iostream>

#include "display.hh"

/*------------------------------------------------------------------------------------------------*/

// TODO Proper handling of SDL failures.

display::display(const space_invaders& machine)
  : machine_{machine}
  , window_{nullptr}
  , renderer_{nullptr}
{
  if (SDL_Init(SDL_INIT_VIDEO) < 0)
  {
    std::cerr << "SDL could not initialize. SDL_Error: " << SDL_GetError() << '\n';
    std::exit(1);
  }

  window_ = SDL_CreateWindow("Space Invaders",
                            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                            224 * multiplier, 256 * multiplier,
                            SDL_WINDOW_SHOWN
                            );

  if (window_ == nullptr)
  {
    std::cerr << "Window could not be created. SDL_Error: " << SDL_GetError() << '\n';
    std::exit(1);
  }

  renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED);
  if (renderer_ == nullptr)
  {
    std::cerr << "Renderer could not be created. SDL_Error: " << SDL_GetError() << '\n';
    std::exit(1);
  }
}

/*------------------------------------------------------------------------------------------------*/

display::~display()
{
  SDL_DestroyWindow(window_);
  SDL_Quit();
}

/*------------------------------------------------------------------------------------------------*/

void
display::operator()()
{
  //  Screen is rotated 90° counterclockwise, we can't simply iterate on video memory.
  //  Rather than computing the rotation, I chose to iterate in such a way that pixels
  //  are set in the right order.
  //
  //                                       Video memory layout
  //                   +-----------------------------------------------------------+
  //                   |0x2400                                                     |
  //                   |                                                           |
  //                   |byte 0                                            byte 7136|
  //                   +-----------------------------------------------------------+
  //
  //
  //                                             224 pixels
  //                   |-----------------------------------------------------------|
  //
  //                  x=0
  //  _            y=0 +--------------+--------------+--------------+--------------+
  //  |                | byte 31      | byte 63      |              | byte 7167    | | pos=0
  //  |    8 pixels    |              |              |    ......    |              | |
  //  |    on 1 byte   |  <i=0,j=31>  |    <1,31>    |              |  <223,31>    | v pos=7
  //  |                +--------------+--------------+--------------+--------------+
  //  |                | byte 30      | byte 62      |              | byte 7166    |
  //  |                |              |              |    ......    |              |
  //  |                |    <0,30>    |    <1,30>    |              |  <223,30>    |
  //  |   256 pixels   +--------------+--------------+--------------+--------------+
  //  |                |              |              |              |              |
  //  |                |    ......    |    ......    |              |    ......    |
  //  |                |              |              |    <i, j>    |              |
  //  |                +--------------+--------------+--------------+--------------+
  //  |                | byte 0       | byte 32      |              | byte 7136    |
  //  |                |              |              |    ......    |              |
  //  |                |    <0,0>     |    <1,0>     |              | <i=223,j=0>  |
  //  -                +--------------+--------------+--------------+--------------+ x=223
  //                                                                            y=31

  SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 0);
  SDL_RenderClear(renderer_);

  SDL_SetRenderDrawColor(renderer_, 255, 255, 255, 0);
  for (auto j = 31, y = 0; j >= 0; --j, ++y)
  {
    for (auto i = 0, x = 0; i < 224; ++i, ++x)
    {
      const auto byte = machine_.read_memory(0x2400 + (j  + i * 32));
      for (auto b = 7, pos = 0; b >= 0; --b, ++pos)
      {
        if (((byte >> b) & 0x01) == 1)
        {
          const auto rect = SDL_Rect{
            static_cast<int>(x * multiplier),
            static_cast<int>((y * 8 + pos) * multiplier),
            multiplier,
            multiplier};
          SDL_RenderFillRect(renderer_, &rect);
        }
      }
    }
  }
  SDL_RenderPresent(renderer_);
}

/*------------------------------------------------------------------------------------------------*/
