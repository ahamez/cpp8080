#pragma once

#include <cstdint>
#include <utility>

#include "cpp8080/meta/instruction.hh"
#include "cpp8080/meta/instructions.hh"
#include "cpp8080/meta/unimplemented.hh"
#include "cpp8080/specific/state.hh"

namespace cpp8080::specific {

/*------------------------------------------------------------------------------------------------*/

struct nop : meta::describe_instruction<0x00, 4, 1>
{
  static constexpr auto name = "nop";
  
  template <typename Machine> void operator()(state<Machine>&) const noexcept
  {}
};

struct lxi_b : meta::describe_instruction<0x01, 10, 3>
{
  static constexpr auto name = "lxi_b";
  
  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.c = state.op1();
    state.b = state.op2();
    state.pc += 2;
  }
};

struct stax_b : meta::describe_instruction<0x02, 7, 1>
{
  static constexpr auto name = "stax_b";
 
  template <typename Machine> void operator()(state<Machine>& state) const
  {
    const auto offset = (state.b << 8) | state.c;
    state.write_memory(offset, state.a);
  }
};

struct inx_b : meta::describe_instruction<0x03, 5, 1>
{
  static constexpr auto name = "inx_b";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.c += 1;
    if (state.c == 0)
    {
      state.b += 1;
    }
  }
};

struct inr_b : meta::describe_instruction<0x04, 5, 1>
{
  static constexpr auto name = "inr_b";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.b += 1;
    state.flags_zsp(state.b);
  }
};

struct dcr_b : meta::describe_instruction<0x05, 5, 1>
{
  static constexpr auto name = "dcr_b";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.b -= 1;
    state.flags_zsp(state.b);
  }
};

struct mvi_b : meta::describe_instruction<0x06, 7, 2>
{
  static constexpr auto name = "mvi_b";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.b = state.op1();
    state.pc += 1;
  }
};

struct rlc : meta::describe_instruction<0x07, 4, 1>
{
  static constexpr auto name = "rlc";
  
  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    const auto x = state.a;
    state.a = ((x & 0x80) >> 7) | (x << 1);
    state.cc.cy = (0x80 == (x & 0x80));
  }
};

struct dad_b : meta::describe_instruction<0x09, 10, 1>
{
  static constexpr auto name = "dad_b";
  
  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    const auto hl = (state.h << 8) | state.l;
    const auto bc = (state.b << 8) | state.c;
    const auto res = hl + bc;
    state.h = (res & 0xff00) >> 8;
    state.l = res & 0xff;
    state.cc.cy = ((res & 0xffff0000) != 0);
  }
};

struct ldax_b : meta::describe_instruction<0x0a, 7, 1>
{
  static constexpr auto name = "ldax_b";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    const auto offset = (state.b << 8) | state.c;
    state.a = state.read_memory(offset);
  }
};

struct dcx_b : meta::describe_instruction<0x0b, 5, 1>
{
  static constexpr auto name = "dcx_b";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.c -= 1;
    if (state.c == 0xff)
    {
      state.b -= 1;
    }
  }
};

struct inr_c : meta::describe_instruction<0x0c, 5, 1>
{
  static constexpr auto name = "inr_c";
  
  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.c += 1;
    state.flags_zsp(state.c);
  }
};

struct dcr_c : meta::describe_instruction<0x0d, 5, 1>
{
  static constexpr auto name = "dcr_c";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.c -= 1;
    state.flags_zsp(state.c);
  }
};

struct mvi_c : meta::describe_instruction<0x0e, 7, 2>
{
  static constexpr auto name = "mvi_c";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.c = state.op1();
    state.pc += 1;
  }
};

struct rrc : meta::describe_instruction<0x0f, 4, 1>
{
  static constexpr auto name = "rrc";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    const auto x = state.a;
    state.a = ((x & 1) << 7) | (x >> 1);
    state.cc.cy = (1 == (x & 1));
  }
};

struct lxi_d : meta::describe_instruction<0x11, 10, 3>
{
  static constexpr auto name = "lxi_d";
  
  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.e = state.op1();
    state.d = state.op2();
    state.pc += 2;
  }
};

struct stax_d : meta::describe_instruction<0x12, 7, 1>
{
  static constexpr auto name = "stax_d";

  template <typename Machine> void operator()(state<Machine>& state) const
  {
    const auto offset = (state.d << 8) | state.e;
    state.write_memory(offset, state.a);
  }
};

struct inx_d : meta::describe_instruction<0x13, 5, 1>
{
  static constexpr auto name = "inx_d";
  
  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.e += 1;
    if (state.e == 0)
    {
      state.d += 1;
    }
  }
};

struct inr_d : meta::describe_instruction<0x14, 5, 1>
{
  static constexpr auto name = "inr_d";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.d += 1;
    state.flags_zsp(state.d);
  }
};

struct dcr_d : meta::describe_instruction<0x15, 5, 1>
{
  static constexpr auto name = "dcr_d";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.d -= 1;
    state.flags_zsp(state.d);
  }
};

struct mvi_d : meta::describe_instruction<0x16, 7, 2>
{
  static constexpr auto name = "mvi_d";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.d = state.op1();
    state.pc += 1;
  }
};

struct ral : meta::describe_instruction<0x17, 4, 1>
{
  static constexpr auto name = "ral";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    const auto a = state.a;
    state.a = state.cc.cy | (a << 1);
    state.cc.cy = (0x80 == (a & 0x80));
  }
};

struct dad_d : meta::describe_instruction<0x19, 10, 1>
{
  static constexpr auto name = "dad_d";
  
  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    const auto hl = (state.h << 8) | state.l;
    const auto de = (state.d << 8) | state.e;
    const auto res = hl + de;
    state.h = (res & 0xff00) >> 8;
    state.l = res & 0xff;
    state.cc.cy = ((res & 0xffff0000) != 0);
  }
};

struct ldax_d : meta::describe_instruction<0x1a, 7, 1>
{
  static constexpr auto name = "ldax_d";

  template <typename Machine> void operator()(state<Machine>& state) const
  {
    const auto offset = (state.d << 8) | state.e;
    state.a = state.read_memory(offset);
  }
};

struct dcx_d : meta::describe_instruction<0x1b, 5, 1>
{
  static constexpr auto name = "dcx_d";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x1b"};
  }
};

struct inr_e : meta::describe_instruction<0x1c, 5, 1>
{
  static constexpr auto name = "inr_e";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x1c"};
  }
};

struct dcr_e : meta::describe_instruction<0x1d, 5, 1>
{
  static constexpr auto name = "dcr_e";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x1d"};
  }
};

struct mvi_e : meta::describe_instruction<0x1e, 7, 2>
{
  static constexpr auto name = "mvi_e";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x1e"};
  }
};

struct rar : meta::describe_instruction<0x1f, 4, 1>
{
  static constexpr auto name = "rar";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x1f"};
  }
};

struct rim : meta::describe_instruction<0x20, 4, 1>
{
  static constexpr auto name = "rim";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x20"};
  }
};

struct lxi_h : meta::describe_instruction<0x21, 10, 3>
{
  static constexpr auto name = "lxi_h";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.l = state.op1();
    state.h = state.op2();
    state.pc += 2;
  }
};

struct shld_adr : meta::describe_instruction<0x22, 16, 3>
{
  static constexpr auto name = "shld_adr";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x22"};
  }
};

struct inx_h : meta::describe_instruction<0x23, 5, 1>
{
  static constexpr auto name = "inx_h";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.l += 1;
    if (state.l == 0)
    {
      state.h += 1;
    }
  }
};

struct inr_h : meta::describe_instruction<0x24, 5, 1>
{
  static constexpr auto name = "inr_h";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x24"};
  }
};

struct dcr_h : meta::describe_instruction<0x25, 5, 1>
{
  static constexpr auto name = "dcr_h";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x25"};
  }
};

struct mvi_h : meta::describe_instruction<0x26, 7, 2>
{
  static constexpr auto name = "mvi_h";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.h = state.op1();
    state.pc += 1;
  }
};

struct daa : meta::describe_instruction<0x27, 4, 1>
{
  static constexpr auto name = "daa";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x27"};
  }
};

struct dad_h : meta::describe_instruction<0x29, 10, 1>
{
  static constexpr auto name = "dad_h";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    const auto hl = (state.h << 8) | state.l;
    const auto res = hl + hl;
    state.h = (res & 0xff00) >> 8;
    state.l = res & 0xff;
    state.cc.cy = ((res & 0xffff0000) != 0);
  }
};

struct lhld_adr : meta::describe_instruction<0x2a, 16, 3>
{
 static constexpr auto name = "lhld_adr";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x2a"};
  }
};

struct dcx_h : meta::describe_instruction<0x2b, 5, 1>
{
  static constexpr auto name = "dcx_h";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x2b"};
  }
};

struct inr_l : meta::describe_instruction<0x2c, 5, 1>
{
  static constexpr auto name = "inr_l";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x2c"};
  }
};

struct dcr_l : meta::describe_instruction<0x2d, 5, 1>
{
  static constexpr auto name = "dcr_l";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x2d"};
  }
};

struct mvi_l_d8 : meta::describe_instruction<0x2e, 7, 2>
{
  static constexpr auto name = "mvi_l";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x2e"};
  }
};

struct cma : meta::describe_instruction<0x2f, 4, 1>
{
  static constexpr auto name = "cma";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x2f"};
  }
};

struct sim : meta::describe_instruction<0x30, 4, 1>
{
  static constexpr auto name = "sim";
  
  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x30"};
  }
};

struct lxi_sp : meta::describe_instruction<0x31, 10, 3>
{
  static constexpr auto name = "lxi_sp";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.sp = (state.op2() << 8) | state.op1();
    state.pc += 2;
  }
};

struct sta_adr : meta::describe_instruction<0x32, 13, 3>
{
  static constexpr auto name = "sta_adr";

  template <typename Machine> void operator()(state<Machine>& state) const
  {
    const auto offset = (state.op2() << 8) | state.op1();
    state.write_memory(offset, state.a);
    state.pc += 2;
  }
};

struct inx_sp : meta::describe_instruction<0x33, 5, 1>
{
  static constexpr auto name = "inx_sp";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x33"};
  }
};

struct inr_m : meta::describe_instruction<0x34, 10, 1>
{
  static constexpr auto name = "inr_m";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x34"};
  }
};

struct dcr_m : meta::describe_instruction<0x35, 10, 1>
{
  static constexpr auto name = "dcr_m";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x35"};
  }
};

struct mvi_m : meta::describe_instruction<0x36, 10, 2>
{
  static constexpr auto name = "mvi_m";

  template <typename Machine> void operator()(state<Machine>& state) const
  {
    state.write_hl(state.op1());
    state.pc += 1;
  }
};

struct stc : meta::describe_instruction<0x37, 4, 1>
{
  static constexpr auto name = "stc";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x37"};
  }
};

struct dad_sp : meta::describe_instruction<0x39, 10, 1>
{
  static constexpr auto name = "dad_sp";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x39"};
  }
};

struct lda_adr : meta::describe_instruction<0x3a, 13, 3>
{
  static constexpr auto name = "lda_adr";

  template <typename Machine> void operator()(state<Machine>& state) const
  {
    const auto offset = (state.op2() << 8) | state.op1();
    state.a = state.read_memory(offset);
    state.pc += 2;
  }
};

struct dcx_sp : meta::describe_instruction<0x3b, 5, 1>
{
  static constexpr auto name = "dcx_sp";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x3b"};
  }
};

struct inr_a : meta::describe_instruction<0x3c, 5, 1>
{
  static constexpr auto name = "inr_a";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x3c"};
  }
};

struct dcr_a : meta::describe_instruction<0x3d, 5, 1>
{
  static constexpr auto name = "dcr_a";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x3d"};
  }
};

struct mvi_a : meta::describe_instruction<0x3e, 7, 2>
{
  static constexpr auto name = "mvi_a";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.a = state.op1();
    state.pc += 1;
  }
};

struct cmc : meta::describe_instruction<0x3f, 4, 1>
{
  static constexpr auto name = "cmc";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x3f"};
  }
};

struct mov_b_b : meta::describe_instruction<0x40, 5, 1>
{
  static constexpr auto name = "mov_b_b";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x40"};
  }
};

struct mov_b_c : meta::describe_instruction<0x41, 5, 1>
{
  static constexpr auto name = "mov_b_c";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x41"};
  }
};

struct mov_b_d : meta::describe_instruction<0x42, 5, 1>
{
  static constexpr auto name = "mov_b_d";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x42"};
  }
};

struct mov_b_e : meta::describe_instruction<0x43, 5, 1>
{
  static constexpr auto name = "mov_b_e";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x43"};
  }
};

struct mov_b_h : meta::describe_instruction<0x44, 5, 1>
{
  static constexpr auto name = "mov_b_h";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x44"};
  }
};

struct mov_b_l : meta::describe_instruction<0x45, 5, 1>
{
  static constexpr auto name = "mov_b_l";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x45"};
  }
};

struct mov_b_m : meta::describe_instruction<0x46, 7, 1>
{
  static constexpr auto name = "mov_b_m";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x46"};
  }
};

struct mov_b_a : meta::describe_instruction<0x47, 5, 1>
{
  static constexpr auto name = "mov_b_a";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x47"};
  }
};

struct mov_c_b : meta::describe_instruction<0x48, 5, 1>
{
  static constexpr auto name = "mov_c_b";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x48"};
  }
};

struct mov_c_c : meta::describe_instruction<0x49, 5, 1>
{
  static constexpr auto name = "mov_c_c";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x49"};
  }
};

struct mov_c_d : meta::describe_instruction<0x4a, 5, 1>
{
  static constexpr auto name = "mov_c_d";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x4a"};
  }
};

struct mov_c_e : meta::describe_instruction<0x4b, 5, 1>
{
  static constexpr auto name = "mov_c_e";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x4b"};
  }
};

struct mov_c_h : meta::describe_instruction<0x4c, 5, 1>
{
  static constexpr auto name = "mov_c_h";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x4c"};
  }
};

struct mov_c_l : meta::describe_instruction<0x4d, 5, 1>
{
  static constexpr auto name = "mov_c_l";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x4d"};
  }
};

struct mov_c_m : meta::describe_instruction<0x4e, 7, 1>
{
  static constexpr auto name = "mov_c_m";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x4e"};
  }
};

struct mov_c_a : meta::describe_instruction<0x4f, 5, 1>
{
  static constexpr auto name = "mov_c_a";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x4f"};
  }
};

struct mov_d_b : meta::describe_instruction<0x50, 5, 1>
{
  static constexpr auto name = "mov_d_b";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x50"};
  }
};

struct mov_d_c : meta::describe_instruction<0x51, 5, 1>
{
  static constexpr auto name = "mov_d_c";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x51"};
  }
};

struct mov_d_d : meta::describe_instruction<0x52, 5, 1>
{
  static constexpr auto name = "mov_d_d";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x52"};
  }
};

struct mov_d_e : meta::describe_instruction<0x53, 5, 1>
{
  static constexpr auto name = "mov_d_e";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x53"};
  }
};

struct mov_d_h : meta::describe_instruction<0x54, 5, 1>
{
  static constexpr auto name = "mov_d_h";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x54"};
  }
};

struct mov_d_l : meta::describe_instruction<0x55, 5, 1>
{
  static constexpr auto name = "mov_d_l";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x55"};
  }
};

struct mov_d_m : meta::describe_instruction<0x56, 7, 1>
{
  static constexpr auto name = "mov_d_m";

  template <typename Machine> void operator()(state<Machine>& state) const
  {
    state.d = state.read_hl();
  }
};

struct mov_d_a : meta::describe_instruction<0x57, 5, 1>
{
  static constexpr auto name = "mov_d_a";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x57"};
  }
};

struct mov_e_b : meta::describe_instruction<0x58, 5, 1>
{
  static constexpr auto name = "mov_e_b";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x58"};
  }
};

struct mov_e_c : meta::describe_instruction<0x59, 5, 1>
{
  static constexpr auto name = "mov_e_c";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x59"};
  }
};

struct mov_e_d : meta::describe_instruction<0x5a, 5, 1>
{
  static constexpr auto name = "mov_e_d";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x5a"};
  }
};

struct mov_e_e : meta::describe_instruction<0x5b, 5, 1>
{
  static constexpr auto name = "mov_e_e";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x5b"};
  }
};

struct mov_e_h : meta::describe_instruction<0x5c, 5, 1>
{
  static constexpr auto name = "mov_e_h";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x5c"};
  }
};

struct mov_e_l : meta::describe_instruction<0x5d, 5, 1>
{
  static constexpr auto name = "mov_e_l";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x5d"};
  }
};

struct mov_e_m : meta::describe_instruction<0x5e, 7, 1>
{
  static constexpr auto name = "mov_e_m";

  template <typename Machine> void operator()(state<Machine>& state) const
  {
    state.e = state.read_hl();
  }
};

struct mov_e_a : meta::describe_instruction<0x5f, 5, 1>
{
  static constexpr auto name = "mov_e_a";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x5f"};
  }
};

struct mov_h_b : meta::describe_instruction<0x60, 5, 1>
{
  static constexpr auto name = "mov_h_b";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x60"};
  }
};

struct mov_h_c : meta::describe_instruction<0x61, 5, 1>
{
  static constexpr auto name = "mov_h_c";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x61"};
  }
};

struct mov_h_d : meta::describe_instruction<0x62, 5, 1>
{
  static constexpr auto name = "mov_h_d";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x62"};
  }
};

struct mov_h_e : meta::describe_instruction<0x63, 5, 1>
{
  static constexpr auto name = "mov_h_e";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x63"};
  }
};

struct mov_h_h : meta::describe_instruction<0x64, 5, 1>
{
  static constexpr auto name = "mov_h_h";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x64"};
  }
};

struct mov_h_l : meta::describe_instruction<0x65, 5, 1>
{
  static constexpr auto name = "mov_h_l";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x65"};
  }
};

struct mov_h_m : meta::describe_instruction<0x66, 7, 1>
{
  static constexpr auto name = "mov_h_m";

  template <typename Machine> void operator()(state<Machine>& state) const
  {
    state.h = state.read_hl();
  }
};

struct mov_h_a : meta::describe_instruction<0x67, 5, 1>
{
  static constexpr auto name = "mov_h_a";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x67"};
  }
};

struct mov_l_b : meta::describe_instruction<0x68, 5, 1>
{
  static constexpr auto name = "mov_l_b";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x68"};
  }
};

struct mov_l_c : meta::describe_instruction<0x69, 5, 1>
{
  static constexpr auto name = "mov_l_c";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x69"};
  }
};

struct mov_l_d : meta::describe_instruction<0x6a, 5, 1>
{
  static constexpr auto name = "mov_l_d";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x6a"};
  }
};

struct mov_l_e : meta::describe_instruction<0x6b, 5, 1>
{
  static constexpr auto name = "mov_l_e";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x6b"};
  }
};

struct mov_l_h : meta::describe_instruction<0x6c, 5, 1>
{
  static constexpr auto name = "mov_l_h";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x6c"};
  }
};

struct mov_l_l : meta::describe_instruction<0x6d, 5, 1>
{
  static constexpr auto name = "mov_l_l";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x6d"};
  }
};

struct mov_l_m : meta::describe_instruction<0x6e, 7, 1>
{
  static constexpr auto name = "mov_l_m";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x6e"};
  }
};

struct mov_l_a : meta::describe_instruction<0x6f, 5, 1>
{
  static constexpr auto name = "mov_l_a";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.l = state.a;
  }
};

struct mov_m_b : meta::describe_instruction<0x70, 7, 1>
{
  static constexpr auto name = "mov_m_b";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x70"};
  }
};

struct mov_m_c : meta::describe_instruction<0x71, 7, 1>
{
  static constexpr auto name = "mov_m_c";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x71"};
  }
};

struct mov_m_d : meta::describe_instruction<0x72, 7, 1>
{
  static constexpr auto name = "mov_m_d";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x72"};
  }
};

struct mov_m_e : meta::describe_instruction<0x73, 7, 1>
{
  static constexpr auto name = "mov_m_e";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x73"};
  }
};

struct mov_m_h : meta::describe_instruction<0x74, 7, 1>
{
  static constexpr auto name = "mov_m_h";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x74"};
  }
};

struct mov_m_l : meta::describe_instruction<0x75, 7, 1>
{
  static constexpr auto name = "mov_m_l";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x75"};
  }
};

struct hlt : meta::describe_instruction<0x76, 7, 1>
{
  static constexpr auto name = "hlt";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x76"};
  }
};

struct mov_m_a : meta::describe_instruction<0x77, 7, 1>
{
  static constexpr auto name = "mov_m_a";

  template <typename Machine> void operator()(state<Machine>& state) const
  {
    state.write_hl(state.a);
  }
};

struct mov_a_b : meta::describe_instruction<0x78, 5, 1>
{
  static constexpr auto name = "mov_a_b";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x78"};
  }
};

struct mov_a_c : meta::describe_instruction<0x79, 5, 1>
{
  static constexpr auto name = "mov_a_c";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x79"};
  }
};

struct mov_a_d : meta::describe_instruction<0x7a, 5, 1>
{
  static constexpr auto name = "mov_a_d";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.a = state.d;
  }
};

struct mov_a_e : meta::describe_instruction<0x7b, 5, 1>
{
  static constexpr auto name = "mov_a_e";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.a = state.e;
  }
};

struct mov_a_h : meta::describe_instruction<0x7c, 5, 1>
{
  static constexpr auto name = "mov_a_h";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.a = state.h;
  }
};

struct mov_a_l : meta::describe_instruction<0x7d, 5, 1>
{
  static constexpr auto name = "mov_a_l";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x7d"};
  }
};

struct mov_a_m : meta::describe_instruction<0x7e, 7, 1>
{
  static constexpr auto name = "mov_a_m";

  template <typename Machine> void operator()(state<Machine>& state) const
  {
    state.a = state.read_hl();
  }
};

struct mov_a_a : meta::describe_instruction<0x7f, 5, 1>
{
  static constexpr auto name = "mov_a_a";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x7f"};
  }
};

struct add_b : meta::describe_instruction<0x80, 4, 1>
{
  static constexpr auto name = "add_b";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x80"};
  }
};

struct add_c : meta::describe_instruction<0x81, 4, 1>
{
  static constexpr auto name = "add_c";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x81"};
  }
};

struct add_d : meta::describe_instruction<0x82, 4, 1>
{
  static constexpr auto name = "add_d";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x82"};
  }
};

struct add_e : meta::describe_instruction<0x83, 4, 1>
{
  static constexpr auto name = "add_e";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x83"};
  }
};

struct add_h : meta::describe_instruction<0x84, 4, 1>
{
  static constexpr auto name = "add_h";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x84"};
  }
};

struct add_l : meta::describe_instruction<0x85, 4, 1>
{
  static constexpr auto name = "add_l";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x85"};
  }
};

struct add_m : meta::describe_instruction<0x86, 7, 1>
{
  static constexpr auto name = "add_m";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x86"};
  }
};

struct add_a : meta::describe_instruction<0x87, 4, 1>
{
  static constexpr auto name = "add_a";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x87"};
  }
};

struct adc_b : meta::describe_instruction<0x88, 4, 1>
{
  static constexpr auto name = "adc_b";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x88"};
  }
};

struct adc_c : meta::describe_instruction<0x89, 4, 1>
{
  static constexpr auto name = "adc_c";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x89"};
  }
};

struct adc_d : meta::describe_instruction<0x8a, 4, 1>
{
  static constexpr auto name = "adc_d";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x8a"};
  }
};

struct adc_e : meta::describe_instruction<0x8b, 4, 1>
{
  static constexpr auto name = "adc_e";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x8b"};
  }
};

struct adc_h : meta::describe_instruction<0x8c, 4, 1>
{
  static constexpr auto name = "adc_h";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x8c"};
  }
};

struct adc_l : meta::describe_instruction<0x8d, 4, 1>
{
  static constexpr auto name = "adc_l";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x8d"};
  }
};

struct adc_m : meta::describe_instruction<0x8e, 7, 1>
{
  static constexpr auto name = "adc_m";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x8e"};
  }
};

struct adc_a : meta::describe_instruction<0x8f, 4, 1>
{
  static constexpr auto name = "adc_a";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x8f"};
  }
};

struct sub_b : meta::describe_instruction<0x90, 4, 1>
{
  static constexpr auto name = "sub_b";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x90"};
  }
};

struct sub_c : meta::describe_instruction<0x91, 4, 1>
{
  static constexpr auto name = "sub_c";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x91"};
  }
};

struct sub_d : meta::describe_instruction<0x92, 4, 1>
{
  static constexpr auto name = "sub_d";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x92"};
  }
};

struct sub_e : meta::describe_instruction<0x93, 4, 1>
{
  static constexpr auto name = "sub_e";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x93"};
  }
};

struct sub_h : meta::describe_instruction<0x94, 4, 1>
{
  static constexpr auto name = "sub_h";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x94"};
  }
};

struct sub_l : meta::describe_instruction<0x95, 4, 1>
{
  static constexpr auto name = "sub_l";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x95"};
  }
};

struct sub_m : meta::describe_instruction<0x96, 7, 1>
{
  static constexpr auto name = "sub_m";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x96"};
  }
};

struct sub_a : meta::describe_instruction<0x97, 4, 1>
{
  static constexpr auto name = "sub_a";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x97"};
  }
};

struct sbb_b : meta::describe_instruction<0x98, 4, 1>
{
  static constexpr auto name = "sbb_b";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x98"};
  }
};

struct sbb_c : meta::describe_instruction<0x99, 4, 1>
{
  static constexpr auto name = "sbb_c";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x99"};
  }
};

struct sbb_d : meta::describe_instruction<0x9a, 4, 1>
{
  static constexpr auto name = "sbb_d";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x9a"};
  }
};

struct sbb_e : meta::describe_instruction<0x9b, 4, 1>
{
  static constexpr auto name = "sbb_e";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x9b"};
  }
};

struct sbb_h : meta::describe_instruction<0x9c, 4, 1>
{
  static constexpr auto name = "sbb_h";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x9c"};
  }
};

struct sbb_l : meta::describe_instruction<0x9d, 4, 1>
{
  static constexpr auto name = "sbb_l";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x9d"};
  }
};

struct sbb_m : meta::describe_instruction<0x9e, 7, 1>
{
  static constexpr auto name = "sbb_m";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x9e"};
  }
};

struct sbb_a : meta::describe_instruction<0x9f, 4, 1>
{
  static constexpr auto name = "sbb_a";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0x9f"};
  }
};

struct ana_b : meta::describe_instruction<0xa0, 4, 1>
{
  static constexpr auto name = "ana_b";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0xa0"};
  }
};

struct ana_c : meta::describe_instruction<0xa1, 4, 1>
{
  static constexpr auto name = "ana_c";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0xa1"};
  }
};

struct ana_d : meta::describe_instruction<0xa2, 4, 1>
{
  static constexpr auto name = "ana_d";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0xa2"};
  }
};

struct ana_e : meta::describe_instruction<0xa3, 4, 1>
{
  static constexpr auto name = "ana_e";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0xa3"};
  }
};

struct ana_h : meta::describe_instruction<0xa4, 4, 1>
{
  static constexpr auto name = "ana_h";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0xa4"};
  }
};

struct ana_l : meta::describe_instruction<0xa5, 4, 1>
{
  static constexpr auto name = "ana_l";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0xa5"};
  }
};

struct ana_m : meta::describe_instruction<0xa6, 7, 1>
{
  static constexpr auto name = "ana_m";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0xa6"};
  }
};

struct ana_a : meta::describe_instruction<0xa7, 4, 1>
{
  static constexpr auto name = "ana_a";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.a = state.a & state.a;
    state.logic_flags_a();
  }
};

struct xra_b : meta::describe_instruction<0xa8, 4, 1>
{
  static constexpr auto name = "xra_b";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0xa8"};
  }
};

struct xra_c : meta::describe_instruction<0xa9, 4, 1>
{
  static constexpr auto name = "xra_c";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0xa9"};
  }
};

struct xra_d : meta::describe_instruction<0xaa, 4, 1>
{
  static constexpr auto name = "xra_d";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0xaa"};
  }
};

struct xra_e : meta::describe_instruction<0xab, 4, 1>
{
  static constexpr auto name = "xra_e";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0xab"};
  }
};

struct xra_h : meta::describe_instruction<0xac, 4, 1>
{
  static constexpr auto name = "xra_h";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0xac"};
  }
};

struct xra_l : meta::describe_instruction<0xad, 4, 1>
{
  static constexpr auto name = "xra_l";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0xad"};
  }
};

struct xra_m : meta::describe_instruction<0xae, 7, 1>
{
  static constexpr auto name = "xra_m";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0xae"};
  }
};

struct xra_a : meta::describe_instruction<0xaf, 4, 1>
{
  static constexpr auto name = "xra_a";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.a = state.a ^ state.a;
    state.logic_flags_a();
  }
};

struct ora_b : meta::describe_instruction<0xb0, 4, 1>
{
  static constexpr auto name = "ora_b";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0xb0"};
  }
};

struct ora_c : meta::describe_instruction<0xb1, 4, 1>
{
  static constexpr auto name = "ora_c";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0xb1"};
  }
};

struct ora_d : meta::describe_instruction<0xb2, 4, 1>
{
  static constexpr auto name = "ora_d";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0xb2"};
  }
};

struct ora_e : meta::describe_instruction<0xb3, 4, 1>
{
  static constexpr auto name = "ora_e";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0xb3"};
  }
};

struct ora_h : meta::describe_instruction<0xb4, 4, 1>
{
  static constexpr auto name = "ora_h";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0xb4"};
  }
};

struct ora_l : meta::describe_instruction<0xb5, 4, 1>
{
  static constexpr auto name = "ora_l";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0xb5"};
  }
};

struct ora_m : meta::describe_instruction<0xb6, 7, 1>
{
  static constexpr auto name = "ora_m";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0xb6"};
  }
};

struct ora_a : meta::describe_instruction<0xb7, 4, 1>
{
  static constexpr auto name = "ora_a";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0xb7"};
  }
};

struct cmp_b : meta::describe_instruction<0xb8, 4, 1>
{
  static constexpr auto name = "cmp_b";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0xb8"};
  }
};

struct cmp_c : meta::describe_instruction<0xb9, 4, 1>
{
  static constexpr auto name = "cmp_c";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0xb9"};
  }
};

struct cmp_d : meta::describe_instruction<0xba, 4, 1>
{
  static constexpr auto name = "cmp_d";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0xba"};
  }
};

struct cmp_e : meta::describe_instruction<0xbb, 4, 1>
{
  static constexpr auto name = "cmp_e";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0xbb"};
  }
};

struct cmp_h : meta::describe_instruction<0xbc, 4, 1>
{
  static constexpr auto name = "cmp_h";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0xbc"};
  }
};

struct cmp_l : meta::describe_instruction<0xbd, 4, 1>
{
  static constexpr auto name = "cmp_l";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0xbd"};
  }
};

struct cmp_m : meta::describe_instruction<0xbe, 7, 1>
{
  static constexpr auto name = "cmp_m";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0xbe"};
  }
};

struct cmp_a : meta::describe_instruction<0xbf, 4, 1>
{
  static constexpr auto name = "cmp_a";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0xbf"};
  }
};

struct rnz : meta::describe_instruction<0xc0, 11, 1>
{
  static constexpr auto name = "rnz";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0xc0"};
  }
};

struct pop_b : meta::describe_instruction<0xc1, 10, 1>
{
  static constexpr auto name = "pop_b";

  template <typename Machine> void operator()(state<Machine>& state) const
  {
    state.pop(state.b, state.c);
  }
};

struct jnz_adr : meta::describe_instruction<0xc2, 10, 3>
{
  static constexpr auto name = "jnz_adr";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    if (state.cc.z == 0)
    {
      state.pc = (state.op2() << 8) | state.op1();
    }
    else
    {
      state.pc += 2;
    }
  }
};

struct jmp_adr : meta::describe_instruction<0xc3, 10, 3>
{
  static constexpr auto name = "jmp_adr";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.pc = (state.op2() << 8) | state.op1();
  }
};

struct cnz_adr : meta::describe_instruction<0xc4, 17, 3>
{
  static constexpr auto name = "cnz_adr";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0xc4"};
  }
};

struct push_b : meta::describe_instruction<0xc5, 11, 1>
{
  static constexpr auto name = "push_b";

  template <typename Machine> void operator()(state<Machine>& state) const
  {
    state.push(state.b, state.c);
  }
};

struct adi : meta::describe_instruction<0xc6, 7, 2>
{
  static constexpr auto name = "adi";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    const auto x = static_cast<std::uint16_t>(state.a) + static_cast<std::uint16_t>(state.op1());
    state.flags_zsp(x & 0xff);
    state.cc.cy = (x > 0xff);
    state.a = x & 0xff;
    state.pc += 1;
  }
};

struct rst_0 : meta::describe_instruction<0xc7, 11, 1>
{
  static constexpr auto name = "rst_0";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0xc7"};
  }
};

struct rz : meta::describe_instruction<0xc8, 11, 1>
{
  static constexpr auto name = "rz";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0xc8"};
  }
};

struct ret : meta::describe_instruction<0xc9, 10, 1>
{
  static constexpr auto name = "ret";

  template <typename Machine> void operator()(state<Machine>& state) const
  {
    state.pc = state.read_memory(state.sp) | (state.read_memory(state.sp + 1) << 8);
    state.sp += 2;
  }
};

struct jz_adr : meta::describe_instruction<0xca, 10, 3>
{
  static constexpr auto name = "jz_adr";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0xca"};
  }
};

struct cz_adr : meta::describe_instruction<0xcc, 10, 3>
{
  static constexpr auto name = "cz_adr";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0xcc"};
  }
};

struct call_adr : meta::describe_instruction<0xcd, 17, 3>
{
  static constexpr auto name = "call_adr";

  template <typename Machine> void operator()(state<Machine>& state) const
  {
    const auto ret = state.pc + 2;
    state.write_memory(state.sp - 1, (ret >> 8) & 0xff);
    state.write_memory(state.sp - 2, ret & 0xff);
    state.sp -= 2;
    state.pc = (state.op2() << 8) | state.op1();
  }
};

struct aci_d8 : meta::describe_instruction<0xce, 7, 2>
{
  static constexpr auto name = "aci_d8";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0xce"};
  }
};

struct rst_1 : meta::describe_instruction<0xcf, 11, 1>
{
  static constexpr auto name = "rst_1";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0xcf"};
  }
};

struct rnc : meta::describe_instruction<0xd0, 11, 1>
{
  static constexpr auto name = "rnc";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0xd0"};
  }
};

struct pop_d : meta::describe_instruction<0xd1, 10, 1>
{
  static constexpr auto name = "pop_d";

  template <typename Machine> void operator()(state<Machine>& state) const
  {
    state.pop(state.d, state.e);
  }
};

struct jnc_adr : meta::describe_instruction<0xd2, 10, 3>
{
  static constexpr auto name = "jnc_adr";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0xd2"};
  }
};

struct out : meta::describe_instruction<0xd3, 10, 2>
{
  static constexpr auto name = "out";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.pc += 1;
  }
};

struct cnc_adr : meta::describe_instruction<0xd4, 17, 3>
{
  static constexpr auto name = "cnc_adr";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0xd4"};
  }
};

struct push_d : meta::describe_instruction<0xd5, 11, 1>
{
  static constexpr auto name = "push_d";

  template <typename Machine> void operator()(state<Machine>& state) const
  {
    state.push(state.d, state.e);
  }
};

struct sui_d8 : meta::describe_instruction<0xd6, 7, 2>
{
  static constexpr auto name = "sui_d8";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0xd6"};
  }
};

struct rst_2 : meta::describe_instruction<0xd7, 11, 1>
{
  static constexpr auto name = "rst_2";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0xd7"};
  }
};

struct rc : meta::describe_instruction<0xd8, 11, 1>
{
  static constexpr auto name = "rc";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0xd8"};
  }
};

struct jc_adr : meta::describe_instruction<0xda, 10, 3>
{
  static constexpr auto name = "jc_adr";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0xda"};
  }
};

struct in : meta::describe_instruction<0xdb, 10, 2>
{
  static constexpr auto name = "in_d8";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0xdb"};
  }
};

struct cc_adr : meta::describe_instruction<0xdc, 10, 3>
{
  static constexpr auto name = "cc_adr";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0xdc"};
  }
};

struct sbi_d8 : meta::describe_instruction<0xde, 7, 2>
{
  static constexpr auto name = "sbi_d8";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0xde"};
  }
};

struct rst_3 : meta::describe_instruction<0xdf, 11, 1>
{
  static constexpr auto name = "rst_3";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0xdf"};
  }
};

struct rpo : meta::describe_instruction<0xe0, 11, 1>
{
  static constexpr auto name = "rpo";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0xe0"};
  }
};

struct pop_h : meta::describe_instruction<0xe1, 10, 1>
{
  static constexpr auto name = "pop_h";

  template <typename Machine> void operator()(state<Machine>& state) const
  {
    state.pop(state.h, state.l);
  }
};

struct jpo_adr : meta::describe_instruction<0xe2, 10, 3>
{
  static constexpr auto name = "jpo_adr";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0xe2"};
  }
};

struct xthl : meta::describe_instruction<0xe3, 18, 1>
{
  static constexpr auto name = "xthl";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0xe3"};
  }
};

struct cpo_adr : meta::describe_instruction<0xe4, 17, 3>
{
  static constexpr auto name = "cpo_adr";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0xe4"};
  }
};

struct push_h : meta::describe_instruction<0xe5, 11, 1>
{
  static constexpr auto name = "push_h";

  template <typename Machine> void operator()(state<Machine>& state) const
  {
    state.push(state.h, state.l);
  }
};

struct ani_d8 : meta::describe_instruction<0xe6, 7, 2>
{
  static constexpr auto name = "ani_d8";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.a = state.a & state.op1();
    state.logic_flags_a();
    state.pc += 1;
  }
};

struct rst_4 : meta::describe_instruction<0xe7, 11, 1>
{
  static constexpr auto name = "rst_4";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0xe7"};
  }
};

struct rpe : meta::describe_instruction<0xe8, 11, 1>
{
  static constexpr auto name = "rpe";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0xe8"};
  }
};

struct pchl : meta::describe_instruction<0xe9, 5, 1>
{
  static constexpr auto name = "pchl";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0xe9"};
  }
};

struct jpe_adr : meta::describe_instruction<0xea, 10, 3>
{
  static constexpr auto name = "jpe_adr";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0xea"};
  }
};

struct xchg : meta::describe_instruction<0xeb, 5, 1>
{
  static constexpr auto name = "xchg";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    std::swap(state.d, state.h);
    std::swap(state.e, state.l);
  }
};

struct cpe_adr : meta::describe_instruction<0xec, 17, 3>
{
  static constexpr auto name = "cpe_adr";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0xec"};
  }
};

struct xri_d8 : meta::describe_instruction<0xee, 7, 2>
{
  static constexpr auto name = "xri_d8";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0xee"};
  }
};

struct rst_5 : meta::describe_instruction<0xef, 11, 1>
{
  static constexpr auto name = "rst_5";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0xef"};
  }
};

struct rp : meta::describe_instruction<0xf0, 11, 1>
{
  static constexpr auto name = "rp";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0xf0"};
  }
};

struct pop_psw : meta::describe_instruction<0xf1, 10, 1>
{
  static constexpr auto name = "pop_psw";

  template <typename Machine> void operator()(state<Machine>& state) const
  {
    state.pop(state.a, *reinterpret_cast<std::uint8_t*>(&state.cc));
  }
};

struct jp_adr : meta::describe_instruction<0xf2, 10, 3>
{
  static constexpr auto name = "jp_adr";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0xf2"};
  }
};

struct di : meta::describe_instruction<0xf3, 4, 1>
{
  static constexpr auto name = "di";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0xf3"};
  }
};

struct cp_adr : meta::describe_instruction<0xf4, 17, 3>
{
  static constexpr auto name = "cp_adr";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0xf4"};
  }
};

struct push_psw : meta::describe_instruction<0xf5, 11, 1>
{
  static constexpr auto name = "push_psw";

  template <typename Machine> void operator()(state<Machine>& state) const
  {
    state.push(state.a, *reinterpret_cast<std::uint8_t*>(&state.cc));
  }
};

struct ori : meta::describe_instruction<0xf6, 7, 2>
{
  static constexpr auto name = "ori";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0xf6"};
  }
};

struct rst_6 : meta::describe_instruction<0xf7, 11, 1>
{
  static constexpr auto name = "rst_6";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0xf7"};
  }
};

struct rm : meta::describe_instruction<0xf8, 11, 1>
{
  static constexpr auto name = "rm";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0xf8"};
  }
};

struct sphl : meta::describe_instruction<0xf9, 5, 1>
{
  static constexpr auto name = "sphl";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0xf9"};
  }
};

struct jm_adr : meta::describe_instruction<0xfa, 10, 3>
{
  static constexpr auto name = "jm_adr";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0xfa"};
  }
};

struct ei : meta::describe_instruction<0xfb, 4, 1>
{
  static constexpr auto name = "ei";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.int_enable = 1;
  }
};

struct cm_adr : meta::describe_instruction<0xfc, 17, 3>
{
  static constexpr auto name = "cm_adr";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0xfc"};
  }
};

struct cpi : meta::describe_instruction<0xfe, 7, 2>
{
  static constexpr auto name = "cpi";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    const auto x = state.a - state.op1();
    state.flags_zsp(x);
    state.cc.cy = (state.a < state.op1());
    state.pc += 1;
  }
};

struct rst_7 : meta::describe_instruction<0xff, 11, 1>
{
  static constexpr auto name = "rst_7";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw std::runtime_error{"Unimplemented instruction 0xff"};
  }
};

/*------------------------------------------------------------------------------------------------*/

using instructions_8080 = meta::instructions<
  meta::instruction<nop>,
  meta::instruction<lxi_b>,
  meta::instruction<stax_b>,
  meta::instruction<inx_b>,
  meta::instruction<inr_b>,
  meta::instruction<dcr_b>,
  meta::instruction<mvi_b>,
  meta::instruction<rlc>,
  meta::instruction<meta::unimplemented<0x08>>,
  meta::instruction<dad_b>,
  meta::instruction<ldax_b>,
  meta::instruction<dcx_b>,
  meta::instruction<inr_c>,
  meta::instruction<dcr_c>,
  meta::instruction<mvi_c>,
  meta::instruction<rrc>,
  meta::instruction<meta::unimplemented<0x10>>,
  meta::instruction<lxi_d>,
  meta::instruction<stax_d>,
  meta::instruction<inx_d>,
  meta::instruction<inr_d>,
  meta::instruction<dcr_d>,
  meta::instruction<mvi_d>,
  meta::instruction<ral>,
  meta::instruction<meta::unimplemented<0x18>>,
  meta::instruction<dad_d>,
  meta::instruction<ldax_d>,
  meta::instruction<dcx_d>,
  meta::instruction<inr_e>,
  meta::instruction<dcr_e>,
  meta::instruction<mvi_e>,
  meta::instruction<rar>,
  meta::instruction<rim>,
  meta::instruction<lxi_h>,
  meta::instruction<shld_adr>,
  meta::instruction<inx_h>,
  meta::instruction<inr_h>,
  meta::instruction<dcr_h>,
  meta::instruction<mvi_h>,
  meta::instruction<daa>,
  meta::instruction<meta::unimplemented<0x28>>,
  meta::instruction<dad_h>,
  meta::instruction<lhld_adr>,
  meta::instruction<dcx_h>,
  meta::instruction<inr_l>,
  meta::instruction<dcr_l>,
  meta::instruction<mvi_l_d8>,
  meta::instruction<cma>,
  meta::instruction<sim>,
  meta::instruction<lxi_sp>,
  meta::instruction<sta_adr>,
  meta::instruction<inx_sp>,
  meta::instruction<inr_m>,
  meta::instruction<dcr_m>,
  meta::instruction<mvi_m>,
  meta::instruction<stc>,
  meta::instruction<meta::unimplemented<0x38>>,
  meta::instruction<dad_sp>,
  meta::instruction<lda_adr>,
  meta::instruction<dcx_sp>,
  meta::instruction<inr_a>,
  meta::instruction<dcr_a>,
  meta::instruction<mvi_a>,
  meta::instruction<cmc>,
  meta::instruction<mov_b_b>,
  meta::instruction<mov_b_c>,
  meta::instruction<mov_b_d>,
  meta::instruction<mov_b_e>,
  meta::instruction<mov_b_h>,
  meta::instruction<mov_b_l>,
  meta::instruction<mov_b_m>,
  meta::instruction<mov_b_a>,
  meta::instruction<mov_c_b>,
  meta::instruction<mov_c_c>,
  meta::instruction<mov_c_d>,
  meta::instruction<mov_c_e>,
  meta::instruction<mov_c_h>,
  meta::instruction<mov_c_l>,
  meta::instruction<mov_c_m>,
  meta::instruction<mov_c_a>,
  meta::instruction<mov_d_b>,
  meta::instruction<mov_d_c>,
  meta::instruction<mov_d_d>,
  meta::instruction<mov_d_e>,
  meta::instruction<mov_d_h>,
  meta::instruction<mov_d_l>,
  meta::instruction<mov_d_m>,
  meta::instruction<mov_d_a>,
  meta::instruction<mov_e_b>,
  meta::instruction<mov_e_c>,
  meta::instruction<mov_e_d>,
  meta::instruction<mov_e_e>,
  meta::instruction<mov_e_h>,
  meta::instruction<mov_e_l>,
  meta::instruction<mov_e_m>,
  meta::instruction<mov_e_a>,
  meta::instruction<mov_h_b>,
  meta::instruction<mov_h_c>,
  meta::instruction<mov_h_d>,
  meta::instruction<mov_h_e>,
  meta::instruction<mov_h_h>,
  meta::instruction<mov_h_l>,
  meta::instruction<mov_h_m>,
  meta::instruction<mov_h_a>,
  meta::instruction<mov_l_b>,
  meta::instruction<mov_l_c>,
  meta::instruction<mov_l_d>,
  meta::instruction<mov_l_e>,
  meta::instruction<mov_l_h>,
  meta::instruction<mov_l_l>,
  meta::instruction<mov_l_m>,
  meta::instruction<mov_l_a>,
  meta::instruction<mov_m_b>,
  meta::instruction<mov_m_c>,
  meta::instruction<mov_m_d>,
  meta::instruction<mov_m_e>,
  meta::instruction<mov_m_h>,
  meta::instruction<mov_m_l>,
  meta::instruction<hlt>,
  meta::instruction<mov_m_a>,
  meta::instruction<mov_a_b>,
  meta::instruction<mov_a_c>,
  meta::instruction<mov_a_d>,
  meta::instruction<mov_a_e>,
  meta::instruction<mov_a_h>,
  meta::instruction<mov_a_l>,
  meta::instruction<mov_a_m>,
  meta::instruction<mov_a_a>,
  meta::instruction<add_b>,
  meta::instruction<add_c>,
  meta::instruction<add_d>,
  meta::instruction<add_e>,
  meta::instruction<add_h>,
  meta::instruction<add_l>,
  meta::instruction<add_m>,
  meta::instruction<add_a>,
  meta::instruction<adc_b>,
  meta::instruction<adc_c>,
  meta::instruction<adc_d>,
  meta::instruction<adc_e>,
  meta::instruction<adc_h>,
  meta::instruction<adc_l>,
  meta::instruction<adc_m>,
  meta::instruction<adc_a>,
  meta::instruction<sub_b>,
  meta::instruction<sub_c>,
  meta::instruction<sub_d>,
  meta::instruction<sub_e>,
  meta::instruction<sub_h>,
  meta::instruction<sub_l>,
  meta::instruction<sub_m>,
  meta::instruction<sub_a>,
  meta::instruction<sbb_b>,
  meta::instruction<sbb_c>,
  meta::instruction<sbb_d>,
  meta::instruction<sbb_e>,
  meta::instruction<sbb_h>,
  meta::instruction<sbb_l>,
  meta::instruction<sbb_m>,
  meta::instruction<sbb_a>,
  meta::instruction<ana_b>,
  meta::instruction<ana_c>,
  meta::instruction<ana_d>,
  meta::instruction<ana_e>,
  meta::instruction<ana_h>,
  meta::instruction<ana_l>,
  meta::instruction<ana_m>,
  meta::instruction<ana_a>,
  meta::instruction<xra_b>,
  meta::instruction<xra_c>,
  meta::instruction<xra_d>,
  meta::instruction<xra_e>,
  meta::instruction<xra_h>,
  meta::instruction<xra_l>,
  meta::instruction<xra_m>,
  meta::instruction<xra_a>,
  meta::instruction<ora_b>,
  meta::instruction<ora_c>,
  meta::instruction<ora_d>,
  meta::instruction<ora_e>,
  meta::instruction<ora_h>,
  meta::instruction<ora_l>,
  meta::instruction<ora_m>,
  meta::instruction<ora_a>,
  meta::instruction<cmp_b>,
  meta::instruction<cmp_c>,
  meta::instruction<cmp_d>,
  meta::instruction<cmp_e>,
  meta::instruction<cmp_h>,
  meta::instruction<cmp_l>,
  meta::instruction<cmp_m>,
  meta::instruction<cmp_a>,
  meta::instruction<rnz>,
  meta::instruction<pop_b>,
  meta::instruction<jnz_adr>,
  meta::instruction<jmp_adr>,
  meta::instruction<cnz_adr>,
  meta::instruction<push_b>,
  meta::instruction<adi>,
  meta::instruction<rst_0>,
  meta::instruction<rz>,
  meta::instruction<ret>,
  meta::instruction<jz_adr>,
  meta::instruction<meta::unimplemented<0xcb>>,
  meta::instruction<cz_adr>,
  meta::instruction<call_adr>,
  meta::instruction<aci_d8>,
  meta::instruction<rst_1>,
  meta::instruction<rnc>,
  meta::instruction<pop_d>,
  meta::instruction<jnc_adr>,
  meta::instruction<out>,
  meta::instruction<cnc_adr>,
  meta::instruction<push_d>,
  meta::instruction<sui_d8>,
  meta::instruction<rst_2>,
  meta::instruction<rc>,
  meta::instruction<meta::unimplemented<0xd9>>,
  meta::instruction<jc_adr>,
  meta::instruction<in>,
  meta::instruction<cc_adr>,
  meta::instruction<meta::unimplemented<0xdd>>,
  meta::instruction<sbi_d8>,
  meta::instruction<rst_3>,
  meta::instruction<rpo>,
  meta::instruction<pop_h>,
  meta::instruction<jpo_adr>,
  meta::instruction<xthl>,
  meta::instruction<cpo_adr>,
  meta::instruction<push_h>,
  meta::instruction<ani_d8>,
  meta::instruction<rst_4>,
  meta::instruction<rpe>,
  meta::instruction<pchl>,
  meta::instruction<jpe_adr>,
  meta::instruction<xchg>,
  meta::instruction<cpe_adr>,
  meta::instruction<meta::unimplemented<0xed>>,
  meta::instruction<xri_d8>,
  meta::instruction<rst_5>,
  meta::instruction<rp>,
  meta::instruction<pop_psw>,
  meta::instruction<jp_adr>,
  meta::instruction<di>,
  meta::instruction<cp_adr>,
  meta::instruction<push_psw>,
  meta::instruction<ori>,
  meta::instruction<rst_6>,
  meta::instruction<rm>,
  meta::instruction<sphl>,
  meta::instruction<jm_adr>,
  meta::instruction<ei>,
  meta::instruction<cm_adr>,
  meta::instruction<meta::unimplemented<0xfd>>,
  meta::instruction<cpi>,
  meta::instruction<rst_7>
>;

/*------------------------------------------------------------------------------------------------*/

} // namespace cpp8080::specific
