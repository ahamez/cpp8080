#include <fstream>
#include <iostream>
#include <memory>

#include "cpp8080/emulator.hh"
#include "cpp8080/space_invaders_machine.hh"

/*------------------------------------------------------------------------------------------------*/

int
main(int argc, char** argv)
{
  if (argc != 2)
  {
    std::cerr << "Usage: " << argv[0] << " /path/to/rom_file\n";
    return 1;
  }
  auto rom_file = std::ifstream{argv[1], std::ios::binary};
  if (not rom_file.is_open())
  {
    std::cerr << "Cannot open ROM file " << argv[1] << '\n';
    return 1;
  }

  auto machine =
    std::make_shared<cpp8080::space_invaders_machine>(std::istreambuf_iterator<char>{rom_file},
                                                      std::istreambuf_iterator<char>{});
  auto emulator = cpp8080::emulator{machine};
}

/*------------------------------------------------------------------------------------------------*/
