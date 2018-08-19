#include <fstream>
#include <iostream>
#include <istream>

#include <SDL2/SDL.h>

#include "arcade.hh"
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
    std::unique_ptr<arcade>{new sdl{}},
    std::istreambuf_iterator<char>{file},
    std::istreambuf_iterator<char>{}
  };

  machine();
}

/*------------------------------------------------------------------------------------------------*/
