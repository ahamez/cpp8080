#include <fstream>
#include <iostream>
#include <istream>
#include <memory>
#include <thread>

#include <SDL2/SDL.h>

#include "cpp8080/emulator.hh"
#include "cpp8080/machine/cpu_diag.hh"
#include "cpp8080/machine/space_invaders.hh"

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

  auto machine =
    std::make_shared<cpp8080::machine::space_invaders>(std::istreambuf_iterator<char>{file},
                                                       std::istreambuf_iterator<char>{});
//  auto machine =
//    std::make_shared<cpp8080::machine::cpu_diag>(std::istreambuf_iterator<char>{file},
//                                                 std::istreambuf_iterator<char>{});

  auto emulator = cpp8080::emulator{machine};

  auto window = SDL_CreateWindow("Space Invaders",
                                 SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                 256, 224,
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
    auto e = SDL_Event{};
    auto quit = false;

    emulator.start();
    while (not quit)
    {
      while (SDL_PollEvent(&e))
      {
        switch (e.type)
        {
          case SDL_QUIT:
          {
            quit = true;
            std::exit(0);
          }

          case SDL_KEYDOWN:
          {
            switch (e.key.keysym.sym)
            {
              case SDLK_c:
                std::cout << "down c\n";
                machine->key_down(cpp8080::machine::space_invaders::key::coin);
                break;

              case SDLK_LEFT:
                std::cout << "down left\n";
                machine->key_down(cpp8080::machine::space_invaders::key::left);
                break;

              case SDLK_RIGHT:
                std::cout << "down right\n";
                machine->key_down(cpp8080::machine::space_invaders::key::right);
                break;

              case SDLK_SPACE:
                std::cout << "down space\n";
                machine->key_down(cpp8080::machine::space_invaders::key::fire);
                break;

              case SDLK_f:
                std::cout << "down f\n";
                machine->key_down(cpp8080::machine::space_invaders::key::start);
                break;
            }
          }

          case SDL_KEYUP:
          {
            switch (e.key.keysym.sym)
            {
              case SDLK_c:
                std::cout << "up c\n";
                machine->key_up(cpp8080::machine::space_invaders::key::coin);
                break;

              case SDLK_LEFT:
                std::cout << "up left\n";
                machine->key_up(cpp8080::machine::space_invaders::key::left);
                break;

              case SDLK_RIGHT:
                std::cout << "up right\n";
                machine->key_up(cpp8080::machine::space_invaders::key::right);
                break;

              case SDLK_SPACE:
                std::cout << "up space\n";
                machine->key_up(cpp8080::machine::space_invaders::key::fire);
                break;

              case SDLK_f:
                std::cout << "up f\n";
                machine->key_up(cpp8080::machine::space_invaders::key::start);
                break;
            }
          }
        }
      }

      emulator();

      SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
      SDL_RenderClear(renderer);
      SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

//      for (auto i = 0ul; i < 256; ++i)
//      {
//        for (auto j = 0ul; j < 224/8; ++j)
//        {
//          for (auto p = 0ul; p < 8; ++p)
//          {
//            if ((machine->read_memory(0x2400 + i * j) & (1 << p)) == 1)
//            {
//              const auto rect = SDL_Rect{static_cast<int>(i), static_cast<int>(j*8), 1, 1};
//              SDL_RenderFillRect(renderer, &rect);
//            }
//          }
//        }
//      }

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
