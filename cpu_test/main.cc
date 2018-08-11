#include <fstream>
#include <iostream>
#include <istream>
#include <memory>
#include <thread>

#include "cpp8080/machine/cpu_test.hh"

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

  auto cpu_test = cpp8080::machine::cpu_test{
    std::istreambuf_iterator<char>{file},
    std::istreambuf_iterator<char>{}
  };

  cpu_test();
}

/*------------------------------------------------------------------------------------------------*/
