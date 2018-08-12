#include <fstream>
#include <iostream>
#include <istream>
#include <memory>
#include <thread>

#include <SDL2/SDL.h>

#include "space_invaders.hh"

/*------------------------------------------------------------------------------------------------*/

int
main(int argc, char** argv)
{
  if(SDL_Init(SDL_INIT_VIDEO) < 0)
  {
    std::cerr << "SDL could not initialize. SDL_Error: " << SDL_GetError() << '\n';
    std::exit(1);
  }

  if (argc != 2)
  {
    std::cerr << "Usage: " << argv[0] << " /path/to/file\n";
    return 1;
  }
  auto file = std::ifstream{argv[1], std::ios::binary};
  if (not file.is_open())
  {
    std::cerr << "Cannot open ROM file " << argv[1] << '\n';
    return 1;
  }

  static constexpr auto factor = 2;

  auto machine = space_invaders{
    std::istreambuf_iterator<char>{file},
    std::istreambuf_iterator<char>{}
  };

  auto window = SDL_CreateWindow("Space Invaders",
                                 SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                 224 * factor, 256 * factor,
                                 SDL_WINDOW_SHOWN
                                 );
  if (window == nullptr)
  {
    std::cerr << "Window could not be created. SDL_Error: " << SDL_GetError() << '\n';
    std::exit(1);
  }

  auto renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (renderer == nullptr)
  {
    std::cerr << "Renderer could not be created. SDL_Error: " << SDL_GetError() << '\n';
    std::exit(1);
  }

  try
  {
    machine.start();
    while (true)
    {
      SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
      SDL_RenderClear(renderer);

      auto e = SDL_Event{};
      while (SDL_PollEvent(&e))
      {
        switch (e.type)
        {
          case SDL_QUIT:
          {
            std::exit(0);
          }

          case SDL_KEYDOWN:
          {
            switch (e.key.keysym.sym)
            {
              case SDLK_ESCAPE:
                std::exit(0);

              case SDLK_c:
                machine.key_down(space_invaders::key::coin);
                break;

              case SDLK_LEFT:
                machine.key_down(space_invaders::key::left);
                break;

              case SDLK_RIGHT:
                machine.key_down(space_invaders::key::right);
                break;

              case SDLK_SPACE:
                machine.key_down(space_invaders::key::fire);
                break;

              case SDLK_1:
                machine.key_down(space_invaders::key::start_1player);
                break;
            }
            break;
          }

          case SDL_KEYUP:
          {
            switch (e.key.keysym.sym)
            {
              case SDLK_c:
                machine.key_up(space_invaders::key::coin);
                break;

              case SDLK_LEFT:
                machine.key_up(space_invaders::key::left);
                break;

              case SDLK_RIGHT:
                machine.key_up(space_invaders::key::right);
                break;

              case SDLK_SPACE:
                machine.key_up(space_invaders::key::fire);
                break;

              case SDLK_1:
                machine.key_up(space_invaders::key::start_1player);
                break;
            }
          }
          break;
        }
      }

      machine();

//  Screen is rotated 90° counterclockwise, we can't simply iterate on video memory.
//  Rather than computing the rotaion, I chose to iterate in such a way that pixels are
//  set in the right order.
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
//                   <----------------------------------------------------------->
//                   x
//                  y+--------------+--------------+--------------+--------------+
//  8 pixels   ^ ^   | byte 31      | byte 63      |              | byte 7167    | |
//  on 1 byte  | |   |              |              |    ......    |              | |  pos [0,7]
//             v |   |    <0,31>    |    <1,31>    |              |  <223,31>    | v
//               |   +--------------+--------------+--------------+--------------+
//               |   | byte 30      | byte 62      |              | byte 7166    |
//               |   |              |              |    ......    |              |
//               |   |    <0,30>    |    <1,30>    |              |  <223,30>    |
//    256 pixels |   +--------------+--------------+--------------+--------------+
//               |   |              |              |              |              |
//               |   |    ......    |    ......    |              |    ......    |
//               |   |              |              |    <i, j>    |              |
//               |   +--------------+--------------+--------------+--------------+
//               |   | byte 0       | byte 32      |              | byte 7136    |
//               |   |              |              |    ......    |              |
//               |   |    <0,0>     |    <1,0>     |              |  <223,0>     |
//               v   +--------------+--------------+--------------+--------------+

      SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0);
      for (auto j = 31, y = 0; j >= 0; --j, ++y)
      {
        for (auto i = 0, x = 0; i < 224; ++i, ++x)
        {
          const auto byte = machine.read_memory(0x2400 + (j  + i * 32));
          for (auto b = 7, pos = 0; b >= 0; --b, ++pos)
          {
            if (((byte >> b) & 0x01) == 1)
            {
              const auto rect = SDL_Rect{
                static_cast<int>(x * factor),
                static_cast<int>((y * 8 + pos) * factor),
                1 * factor,
                1 * factor};
              SDL_RenderFillRect(renderer, &rect);
            }
          }
        }
      }
      SDL_RenderPresent(renderer);

      std::this_thread::sleep_for(std::chrono::milliseconds{1});
    }
  }
  catch (const cpp8080::specific::halt& h)
  {
    std::cout << h.what() << '\n';
  }

  SDL_DestroyWindow(window);
  SDL_Quit();
}

/*------------------------------------------------------------------------------------------------*/
