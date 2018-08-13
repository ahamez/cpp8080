#include <fstream>
#include <iostream>
#include <istream>
#include <memory>
#include <thread>

#include <SDL2/SDL.h>

#include "display.hh"
#include "space_invaders.hh"

/*------------------------------------------------------------------------------------------------*/

int
main(int argc, char** argv)
{
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

  auto machine = space_invaders{
    std::istreambuf_iterator<char>{file},
    std::istreambuf_iterator<char>{}
  };

  auto render_screen = display{machine};

  machine.start();
  for (auto quit = false; not quit;)
  {
    for (auto e = SDL_Event{}; SDL_PollEvent(&e);)
    {
      switch (e.type)
      {
        case SDL_QUIT:
        {
          quit = true;
          break;
        }

        case SDL_KEYDOWN:
        {
          switch (e.key.keysym.sym)
          {
            case SDLK_ESCAPE:
              quit = true;
              break;

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
    render_screen();

    std::this_thread::sleep_for(std::chrono::milliseconds{1});
  }
}

/*------------------------------------------------------------------------------------------------*/
