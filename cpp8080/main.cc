#include <fstream>
#include <iostream>

#include "cpp8080/emulator.hh"

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

  cpp8080::emulator{rom_file}();
}

/*------------------------------------------------------------------------------------------------*/
