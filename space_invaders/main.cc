#include <fstream>
#include <iostream>
#include <istream>
#include <memory>
#include <thread>

#include <SDL2/SDL.h>

#include "sdl.hh"
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
  auto display = sdl{machine};

  machine.start();
  while (true)
  {
    if (const auto run = display.process_events(); run)
    {
      machine();
      display.render_screen();
      std::this_thread::sleep_for(std::chrono::milliseconds{1});
    }
    else
    {
      return 0;
    }
  }
}

/*------------------------------------------------------------------------------------------------*/
