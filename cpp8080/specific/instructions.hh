#pragma once

#include <cstdint>
#include <utility>

#include "cpp8080/meta/make_instructions.hh"
#include "cpp8080/meta/unimplemented.hh"
#include "cpp8080/specific/halt.hh"
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
    const std::uint16_t offset = state.bc();
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
    const std::uint32_t res = state.hl() + state.bc();
    state.h = (res & 0xff00) >> 8;
    state.l = res & 0x00ff;
    state.cc.cy = ((res & 0xffff0000) != 0);
  }
};

struct ldax_b : meta::describe_instruction<0x0a, 7, 1>
{
  static constexpr auto name = "ldax_b";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.a = state.read_memory(state.bc());
  }
};

struct dcx_b : meta::describe_instruction<0x0b, 5, 1>
{
  static constexpr auto name = "dcx_b";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.c -= 1;
    if (state.c == 0x00ff)
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
    state.write_memory(state.de(), state.a);
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
    const std::uint32_t res = state.hl() + state.de();
    state.h = (res & 0xff00) >> 8;
    state.l = res & 0x00ff;
    state.cc.cy = ((res & 0xffff0000) != 0);
  }
};

struct ldax_d : meta::describe_instruction<0x1a, 7, 1>
{
  static constexpr auto name = "ldax_d";

  template <typename Machine> void operator()(state<Machine>& state) const
  {
    state.a = state.read_memory(state.de());
  }
};

struct dcx_d : meta::describe_instruction<0x1b, 5, 1>
{
  static constexpr auto name = "dcx_d";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.e -= 1;
    if (state.e == 0x00ff)
    {
      state.d -= 1;
    }
  }
};

struct inr_e : meta::describe_instruction<0x1c, 5, 1>
{
  static constexpr auto name = "inr_e";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.e += 1;
    state.flags_zsp(state.e);
  }
};

struct dcr_e : meta::describe_instruction<0x1d, 5, 1>
{
  static constexpr auto name = "dcr_e";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.e -= 1;
    state.flags_zsp(state.e);
  }
};

struct mvi_e : meta::describe_instruction<0x1e, 7, 2>
{
  static constexpr auto name = "mvi_e";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.e = state.op1();
    state.pc += 1;
  }
};

struct rar : meta::describe_instruction<0x1f, 4, 1>
{
  static constexpr auto name = "rar";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    const auto a = state.a;
    state.a = (state.cc.cy << 7) | (a >> 1);
    state.cc.cy = (1 == (a & 1));
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

  template <typename Machine> void operator()(state<Machine>& state) const
  {
    const std::uint16_t offset = state.op1() | (state.op2() << 8);
    state.write_memory(offset, state.l);
    state.write_memory(offset + 1, state.h);
    state.pc += 2;
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

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.h += 1;
    state.flags_zsp(state.h);
  }
};

struct dcr_h : meta::describe_instruction<0x25, 5, 1>
{
  static constexpr auto name = "dcr_h";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.h -= 1;
    state.flags_zsp(state.h);
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

  template <typename Machine> void operator()(state<Machine>& state) const
  {
    if ((state.a & 0x000f) > 9)
    {
      state.a += 6;
    }
    if ((state.a & 0x00f0) > 0x90)
    {
      const auto res = static_cast<std::uint16_t>(state.a) + 0x0060;
      state.a = res & 0x00ff;
      state.arithmetic_flags(res);
    }
  }
};

struct dad_h : meta::describe_instruction<0x29, 10, 1>
{
  static constexpr auto name = "dad_h";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    const std::uint32_t res = 2 * state.hl();
    state.h = (res & 0xff00) >> 8;
    state.l = res & 0x00ff;
    state.cc.cy = ((res & 0xffff0000) != 0);
  }
};

struct lhld_adr : meta::describe_instruction<0x2a, 16, 3>
{
 static constexpr auto name = "lhld_adr";

  template <typename Machine> void operator()(state<Machine>& state) const
  {
    const std::uint16_t offset = state.op1() | (state.op2() << 8);
    state.l = state.read_memory(offset);
    state.h = state.read_memory(offset + 1);
    state.pc += 2;
  }
};

struct dcx_h : meta::describe_instruction<0x2b, 5, 1>
{
  static constexpr auto name = "dcx_h";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.l -= 1;
    if (state.l == 0x00ff)
    {
      state.h -= 1;
    }
  }
};

struct inr_l : meta::describe_instruction<0x2c, 5, 1>
{
  static constexpr auto name = "inr_l";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.l += 1;
    state.flags_zsp(state.l);
  }
};

struct dcr_l : meta::describe_instruction<0x2d, 5, 1>
{
  static constexpr auto name = "dcr_l";

  template <typename Machine> void operator()(state<Machine>& state) const
  {
    state.l -= 1;
    state.flags_zsp(state.l);

  }
};

struct mvi_l_d8 : meta::describe_instruction<0x2e, 7, 2>
{
  static constexpr auto name = "mvi_l";

  template <typename Machine> void operator()(state<Machine>& state) const
  {
    state.l = state.op1();
    state.pc += 1;
  }
};

struct cma : meta::describe_instruction<0x2f, 4, 1>
{
  static constexpr auto name = "cma";

  template <typename Machine> void operator()(state<Machine>& state) const
  {
    state.a = ~state.a;
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
    const std::uint16_t offset = (state.op2() << 8) | state.op1();
    state.write_memory(offset, state.a);
    state.pc += 2;
  }
};

struct inx_sp : meta::describe_instruction<0x33, 5, 1>
{
  static constexpr auto name = "inx_sp";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.sp += 1;
  }
};

struct inr_m : meta::describe_instruction<0x34, 10, 1>
{
  static constexpr auto name = "inr_m";

  template <typename Machine> void operator()(state<Machine>& state) const
  {
    const auto res = state.read_hl() + 1;
    state.flags_zsp(res);
    state.write_hl(res);
  }
};

struct dcr_m : meta::describe_instruction<0x35, 10, 1>
{
  static constexpr auto name = "dcr_m";

  template <typename Machine> void operator()(state<Machine>& state) const
  {
    const auto res = state.read_hl() - 1;
    state.flags_zsp(res);
    state.write_hl(res);
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

  template <typename Machine> void operator()(state<Machine>& state) const
  {
    state.cc.cy = 1;
  }
};

struct dad_sp : meta::describe_instruction<0x39, 10, 1>
{
  static constexpr auto name = "dad_sp";

  template <typename Machine> void operator()(state<Machine>& state) const
  {
    const std::uint32_t res = state.hl() + state.sp;
    state.h = (res & 0xff00) >> 8;
    state.l = res & 0x00ff;
    state.cc.cy = ((res & 0xffff0000) > 0);
  }
};

struct lda_adr : meta::describe_instruction<0x3a, 13, 3>
{
  static constexpr auto name = "lda_adr";

  template <typename Machine> void operator()(state<Machine>& state) const
  {
    const std::uint16_t offset = (state.op2() << 8) | state.op1();
    state.a = state.read_memory(offset);
    state.pc += 2;
  }
};

struct dcx_sp : meta::describe_instruction<0x3b, 5, 1>
{
  static constexpr auto name = "dcx_sp";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.sp -= 1;
  }
};

struct inr_a : meta::describe_instruction<0x3c, 5, 1>
{
  static constexpr auto name = "inr_a";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.a += 1;
    state.flags_zsp(state.a);
  }
};

struct dcr_a : meta::describe_instruction<0x3d, 5, 1>
{
  static constexpr auto name = "dcr_a";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.a -= 1;
    state.flags_zsp(state.a);
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

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.cc.cy = 0;
  }
};

struct mov_b_b : meta::describe_instruction<0x40, 5, 1>
{
  static constexpr auto name = "mov_b_b";

  template <typename Machine> void operator()(state<Machine>&) const noexcept
  {}
};

struct mov_b_c : meta::describe_instruction<0x41, 5, 1>
{
  static constexpr auto name = "mov_b_c";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
   state.b = state.c;
  }
};

struct mov_b_d : meta::describe_instruction<0x42, 5, 1>
{
  static constexpr auto name = "mov_b_d";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.b = state.d;
  }
};

struct mov_b_e : meta::describe_instruction<0x43, 5, 1>
{
  static constexpr auto name = "mov_b_e";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.b = state.e;
  }
};

struct mov_b_h : meta::describe_instruction<0x44, 5, 1>
{
  static constexpr auto name = "mov_b_h";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.b = state.h;
  }
};

struct mov_b_l : meta::describe_instruction<0x45, 5, 1>
{
  static constexpr auto name = "mov_b_l";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.b = state.l;
  }
};

struct mov_b_m : meta::describe_instruction<0x46, 7, 1>
{
  static constexpr auto name = "mov_b_m";

  template <typename Machine> void operator()(state<Machine>& state) const
  {
    state.b = state.read_hl();
  }
};

struct mov_b_a : meta::describe_instruction<0x47, 5, 1>
{
  static constexpr auto name = "mov_b_a";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.b = state.a;
  }
};

struct mov_c_b : meta::describe_instruction<0x48, 5, 1>
{
  static constexpr auto name = "mov_c_b";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.c = state.b;
  }
};

struct mov_c_c : meta::describe_instruction<0x49, 5, 1>
{
  static constexpr auto name = "mov_c_c";

  template <typename Machine> void operator()(state<Machine>&) const noexcept
  {}
};

struct mov_c_d : meta::describe_instruction<0x4a, 5, 1>
{
  static constexpr auto name = "mov_c_d";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.c = state.d;
  }
};

struct mov_c_e : meta::describe_instruction<0x4b, 5, 1>
{
  static constexpr auto name = "mov_c_e";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.c = state.e;
  }
};

struct mov_c_h : meta::describe_instruction<0x4c, 5, 1>
{
  static constexpr auto name = "mov_c_h";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.c = state.h;
  }
};

struct mov_c_l : meta::describe_instruction<0x4d, 5, 1>
{
  static constexpr auto name = "mov_c_l";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.c = state.l;
  }
};

struct mov_c_m : meta::describe_instruction<0x4e, 7, 1>
{
  static constexpr auto name = "mov_c_m";

  template <typename Machine> void operator()(state<Machine>& state) const
  {
    state.c = state.read_hl();
  }
};

struct mov_c_a : meta::describe_instruction<0x4f, 5, 1>
{
  static constexpr auto name = "mov_c_a";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.c = state.a;
  }
};

struct mov_d_b : meta::describe_instruction<0x50, 5, 1>
{
  static constexpr auto name = "mov_d_b";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.d = state.b;
  }
};

struct mov_d_c : meta::describe_instruction<0x51, 5, 1>
{
  static constexpr auto name = "mov_d_c";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.d = state.c;
  }
};

struct mov_d_d : meta::describe_instruction<0x52, 5, 1>
{
  static constexpr auto name = "mov_d_d";

  template <typename Machine> void operator()(state<Machine>&) const noexcept
  {}
};

struct mov_d_e : meta::describe_instruction<0x53, 5, 1>
{
  static constexpr auto name = "mov_d_e";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.d = state.e;
  }
};

struct mov_d_h : meta::describe_instruction<0x54, 5, 1>
{
  static constexpr auto name = "mov_d_h";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.d = state.h;
  }
};

struct mov_d_l : meta::describe_instruction<0x55, 5, 1>
{
  static constexpr auto name = "mov_d_l";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.d = state.l;
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

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.d = state.a;
  }
};

struct mov_e_b : meta::describe_instruction<0x58, 5, 1>
{
  static constexpr auto name = "mov_e_b";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.e = state.b;
  }
};

struct mov_e_c : meta::describe_instruction<0x59, 5, 1>
{
  static constexpr auto name = "mov_e_c";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.e = state.c;
  }
};

struct mov_e_d : meta::describe_instruction<0x5a, 5, 1>
{
  static constexpr auto name = "mov_e_d";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.e = state.d;
  }
};

struct mov_e_e : meta::describe_instruction<0x5b, 5, 1>
{
  static constexpr auto name = "mov_e_e";

  template <typename Machine> void operator()(state<Machine>&) const noexcept
  {}
};

struct mov_e_h : meta::describe_instruction<0x5c, 5, 1>
{
  static constexpr auto name = "mov_e_h";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.e = state.h;
  }
};

struct mov_e_l : meta::describe_instruction<0x5d, 5, 1>
{
  static constexpr auto name = "mov_e_l";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.e = state.l;
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

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.e = state.a;
  }
};

struct mov_h_b : meta::describe_instruction<0x60, 5, 1>
{
  static constexpr auto name = "mov_h_b";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.h = state.b;
  }
};

struct mov_h_c : meta::describe_instruction<0x61, 5, 1>
{
  static constexpr auto name = "mov_h_c";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.h = state.c;
  }
};

struct mov_h_d : meta::describe_instruction<0x62, 5, 1>
{
  static constexpr auto name = "mov_h_d";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.h = state.d;
  }
};

struct mov_h_e : meta::describe_instruction<0x63, 5, 1>
{
  static constexpr auto name = "mov_h_e";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.h = state.e;
  }
};

struct mov_h_h : meta::describe_instruction<0x64, 5, 1>
{
  static constexpr auto name = "mov_h_h";

  template <typename Machine> void operator()(state<Machine>&) const noexcept
  {}
};

struct mov_h_l : meta::describe_instruction<0x65, 5, 1>
{
  static constexpr auto name = "mov_h_l";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.h = state.l;
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

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.h = state.a;
  }
};

struct mov_l_b : meta::describe_instruction<0x68, 5, 1>
{
  static constexpr auto name = "mov_l_b";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.l = state.b;
  }
};

struct mov_l_c : meta::describe_instruction<0x69, 5, 1>
{
  static constexpr auto name = "mov_l_c";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.l = state.c;
  }
};

struct mov_l_d : meta::describe_instruction<0x6a, 5, 1>
{
  static constexpr auto name = "mov_l_d";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.l = state.d;
  }
};

struct mov_l_e : meta::describe_instruction<0x6b, 5, 1>
{
  static constexpr auto name = "mov_l_e";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.l = state.e;
  }
};

struct mov_l_h : meta::describe_instruction<0x6c, 5, 1>
{
  static constexpr auto name = "mov_l_h";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.l = state.h;
  }
};

struct mov_l_l : meta::describe_instruction<0x6d, 5, 1>
{
  static constexpr auto name = "mov_l_l";

  template <typename Machine> void operator()(state<Machine>&) const noexcept
  {}
};

struct mov_l_m : meta::describe_instruction<0x6e, 7, 1>
{
  static constexpr auto name = "mov_l_m";

  template <typename Machine> void operator()(state<Machine>& state) const
  {
    state.l = state.read_hl();
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

  template <typename Machine> void operator()(state<Machine>& state) const
  {
    state.write_hl(state.b);
  }
};

struct mov_m_c : meta::describe_instruction<0x71, 7, 1>
{
  static constexpr auto name = "mov_m_c";

  template <typename Machine> void operator()(state<Machine>& state) const
  {
    state.write_hl(state.c);
  }
};

struct mov_m_d : meta::describe_instruction<0x72, 7, 1>
{
  static constexpr auto name = "mov_m_d";

  template <typename Machine> void operator()(state<Machine>& state) const
  {
    state.write_hl(state.d);
  }
};

struct mov_m_e : meta::describe_instruction<0x73, 7, 1>
{
  static constexpr auto name = "mov_m_e";

  template <typename Machine> void operator()(state<Machine>& state) const
  {
    state.write_hl(state.e);
  }
};

struct mov_m_h : meta::describe_instruction<0x74, 7, 1>
{
  static constexpr auto name = "mov_m_h";

  template <typename Machine> void operator()(state<Machine>& state) const
  {
    state.write_hl(state.h);
  }
};

struct mov_m_l : meta::describe_instruction<0x75, 7, 1>
{
  static constexpr auto name = "mov_m_l";

  template <typename Machine> void operator()(state<Machine>& state) const
  {
    state.write_hl(state.l);
  }
};

struct hlt : meta::describe_instruction<0x76, 7, 1>
{
  static constexpr auto name = "hlt";

  template <typename Machine> void operator()(state<Machine>&) const
  {
    throw halt{"HLT"};
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

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.a = state.b;
  }
};

struct mov_a_c : meta::describe_instruction<0x79, 5, 1>
{
  static constexpr auto name = "mov_a_c";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.a = state.c;
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

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.a = state.l;
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

  template <typename Machine> void operator()(state<Machine>&) const noexcept
  {}
};

struct add_b : meta::describe_instruction<0x80, 4, 1>
{
  static constexpr auto name = "add_b";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    const auto res = static_cast<std::uint16_t>(state.a) + static_cast<std::uint16_t>(state.b);
    state.arithmetic_flags(res);
    state.a = res & 0x00ff;
  }
};

struct add_c : meta::describe_instruction<0x81, 4, 1>
{
  static constexpr auto name = "add_c";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    const auto res = static_cast<std::uint16_t>(state.a) + static_cast<std::uint16_t>(state.c);
    state.arithmetic_flags(res);
    state.a = res & 0x00ff;
  }
};

struct add_d : meta::describe_instruction<0x82, 4, 1>
{
  static constexpr auto name = "add_d";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    const auto res = static_cast<std::uint16_t>(state.a) + static_cast<std::uint16_t>(state.d);
    state.arithmetic_flags(res);
    state.a = res & 0x00ff;
  }
};

struct add_e : meta::describe_instruction<0x83, 4, 1>
{
  static constexpr auto name = "add_e";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    const auto res = static_cast<std::uint16_t>(state.a) + static_cast<std::uint16_t>(state.e);
    state.arithmetic_flags(res);
    state.a = res & 0x00ff;
  }
};

struct add_h : meta::describe_instruction<0x84, 4, 1>
{
  static constexpr auto name = "add_h";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    const auto res = static_cast<std::uint16_t>(state.a) + static_cast<std::uint16_t>(state.h);
    state.arithmetic_flags(res);
    state.a = res & 0x00ff;
  }
};

struct add_l : meta::describe_instruction<0x85, 4, 1>
{
  static constexpr auto name = "add_l";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    const auto res = static_cast<std::uint16_t>(state.a) + static_cast<std::uint16_t>(state.l);
    state.arithmetic_flags(res);
    state.a = res & 0x00ff;
  }
};

struct add_m : meta::describe_instruction<0x86, 7, 1>
{
  static constexpr auto name = "add_m";

  template <typename Machine> void operator()(state<Machine>& state) const
  {
    const auto res = static_cast<std::uint16_t>(state.a)
                   + static_cast<std::uint16_t>(state.read_hl());
    state.arithmetic_flags(res);
    state.a = res & 0x00ff;
  }
};

struct add_a : meta::describe_instruction<0x87, 4, 1>
{
  static constexpr auto name = "add_a";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    const auto res = static_cast<std::uint16_t>(state.a) + static_cast<std::uint16_t>(state.a);
    state.arithmetic_flags(res);
    state.a = res & 0x00ff;
  }
};

struct adc_b : meta::describe_instruction<0x88, 4, 1>
{
  static constexpr auto name = "adc_b";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    const auto res = static_cast<std::uint16_t>(state.a) + static_cast<std::uint16_t>(state.b)
    + state.cc.cy;
    state.arithmetic_flags(res);
    state.a = res & 0x00ff;
  }
};

struct adc_c : meta::describe_instruction<0x89, 4, 1>
{
  static constexpr auto name = "adc_c";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    const auto res = static_cast<std::uint16_t>(state.a) + static_cast<std::uint16_t>(state.c)
                   + state.cc.cy;
    state.arithmetic_flags(res);
    state.a = res & 0x00ff;
  }
};

struct adc_d : meta::describe_instruction<0x8a, 4, 1>
{
  static constexpr auto name = "adc_d";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    const auto res = static_cast<std::uint16_t>(state.a) + static_cast<std::uint16_t>(state.d)
                   + state.cc.cy;
    state.arithmetic_flags(res);
    state.a = res & 0x00ff;
  }
};

struct adc_e : meta::describe_instruction<0x8b, 4, 1>
{
  static constexpr auto name = "adc_e";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    const auto res = static_cast<std::uint16_t>(state.a) + static_cast<std::uint16_t>(state.e)
                   + state.cc.cy;
    state.arithmetic_flags(res);
    state.a = res & 0x00ff;
  }
};

struct adc_h : meta::describe_instruction<0x8c, 4, 1>
{
  static constexpr auto name = "adc_h";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    const auto res = static_cast<std::uint16_t>(state.a) + static_cast<std::uint16_t>(state.h)
                   + state.cc.cy;
    state.arithmetic_flags(res);
    state.a = res & 0x00ff;
  }
};

struct adc_l : meta::describe_instruction<0x8d, 4, 1>
{
  static constexpr auto name = "adc_l";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    const auto res = static_cast<std::uint16_t>(state.a) + static_cast<std::uint16_t>(state.l)
                   + state.cc.cy;
    state.arithmetic_flags(res);
    state.a = res & 0x00ff;
  }
};

struct adc_m : meta::describe_instruction<0x8e, 7, 1>
{
  static constexpr auto name = "adc_m";

  template <typename Machine> void operator()(state<Machine>& state) const
  {
    const auto res = static_cast<std::uint16_t>(state.a)
                   + static_cast<std::uint16_t>(state.read_hl())
                   + state.cc.cy;
    state.arithmetic_flags(res);
    state.a = res & 0x00ff;
  }
};

struct adc_a : meta::describe_instruction<0x8f, 4, 1>
{
  static constexpr auto name = "adc_a";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    const auto res = static_cast<std::uint16_t>(state.a) + static_cast<std::uint16_t>(state.a)
                   + state.cc.cy;
    state.arithmetic_flags(res);
    state.a = res & 0x00ff;
  }
};

struct sub_b : meta::describe_instruction<0x90, 4, 1>
{
  static constexpr auto name = "sub_b";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    const auto res = static_cast<std::uint16_t>(state.a) - static_cast<std::uint16_t>(state.b);
    state.arithmetic_flags(res);
    state.a = res & 0x00ff;
  }
};

struct sub_c : meta::describe_instruction<0x91, 4, 1>
{
  static constexpr auto name = "sub_c";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    const auto res = static_cast<std::uint16_t>(state.a) - static_cast<std::uint16_t>(state.c);
    state.arithmetic_flags(res);
    state.a = res & 0x00ff;
  }
};

struct sub_d : meta::describe_instruction<0x92, 4, 1>
{
  static constexpr auto name = "sub_d";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    const auto res = static_cast<std::uint16_t>(state.a) - static_cast<std::uint16_t>(state.d);
    state.arithmetic_flags(res);
    state.a = res & 0x00ff;
  }
};

struct sub_e : meta::describe_instruction<0x93, 4, 1>
{
  static constexpr auto name = "sub_e";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    const auto res = static_cast<std::uint16_t>(state.a) - static_cast<std::uint16_t>(state.e);
    state.arithmetic_flags(res);
    state.a = res & 0x00ff;
  }
};

struct sub_h : meta::describe_instruction<0x94, 4, 1>
{
  static constexpr auto name = "sub_h";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    const auto res = static_cast<std::uint16_t>(state.a) - static_cast<std::uint16_t>(state.h);
    state.arithmetic_flags(res);
    state.a = res & 0x00ff;
  }
};

struct sub_l : meta::describe_instruction<0x95, 4, 1>
{
  static constexpr auto name = "sub_l";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    const auto res = static_cast<std::uint16_t>(state.a) - static_cast<std::uint16_t>(state.l);
    state.arithmetic_flags(res);
    state.a = res & 0x00ff;
  }
};

struct sub_m : meta::describe_instruction<0x96, 7, 1>
{
  static constexpr auto name = "sub_m";

  template <typename Machine> void operator()(state<Machine>& state) const
  {
    const auto res = static_cast<std::uint16_t>(state.a)
                   - static_cast<std::uint16_t>(state.read_hl());
    state.arithmetic_flags(res);
    state.a = res & 0x00ff;
  }
};

struct sub_a : meta::describe_instruction<0x97, 4, 1>
{
  static constexpr auto name = "sub_a";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    const auto res = static_cast<std::uint16_t>(state.a) - static_cast<std::uint16_t>(state.a);
    state.arithmetic_flags(res);
    state.a = res & 0x00ff;
  }
};

struct sbb_b : meta::describe_instruction<0x98, 4, 1>
{
  static constexpr auto name = "sbb_b";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    const auto res = static_cast<std::uint16_t>(state.a) - static_cast<std::uint16_t>(state.b)
                   - state.cc.cy;
    state.arithmetic_flags(res);
    state.a = res & 0x00ff;
  }
};

struct sbb_c : meta::describe_instruction<0x99, 4, 1>
{
  static constexpr auto name = "sbb_c";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    const auto res = static_cast<std::uint16_t>(state.a) - static_cast<std::uint16_t>(state.c)
                   - state.cc.cy;
    state.arithmetic_flags(res);
    state.a = res & 0x00ff;
  }
};

struct sbb_d : meta::describe_instruction<0x9a, 4, 1>
{
  static constexpr auto name = "sbb_d";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    const auto res = static_cast<std::uint16_t>(state.a) - static_cast<std::uint16_t>(state.d)
                   - state.cc.cy;
    state.arithmetic_flags(res);
    state.a = res & 0x00ff;
  }
};

struct sbb_e : meta::describe_instruction<0x9b, 4, 1>
{
  static constexpr auto name = "sbb_e";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    const auto res = static_cast<std::uint16_t>(state.a) - static_cast<std::uint16_t>(state.e)
                   - state.cc.cy;
    state.arithmetic_flags(res);
    state.a = res & 0x00ff;
  }
};

struct sbb_h : meta::describe_instruction<0x9c, 4, 1>
{
  static constexpr auto name = "sbb_h";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    const auto res = static_cast<std::uint16_t>(state.a) - static_cast<std::uint16_t>(state.h)
                   - state.cc.cy;
    state.arithmetic_flags(res);
    state.a = res & 0x00ff;
  }
};

struct sbb_l : meta::describe_instruction<0x9d, 4, 1>
{
  static constexpr auto name = "sbb_l";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    const auto res = static_cast<std::uint16_t>(state.a) - static_cast<std::uint16_t>(state.l)
                   - state.cc.cy;
    state.arithmetic_flags(res);
    state.a = res & 0x00ff;
  }
};

struct sbb_m : meta::describe_instruction<0x9e, 7, 1>
{
  static constexpr auto name = "sbb_m";

  template <typename Machine> void operator()(state<Machine>& state) const
  {
    const auto res = static_cast<std::uint16_t>(state.a)
                   - static_cast<std::uint16_t>(state.read_hl())
                   - state.cc.cy;
    state.arithmetic_flags(res);
    state.a = res & 0x00ff;
  }
};

struct sbb_a : meta::describe_instruction<0x9f, 4, 1>
{
  static constexpr auto name = "sbb_a";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    const auto res = static_cast<std::uint16_t>(state.a) - static_cast<std::uint16_t>(state.a)
                   - state.cc.cy;
    state.arithmetic_flags(res);
    state.a = res & 0x00ff;
  }
};

struct ana_b : meta::describe_instruction<0xa0, 4, 1>
{
  static constexpr auto name = "ana_b";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.a = state.a & state.b;
    state.logic_flags_a();
  }
};

struct ana_c : meta::describe_instruction<0xa1, 4, 1>
{
  static constexpr auto name = "ana_c";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.a = state.a & state.c;
    state.logic_flags_a();
  }
};

struct ana_d : meta::describe_instruction<0xa2, 4, 1>
{
  static constexpr auto name = "ana_d";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.a = state.a & state.d;
    state.logic_flags_a();
  }
};

struct ana_e : meta::describe_instruction<0xa3, 4, 1>
{
  static constexpr auto name = "ana_e";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.a = state.a & state.e;
    state.logic_flags_a();
  }
};

struct ana_h : meta::describe_instruction<0xa4, 4, 1>
{
  static constexpr auto name = "ana_h";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.a = state.a & state.h;
    state.logic_flags_a();
  }
};

struct ana_l : meta::describe_instruction<0xa5, 4, 1>
{
  static constexpr auto name = "ana_l";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.a = state.a & state.l;
    state.logic_flags_a();
  }
};

struct ana_m : meta::describe_instruction<0xa6, 7, 1>
{
  static constexpr auto name = "ana_m";

  template <typename Machine> void operator()(state<Machine>& state) const
  {
    state.a = state.a & state.read_hl();
    state.logic_flags_a();
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

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.a = state.a ^ state.b;
    state.logic_flags_a();
  }
};

struct xra_c : meta::describe_instruction<0xa9, 4, 1>
{
  static constexpr auto name = "xra_c";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.a = state.a ^ state.c;
    state.logic_flags_a();
  }
};

struct xra_d : meta::describe_instruction<0xaa, 4, 1>
{
  static constexpr auto name = "xra_d";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.a = state.a ^ state.d;
    state.logic_flags_a();
  }
};

struct xra_e : meta::describe_instruction<0xab, 4, 1>
{
  static constexpr auto name = "xra_e";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.a = state.a ^ state.e;
    state.logic_flags_a();
  }
};

struct xra_h : meta::describe_instruction<0xac, 4, 1>
{
  static constexpr auto name = "xra_h";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.a = state.a ^ state.h;
    state.logic_flags_a();
  }
};

struct xra_l : meta::describe_instruction<0xad, 4, 1>
{
  static constexpr auto name = "xra_l";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.a = state.a ^ state.l;
    state.logic_flags_a();
  }
};

struct xra_m : meta::describe_instruction<0xae, 7, 1>
{
  static constexpr auto name = "xra_m";

  template <typename Machine> void operator()(state<Machine>& state) const
  {
    state.a = state.a ^ state.read_hl();
    state.logic_flags_a();
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

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.a = state.a | state.b;
    state.logic_flags_a();
  }
};

struct ora_c : meta::describe_instruction<0xb1, 4, 1>
{
  static constexpr auto name = "ora_c";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.a = state.a | state.c;
    state.logic_flags_a();
  }
};

struct ora_d : meta::describe_instruction<0xb2, 4, 1>
{
  static constexpr auto name = "ora_d";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.a = state.a | state.d;
    state.logic_flags_a();
  }
};

struct ora_e : meta::describe_instruction<0xb3, 4, 1>
{
  static constexpr auto name = "ora_e";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.a = state.a | state.e;
    state.logic_flags_a();
  }
};

struct ora_h : meta::describe_instruction<0xb4, 4, 1>
{
  static constexpr auto name = "ora_h";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.a = state.a | state.h;
    state.logic_flags_a();
  }
};

struct ora_l : meta::describe_instruction<0xb5, 4, 1>
{
  static constexpr auto name = "ora_l";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.a = state.a | state.l;
    state.logic_flags_a();
  }
};

struct ora_m : meta::describe_instruction<0xb6, 7, 1>
{
  static constexpr auto name = "ora_m";

  template <typename Machine> void operator()(state<Machine>& state) const
  {
    state.a = state.a | state.read_hl();
    state.logic_flags_a();
  }
};

struct ora_a : meta::describe_instruction<0xb7, 4, 1>
{
  static constexpr auto name = "ora_a";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.a = state.a | state.a;
    state.logic_flags_a();
  }
};

struct cmp_b : meta::describe_instruction<0xb8, 4, 1>
{
  static constexpr auto name = "cmp_b";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    const auto res = static_cast<std::uint16_t>(state.a) - static_cast<std::uint16_t>(state.b);
    state.arithmetic_flags(res);
  }
};

struct cmp_c : meta::describe_instruction<0xb9, 4, 1>
{
  static constexpr auto name = "cmp_c";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    const auto res = static_cast<std::uint16_t>(state.a) - static_cast<std::uint16_t>(state.c);
    state.arithmetic_flags(res);
  }
};

struct cmp_d : meta::describe_instruction<0xba, 4, 1>
{
  static constexpr auto name = "cmp_d";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    const auto res = static_cast<std::uint16_t>(state.a) - static_cast<std::uint16_t>(state.d);
    state.arithmetic_flags(res);
  }
};

struct cmp_e : meta::describe_instruction<0xbb, 4, 1>
{
  static constexpr auto name = "cmp_e";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    const auto res = static_cast<std::uint16_t>(state.a) - static_cast<std::uint16_t>(state.e);
    state.arithmetic_flags(res);
  }
};

struct cmp_h : meta::describe_instruction<0xbc, 4, 1>
{
  static constexpr auto name = "cmp_h";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    const auto res = static_cast<std::uint16_t>(state.a) - static_cast<std::uint16_t>(state.h);
    state.arithmetic_flags(res);
  }
};

struct cmp_l : meta::describe_instruction<0xbd, 4, 1>
{
  static constexpr auto name = "cmp_l";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    const auto res = static_cast<std::uint16_t>(state.a) - static_cast<std::uint16_t>(state.l);
    state.arithmetic_flags(res);
  }
};

struct cmp_m : meta::describe_instruction<0xbe, 7, 1>
{
  static constexpr auto name = "cmp_m";

  template <typename Machine> void operator()(state<Machine>& state) const
  {
    const auto res = static_cast<std::uint16_t>(state.a)
                   - static_cast<std::uint16_t>(state.read_hl());
    state.arithmetic_flags(res);
  }
};

struct cmp_a : meta::describe_instruction<0xbf, 4, 1>
{
  static constexpr auto name = "cmp_a";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    const auto res = static_cast<std::uint16_t>(state.a) - static_cast<std::uint16_t>(state.a);
    state.arithmetic_flags(res);
  }
};

struct rnz : meta::describe_instruction<0xc0, 11, 1>
{
  static constexpr auto name = "rnz";

  template <typename Machine> void operator()(state<Machine>& state) const
  {
    if (state.cc.z == 0)
    {
      state.pc = state.read_memory(state.sp) | (state.read_memory(state.sp + 1) << 8);
      state.sp += 2;
    }
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

struct jnz : meta::describe_instruction<0xc2, 10, 3>
{
  static constexpr auto name = "jnz";

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

struct jmp : meta::describe_instruction<0xc3, 10, 3>
{
  static constexpr auto name = "jmp";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.pc = (state.op2() << 8) | state.op1();
  }
};

struct cnz : meta::describe_instruction<0xc4, 17, 3>
{
  static constexpr auto name = "cnz";

  template <typename Machine> void operator()(state<Machine>& state) const
  {
    if (state.cc.z == 0)
    {
      const auto ret = state.pc + 2;
      state.write_memory(state.sp - 1, (ret >> 8) & 0x00ff);
      state.write_memory(state.sp - 2, ret & 0x00ff);
      state.sp -= 2;
      state.pc = (state.op2() << 8) | state.op1();
    }
    else
    {
      state.pc += 2;
    }
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
    state.flags_zsp(x & 0x00ff);
    state.cc.cy = (x > 0x00ff);
    state.a = x & 0x00ff;
    state.pc += 1;
  }
};

struct rst_0 : meta::describe_instruction<0xc7, 11, 1>
{
  static constexpr auto name = "rst_0";

  template <typename Machine> void operator()(state<Machine>& state) const
  {
    const auto ret = state.pc + 2;
    state.write_memory(state.sp - 1, (ret >> 8) & 0x00ff);
    state.write_memory(state.sp - 2, ret & 0x00ff);
    state.sp -= 2;
    state.pc = 0x0000;
  }
};

struct rz : meta::describe_instruction<0xc8, 11, 1>
{
  static constexpr auto name = "rz";

  template <typename Machine> void operator()(state<Machine>& state) const
  {
    if (state.cc.z == 1)
    {
      state.pc = state.read_memory(state.sp) | (state.read_memory(state.sp + 1) << 8);
      state.sp += 2;
    }
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

  template <typename Machine> void operator()(state<Machine>& state) const
  {
    if (state.cc.z)
    {
      state.pc = (state.op2() << 8) | state.op1();
    }
    else
    {
      state.pc += 2;
    }
  }
};

struct cz_adr : meta::describe_instruction<0xcc, 10, 3>
{
  static constexpr auto name = "cz_adr";

  template <typename Machine> void operator()(state<Machine>& state) const
  {
    if (state.cc.z == 1)
    {
      const auto ret = state.pc + 2;
      state.write_memory(state.sp - 1, (ret >> 8) & 0x00ff);
      state.write_memory(state.sp - 2, ret & 0x00ff);
      state.sp -= 2;
      state.pc = (state.op2() << 8) | state.op1();
    }
    else
    {
      state.pc += 2;
    }
  }
};

struct call_adr : meta::describe_instruction<0xcd, 17, 3>
{
  static constexpr auto name = "call_adr";

  template <typename Machine> void operator()(state<Machine>& state) const
  {
    const auto ret = state.pc + 2;
    state.write_memory(state.sp - 1, (ret >> 8) & 0x00ff);
    state.write_memory(state.sp - 2, ret & 0x00ff);
    state.sp -= 2;
    state.pc = (state.op2() << 8) | state.op1();
  }
};

struct aci : meta::describe_instruction<0xce, 7, 2>
{
  static constexpr auto name = "aci";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    const std::uint16_t x = state.a + state.op1() + state.cc.cy;
    state.flags_zsp(x & 0x00ff);
    state.cc.cy = (x > 0x00ff);
    state.a = x & 0x00ff;
    state.pc += 1;
  }
};

struct rst_1 : meta::describe_instruction<0xcf, 11, 1>
{
  static constexpr auto name = "rst_1";

  template <typename Machine> void operator()(state<Machine>& state) const
  {
    const auto ret = state.pc + 2;
    state.write_memory(state.sp - 1, (ret >> 8) & 0x00ff);
    state.write_memory(state.sp - 2, ret & 0x00ff);
    state.sp -= 2;
    state.pc = 0x0008;
  }
};

struct rnc : meta::describe_instruction<0xd0, 11, 1>
{
  static constexpr auto name = "rnc";

  template <typename Machine> void operator()(state<Machine>& state) const
  {
    if (state.cc.cy == 0)
    {
      state.pc = state.read_memory(state.sp) | (state.read_memory(state.sp + 1) << 8);
      state.sp += 2;
    }
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

  template <typename Machine> void operator()(state<Machine>& state) const
  {
    if (state.cc.cy == 0)
    {
      state.pc = (state.op2() << 8) | state.op1();
    }
    else
    {
      state.pc += 2;
    }
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

  template <typename Machine> void operator()(state<Machine>& state) const
  {
    if (state.cc.cy == 0)
    {
      const auto ret = state.pc + 2;
      state.write_memory(state.sp - 1, (ret >> 8) & 0x00ff);
      state.write_memory(state.sp - 2, ret & 0x00ff);
      state.sp -= 2;
      state.pc = (state.op2() << 8) | state.op1();
    }
    else
    {
      state.pc += 2;
    }
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

struct sui : meta::describe_instruction<0xd6, 7, 2>
{
  static constexpr auto name = "sui";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    const auto x = state.a - state.op1();
    state.flags_zsp(x & 0x00ff);
    state.cc.cy = (state.a < state.op1());
    state.a = x;
    state.pc += 1;
  }
};

struct rst_2 : meta::describe_instruction<0xd7, 11, 1>
{
  static constexpr auto name = "rst_2";

  template <typename Machine> void operator()(state<Machine>& state) const
  {
    const auto ret = state.pc + 2;
    state.write_memory(state.sp - 1, (ret >> 8) & 0x00ff);
    state.write_memory(state.sp - 2, ret & 0x00ff);
    state.sp -= 2;
    state.pc = 0x0010;
  }
};

struct rc : meta::describe_instruction<0xd8, 11, 1>
{
  static constexpr auto name = "rc";

  template <typename Machine> void operator()(state<Machine>& state) const
  {
    if (state.cc.cy != 0)
    {
      state.pc = state.read_memory(state.sp) | (state.read_memory(state.sp + 1) << 8);
      state.sp += 2;
    }
  }
};

struct jc_adr : meta::describe_instruction<0xda, 10, 3>
{
  static constexpr auto name = "jc_adr";

  template <typename Machine> void operator()(state<Machine>& state) const
  {
    if (state.cc.cy != 0)
    {
      state.pc = (state.op2() << 8) | state.op1();
    }
    else
    {
      state.pc += 2;
    }
  }
};

struct in : meta::describe_instruction<0xdb, 10, 2>
{
  static constexpr auto name = "in";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.pc += 1;
  }
};

struct cc_adr : meta::describe_instruction<0xdc, 10, 3>
{
  static constexpr auto name = "cc_adr";

  template <typename Machine> void operator()(state<Machine>& state) const
  {
    if (state.cc.cy != 0)
    {
      const auto ret = state.pc + 2;
      state.write_memory(state.sp - 1, (ret >> 8) & 0x00ff);
      state.write_memory(state.sp - 2, ret & 0x00ff);
      state.sp -= 2;
      state.pc = (state.op2() << 8) | state.op1();
    }
    else
    {
      state.pc += 2;
    }
  }
};

struct sbi : meta::describe_instruction<0xde, 7, 2>
{
  static constexpr auto name = "sbi";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    const std::uint16_t x = state.a - state.op1() - state.cc.cy;
    state.flags_zsp(x & 0x00ff);
    state.cc.cy = (x > 0x00ff);
    state.a = x & 0x00ff;
    state.pc += 1;
  }
};

struct rst_3 : meta::describe_instruction<0xdf, 11, 1>
{
  static constexpr auto name = "rst_3";

  template <typename Machine> void operator()(state<Machine>& state) const
  {
    const auto ret = state.pc + 2;
    state.write_memory(state.sp - 1, (ret >> 8) & 0x00ff);
    state.write_memory(state.sp - 2, ret & 0x00ff);
    state.sp -= 2;
    state.pc = 0x0018;
  }
};

struct rpo : meta::describe_instruction<0xe0, 11, 1>
{
  static constexpr auto name = "rpo";

  template <typename Machine> void operator()(state<Machine>& state) const
  {
    if (state.cc.p == 0)
    {
      state.pc = state.read_memory(state.sp) | (state.read_memory(state.sp + 1) << 8);
      state.sp += 2;
    }
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

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    if (state.cc.p == 0)
    {
      state.pc = (state.op2() << 8) | state.op1();
    }
    else
    {
      state.pc += 2;
    }
  }
};

struct xthl : meta::describe_instruction<0xe3, 18, 1>
{
  static constexpr auto name = "xthl";

  template <typename Machine> void operator()(state<Machine>& state) const
  {
    const auto h = state.h;
    const auto l = state.l;
    state.l = state.read_memory(state.sp);
    state.h = state.read_memory(state.sp + 1);
    state.write_memory(state.sp, l);
    state.write_memory(state.sp + 1, h);
  }
};

struct cpo_adr : meta::describe_instruction<0xe4, 17, 3>
{
  static constexpr auto name = "cpo_adr";

  template <typename Machine> void operator()(state<Machine>& state) const
  {
    if (state.cc.p == 0)
    {
      const auto ret = state.pc + 2;
      state.write_memory(state.sp - 1, (ret >> 8) & 0x00ff);
      state.write_memory(state.sp - 2, ret & 0x00ff);
      state.sp -= 2;
      state.pc = (state.op2() << 8) | state.op1();
    }
    else
    {
      state.pc += 2;
    }
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

  template <typename Machine> void operator()(state<Machine>& state) const
  {
    const auto ret = state.pc + 2;
    state.write_memory(state.sp - 1, (ret >> 8) & 0x00ff);
    state.write_memory(state.sp - 2, ret & 0x00ff);
    state.sp -= 2;
    state.pc = 0x0020;
  }
};

struct rpe : meta::describe_instruction<0xe8, 11, 1>
{
  static constexpr auto name = "rpe";

  template <typename Machine> void operator()(state<Machine>& state) const
  {
    if (state.cc.p != 0)
    {
      state.pc = state.read_memory(state.sp) | (state.read_memory(state.sp + 1) << 8);
      state.sp += 2;
    }
  }
};

struct pchl : meta::describe_instruction<0xe9, 5, 1>
{
  static constexpr auto name = "pchl";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.pc = state.hl();
  }
};

struct jpe : meta::describe_instruction<0xea, 10, 3>
{
  static constexpr auto name = "jpe";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    if (state.cc.p != 0)
    {
      state.pc = (state.op2() << 8) | state.op1();
    }
    else
    {
      state.pc += 2;
    }
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

  template <typename Machine> void operator()(state<Machine>& state) const
  {
    if (state.cc.p != 0)
    {
      const auto ret = state.pc + 2;
      state.write_memory(state.sp - 1, (ret >> 8) & 0x00ff);
      state.write_memory(state.sp - 2, ret & 0x00ff);
      state.sp -= 2;
      state.pc = (state.op2() << 8) | state.op1();
    }
    else
    {
      state.pc += 2;
    }

  }
};

struct xri : meta::describe_instruction<0xee, 7, 2>
{
  static constexpr auto name = "xri";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    const auto x = state.a ^ state.op1();
    state.flags_zsp(x);
    state.cc.cy = 0;
    state.a = x;
    state.pc += 1;
  }
};

struct rst_5 : meta::describe_instruction<0xef, 11, 1>
{
  static constexpr auto name = "rst_5";

  template <typename Machine> void operator()(state<Machine>& state) const
  {
    const auto ret = state.pc + 2;
    state.write_memory(state.sp - 1, (ret >> 8) & 0x00ff);
    state.write_memory(state.sp - 2, ret & 0x00ff);
    state.sp -= 2;
    state.pc = 0x0028;
  }
};

struct rp : meta::describe_instruction<0xf0, 11, 1>
{
  static constexpr auto name = "rp";

  template <typename Machine> void operator()(state<Machine>& state) const
  {
    if (state.cc.s == 0)
    {
      state.pc = state.read_memory(state.sp) | (state.read_memory(state.sp + 1) << 8);
      state.sp += 2;
    }
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

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    if (state.cc.s == 0)
    {
      state.pc = (state.op2() << 8) | state.op1();
    }
    else
    {
      state.pc += 2;
    }
  }
};

struct di : meta::describe_instruction<0xf3, 4, 1>
{
  static constexpr auto name = "di";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.disable_interrupt();
  }
};

struct cp_adr : meta::describe_instruction<0xf4, 17, 3>
{
  static constexpr auto name = "cp_adr";

  template <typename Machine> void operator()(state<Machine>& state) const
  {
    if (state.cc.s == 0)
    {
      const auto ret = state.pc + 2;
      state.write_memory(state.sp - 1, (ret >> 8) & 0x00ff);
      state.write_memory(state.sp - 2, ret & 0x00ff);
      state.sp -= 2;
      state.pc = (state.op2() << 8) | state.op1();
    }
    else
    {
      state.pc += 2;
    }

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

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    const auto x = state.a | state.op1();
    state.flags_zsp(x);
    state.cc.cy = 0;
    state.a = x;
    state.pc += 1;
  }
};

struct rst_6 : meta::describe_instruction<0xf7, 11, 1>
{
  static constexpr auto name = "rst_6";

  template <typename Machine> void operator()(state<Machine>& state) const
  {
    const auto ret = state.pc + 2;
    state.write_memory(state.sp - 1, (ret >> 8) & 0x00ff);
    state.write_memory(state.sp - 2, ret & 0x00ff);
    state.sp -= 2;
    state.pc = 0x0030;
  }

};

struct rm : meta::describe_instruction<0xf8, 11, 1>
{
  static constexpr auto name = "rm";

  template <typename Machine> void operator()(state<Machine>& state) const
  {
    if (state.cc.s != 0)
    {
      state.pc = state.read_memory(state.sp) | (state.read_memory(state.sp + 1) << 8);
      state.sp += 2;
    }
  }
};

struct sphl : meta::describe_instruction<0xf9, 5, 1>
{
  static constexpr auto name = "sphl";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.sp = state.hl();
  }
};

struct jm_adr : meta::describe_instruction<0xfa, 10, 3>
{
  static constexpr auto name = "jm_adr";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    if (state.cc.s != 0)
    {
      state.pc = (state.op2() << 8) | state.op1();
    }
    else
    {
      state.pc += 2;
    }
  }
};

struct ei : meta::describe_instruction<0xfb, 4, 1>
{
  static constexpr auto name = "ei";

  template <typename Machine> void operator()(state<Machine>& state) const noexcept
  {
    state.enable_interrupt();
  }
};

struct cm_adr : meta::describe_instruction<0xfc, 17, 3>
{
  static constexpr auto name = "cm_adr";

  template <typename Machine> void operator()(state<Machine>& state) const
  {
    if (state.cc.s != 0)
    {
      const auto ret = state.pc + 2;
      state.write_memory(state.sp - 1, (ret >> 8) & 0x00ff);
      state.write_memory(state.sp - 2, ret & 0x00ff);
      state.sp -= 2;
      state.pc = (state.op2() << 8) | state.op1();
    }
    else
    {
      state.pc += 2;
    }
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

  template <typename Machine> void operator()(state<Machine>& state) const
  {
    const auto ret = state.pc + 2;
    state.write_memory(state.sp - 1, (ret >> 8) & 0x00ff);
    state.write_memory(state.sp - 2, ret & 0x00ff);
    state.sp -= 2;
    state.pc = 0x0038;
  }
};

/*------------------------------------------------------------------------------------------------*/

using instructions_8080 = meta::make_instructions<
  nop,
  lxi_b,
  stax_b,
  inx_b,
  inr_b,
  dcr_b,
  mvi_b,
  rlc,
  meta::unimplemented<0x08>,
  dad_b,
  ldax_b,
  dcx_b,
  inr_c,
  dcr_c,
  mvi_c,
  rrc,
  meta::unimplemented<0x10>,
  lxi_d,
  stax_d,
  inx_d,
  inr_d,
  dcr_d,
  mvi_d,
  ral,
  meta::unimplemented<0x18>,
  dad_d,
  ldax_d,
  dcx_d,
  inr_e,
  dcr_e,
  mvi_e,
  rar,
  rim,
  lxi_h,
  shld_adr,
  inx_h,
  inr_h,
  dcr_h,
  mvi_h,
  daa,
  meta::unimplemented<0x28>,
  dad_h,
  lhld_adr,
  dcx_h,
  inr_l,
  dcr_l,
  mvi_l_d8,
  cma,
  sim,
  lxi_sp,
  sta_adr,
  inx_sp,
  inr_m,
  dcr_m,
  mvi_m,
  stc,
  meta::unimplemented<0x38>,
  dad_sp,
  lda_adr,
  dcx_sp,
  inr_a,
  dcr_a,
  mvi_a,
  cmc,
  mov_b_b,
  mov_b_c,
  mov_b_d,
  mov_b_e,
  mov_b_h,
  mov_b_l,
  mov_b_m,
  mov_b_a,
  mov_c_b,
  mov_c_c,
  mov_c_d,
  mov_c_e,
  mov_c_h,
  mov_c_l,
  mov_c_m,
  mov_c_a,
  mov_d_b,
  mov_d_c,
  mov_d_d,
  mov_d_e,
  mov_d_h,
  mov_d_l,
  mov_d_m,
  mov_d_a,
  mov_e_b,
  mov_e_c,
  mov_e_d,
  mov_e_e,
  mov_e_h,
  mov_e_l,
  mov_e_m,
  mov_e_a,
  mov_h_b,
  mov_h_c,
  mov_h_d,
  mov_h_e,
  mov_h_h,
  mov_h_l,
  mov_h_m,
  mov_h_a,
  mov_l_b,
  mov_l_c,
  mov_l_d,
  mov_l_e,
  mov_l_h,
  mov_l_l,
  mov_l_m,
  mov_l_a,
  mov_m_b,
  mov_m_c,
  mov_m_d,
  mov_m_e,
  mov_m_h,
  mov_m_l,
  hlt,
  mov_m_a,
  mov_a_b,
  mov_a_c,
  mov_a_d,
  mov_a_e,
  mov_a_h,
  mov_a_l,
  mov_a_m,
  mov_a_a,
  add_b,
  add_c,
  add_d,
  add_e,
  add_h,
  add_l,
  add_m,
  add_a,
  adc_b,
  adc_c,
  adc_d,
  adc_e,
  adc_h,
  adc_l,
  adc_m,
  adc_a,
  sub_b,
  sub_c,
  sub_d,
  sub_e,
  sub_h,
  sub_l,
  sub_m,
  sub_a,
  sbb_b,
  sbb_c,
  sbb_d,
  sbb_e,
  sbb_h,
  sbb_l,
  sbb_m,
  sbb_a,
  ana_b,
  ana_c,
  ana_d,
  ana_e,
  ana_h,
  ana_l,
  ana_m,
  ana_a,
  xra_b,
  xra_c,
  xra_d,
  xra_e,
  xra_h,
  xra_l,
  xra_m,
  xra_a,
  ora_b,
  ora_c,
  ora_d,
  ora_e,
  ora_h,
  ora_l,
  ora_m,
  ora_a,
  cmp_b,
  cmp_c,
  cmp_d,
  cmp_e,
  cmp_h,
  cmp_l,
  cmp_m,
  cmp_a,
  rnz,
  pop_b,
  jnz,
  jmp,
  cnz,
  push_b,
  adi,
  rst_0,
  rz,
  ret,
  jz_adr,
  meta::unimplemented<0xcb>,
  cz_adr,
  call_adr,
  aci,
  rst_1,
  rnc,
  pop_d,
  jnc_adr,
  out,
  cnc_adr,
  push_d,
  sui,
  rst_2,
  rc,
  meta::unimplemented<0xd9>,
  jc_adr,
  in,
  cc_adr,
  meta::unimplemented<0xdd>,
  sbi,
  rst_3,
  rpo,
  pop_h,
  jpo_adr,
  xthl,
  cpo_adr,
  push_h,
  ani_d8,
  rst_4,
  rpe,
  pchl,
  jpe,
  xchg,
  cpe_adr,
  meta::unimplemented<0xed>,
  xri,
  rst_5,
  rp,
  pop_psw,
  jp_adr,
  di,
  cp_adr,
  push_psw,
  ori,
  rst_6,
  rm,
  sphl,
  jm_adr,
  ei,
  cm_adr,
  meta::unimplemented<0xfd>,
  cpi,
  rst_7
>;

/*------------------------------------------------------------------------------------------------*/

} // namespace cpp8080::specific
