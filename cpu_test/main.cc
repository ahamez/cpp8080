#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstdint>
#include <fstream>
#include <functional> // function
#include <future>     // async, future
#include <iostream>
#include <istream>    // istreambuf_iterator
#include <regex>
#include <sstream>
#include <thread>
#include <unordered_map>
#include <vector>

#include "cpp8080/meta/instructions.hh"
#include "cpp8080/specific/cpu.hh"
#include "cpp8080/util/concat.hh"

#include "md5.hh"

/*------------------------------------------------------------------------------------------------*/

struct timeout
{
  std::atomic<bool>& stop;

  template <typename Cpu, typename Instruction>
  void pre(const Cpu&, Instruction) const
  {
    if (stop)
    {
      throw std::runtime_error{"Timeout"};
    }
  }

  template <typename Cpu, typename Instruction>
  void post(const Cpu&, Instruction) const noexcept
  {}
};

/*------------------------------------------------------------------------------------------------*/

class cpu_test
{
private:

  struct call : cpp8080::meta::describe_instruction<0xcd, 11, 3>
  {
    static constexpr auto name = "call";

    void operator()(cpp8080::specific::cpu<cpu_test>& cpu) const
    {
      if (const auto operands = cpu.operands_word(); operands == 5)
      {
        if (cpu.c() == 9)
        {
          const std::uint16_t offset = cpu.de();
          auto s = std::string{};
          for (auto i = std::uint16_t{0}; ; ++i)
          {
            const auto c = cpu.memory_read_byte(offset + i);
            if (c == '$')
            {
              break;
            }
            s.push_back(c);
          }

          cpu.machine().ostream() << s << '\n';
        }
        else if (cpu.c() == 2)
        {
          cpu.machine().ostream() << cpu.e();
        }
      }
      else
      {
        cpu.call(operands);
      }
    }
  };

public:

  using overrides = cpp8080::meta::make_instructions<call>;

public:

  cpu_test(const std::vector<std::uint8_t>& rom, std::ostream& os, std::atomic<bool>& stop)
    : cpu_{*this}
    , memory_(65536, 0)
    , os_{os}
    , stop_{stop}
  {
    // Test ROMS start at 0x100.
    std::copy(begin(rom), end(rom), memory_.begin() + 0x100);
    cpu_.jump(0x100);
  }

  void
  memory_write_byte(std::uint16_t address, std::uint8_t value)
  {
    memory_.at(address) = value;
  }

  [[nodiscard]]
  std::uint8_t
  memory_read_byte(std::uint16_t address)
  const
  {
    return memory_.at(address);
  }

  void
  operator()()
  {
    while (true)
    {
      cpu_.step(timeout{stop_});
      if (cpu_.pc() == 0x0000)
      {
        break;
      }
    }
  }

  std::ostream&
  ostream()
  noexcept
  {
    return os_;
  }

private:

  cpp8080::specific::cpu<cpu_test> cpu_;
  std::vector<std::uint8_t> memory_;
  std::ostream& os_;
  std::atomic<bool>& stop_;
};

/*------------------------------------------------------------------------------------------------*/

static constexpr auto md5_8080PRE = "cdf1c368dd49e9d0b9a61081cf48c9d6";
static constexpr auto md5_8080EXM = "748ac4f67c0a1f03831f6547c101b8dc";
static constexpr auto md5_CPUTEST = "bfd431d8caf4439180bfdef190e1ed49";
static constexpr auto md5_TST8080 = "b69f2dc4c0c95935d7f71ff1cb67dbdb";

using test_result_type = std::pair<bool, std::string>;
using checker_type = std::function<test_result_type (const std::string&)>;

const auto checkers = std::unordered_map<std::string, checker_type>
{
  {md5_8080EXM, [](const auto& output)
    {
      auto match = std::smatch{};
      if (std::regex_search(output, match, std::regex{"ERROR"}))
      {
        return std::make_pair(false, output);
      }
      else
      {
        return std::make_pair(true, std::string{});
      }
    }
  },
  {md5_8080PRE, [](const auto& output)
    {
      auto match = std::smatch{};
      if (std::regex_search(output, match, std::regex{"8080 Preliminary tests complete"}))
      {
        return std::make_pair(true, std::string{});
      }
      else
      {
        return std::make_pair(false, output);
      }
    }
  },
  {md5_CPUTEST, [](const auto& output)
    {
      auto match = std::smatch{};
      if (std::regex_search(output, match, std::regex{"CPU TESTS OK"}))
      {
        return std::make_pair(true, std::string{});
      }
      else
      {
        return std::make_pair(false, output);
      }
    }
  },
  {md5_TST8080, [](const auto& output)
    {
      auto match = std::smatch{};
      if (std::regex_search(output, match, std::regex{"CPU IS OPERATIONAL"}))
      {
        return std::make_pair(true, std::string{});
      }
      else
      {
        return std::make_pair(false, output);
      }
    }
  },
};

/*------------------------------------------------------------------------------------------------*/

auto
get_checker(const std::vector<std::uint8_t>& rom)
{
  auto compute_md5 = md5{};
  const auto checksum = compute_md5.update(rom.data(), rom.size())();
  return checkers.at(md5::digest(checksum));
}

/*------------------------------------------------------------------------------------------------*/

int
main(int argc, const char** argv)
{
  if (argc < 3)
  {
    std::cerr << "Usage: " << argv[0] << " timer /path/to/rom_1 ... /path/to/rom_n\n";
    return 1;
  }

  const auto timeout = [&]
  {
    try
    {
      return std::chrono::seconds{std::stoi(argv[1])};
    }
    catch (...)
    {
      std::cerr << "Cannot convert timer parameter to an integer\n";
      std::exit(1);
    }
  }();

  auto futures = std::unordered_map<std::string, std::future<test_result_type>>{};
  auto stop = std::atomic{false};

  auto timer_future = std::async(std::launch::async, [&stop, timeout]
  {
    const auto begin = std::chrono::steady_clock::now();
    while (true)
    {
      if (const auto now = std::chrono::steady_clock::now(); now - begin > timeout)
      {
        stop = true;
        break;
      }
      if (stop)
      {
        break;
      }
      std::this_thread::sleep_for(std::chrono::milliseconds{500});
    }
  });

  for (auto i = 2ul; i < argc; ++i)
  {
    const auto filename = std::string{argv[i]};

    futures.emplace(filename, std::async(std::launch::async, [filename, &stop]() mutable
    {
      auto file = std::ifstream{filename, std::ios::binary};
      if (not file.is_open())
      {
        throw std::runtime_error{cpp8080::util::concat("Cannot open ROM file ", filename)};
      }

      const auto rom = std::vector<std::uint8_t>{
        std::istreambuf_iterator<char>{file},
        std::istreambuf_iterator<char>{}
      };

      auto oss = std::ostringstream{};
      auto tester = cpu_test{rom, oss, stop};

      try
      {
        const auto checker = get_checker(rom);
        tester();
        return checker(oss.str());
      }
      catch (const std::exception& e)
      {
        return std::make_pair(false, std::string{e.what()});
      }
      catch (...)
      {
        return std::make_pair(false, std::string{"Unknown exception"});
      }
    }));
  }

  const auto failures = std::count_if(begin(futures), end(futures), [](auto& test_future)
  {
    auto& [test_name, future] = test_future;
    if (const auto [success, msg] = future.get(); success)
    {
      return false;
    }
    else
    {
      std::cout << test_name << " failure: " << msg << '\n';
      return true;
    }
  });

  stop = true;
  timer_future.get();

  if (failures == 0)
  {
    std::cout << "All tests succeeded.\n";
    return 0;
  }
  else
  {
    std::cout << failures << " tests failed\n";
    return 1;
  }
}

/*------------------------------------------------------------------------------------------------*/
