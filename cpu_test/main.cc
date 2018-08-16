#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <istream> // istreambuf_iterator
#include <vector>

#include "cpp8080/meta/instructions.hh"
#include "cpp8080/specific/cpu.hh"
#include "cpp8080/specific/instructions.hh"

/*------------------------------------------------------------------------------------------------*/

struct call_adr
  : cpp8080::meta::describe_instruction<
      cpp8080::specific::call_adr::opcode,
      cpp8080::specific::call_adr::cycles,
      cpp8080::specific::call_adr::bytes>
{
  static constexpr auto name = "call_adr";

  template <typename Machine>
  void operator()(cpp8080::specific::cpu<Machine>& cpu) const
  {
    if (((cpu.op2() << 8) | cpu.op1()) == 5)
    {
      if (cpu.c == 9)
      {
        const std::uint16_t offset = cpu.de();
        auto s = std::string{};
        for (auto i = std::uint16_t{0}; ; ++i)
        {
          const auto c = cpu.read_memory(offset + i);
          if (c == '$')
          {
            break;
          }
          s.push_back(c);
        }

        std::cout << s << '\n';
      }
      else if (cpu.c == 2)
      {
        std::cout << cpu.e;
      }
    }
    else
    {
      cpp8080::specific::call_adr{}(cpu);
    }
  }
};

/*------------------------------------------------------------------------------------------------*/

class cpu_test
{
public:

  using overrides = cpp8080::meta::make_instructions<
    call_adr
  >;

public:

  template <typename InputIterator>
  cpu_test(InputIterator first, InputIterator last)
    : cpu_{*this}
    , memory_(65536, 0)
  {
    // Test ROMS start at 0x100.
    std::copy(first, last, memory_.begin() + 0x100);
    cpu_.pc = 0x100;
  }

  void
  write_memory(std::uint16_t address, std::uint8_t value)
  {
    memory_.at(address) = value;
  }

  [[nodiscard]]
  std::uint8_t
  read_memory(std::uint16_t address)
  const
  {
    return memory_.at(address);
  }

  void
  operator()()
  {
    while (true)
    {
      cpu_.step();
      if (cpu_.pc == 0)
      {
        std::cout << '\n';
        break;
      }
    }
  }

private:

  cpp8080::specific::cpu<cpu_test> cpu_;
  std::vector<std::uint8_t> memory_;
};

/*------------------------------------------------------------------------------------------------*/

int
main(int argc, char** argv)
{
  if (argc < 2)
  {
    std::cerr << "Usage: " << argv[0] << " /path/to/rom_1 ... /path/to/rom_n\n";
    return 1;
  }

  for (auto i = 1ul; i < argc; ++i)
  {
    std::cout << "\n--------------------------\nTest with ROM " << argv[i] << '\n';

    auto file = std::ifstream{argv[i], std::ios::binary};
    if (not file.is_open())
    {
      std::cerr << "Cannot open ROM file " << argv[i] << '\n';
      return 1;
    }

    auto tester = cpu_test{
      std::istreambuf_iterator<char>{file},
      std::istreambuf_iterator<char>{}
    };

    tester();

    std::cout << "End of test with ROM " << argv[i] << "\n";
  }
}

/*------------------------------------------------------------------------------------------------*/
