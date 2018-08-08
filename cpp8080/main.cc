#include <fstream>
#include <iostream>
#include <istream>
#include <memory>
#include <thread>

#include "cpp8080/emulator.hh"
#include "cpp8080/machine/space_invaders.hh"

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

  auto machine =
    std::make_shared<cpp8080::machine::space_invaders>(std::istreambuf_iterator<char>{file},
                                                       std::istreambuf_iterator<char>{});
  auto emulator = cpp8080::emulator{machine};

  try
  {
    emulator.start();
    while (true)
    {
      emulator();
      std::this_thread::sleep_for(std::chrono::milliseconds{1});
    }
  }
  catch (const cpp8080::specific::halt& h)
  {
    std::cout << h.what() << '\n';
  }
}

/*------------------------------------------------------------------------------------------------*/
