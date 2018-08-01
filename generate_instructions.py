#! /usr/bin/env python3

import csv

cpp_struct = """
struct {0} : describe<{1}, {2}, {3}>
{{
  static constexpr auto name = "{0}";

  void operator()(state_8080&) const
  {{
    throw std::runtime_error{{"Unimplemented instruction {1}"}};
  }}
}};"""

with open("opcodes.csv") as csvfile:
  opcodes = csv.reader(csvfile, delimiter=';')
  for opcode in opcodes:
    print(cpp_struct.format(opcode[0], opcode[1], opcode[2], opcode[3]))

# with open("opcodes.csv") as csvfile:
#   opcodes = csv.reader(csvfile, delimiter=';')
#   for opcode in opcodes:
#     print("  instruction<asm8080::{}>,".format(opcode[0]))
