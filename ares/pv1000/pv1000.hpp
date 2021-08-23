#pragma once
//started: 2021-08-11

#include <ares/ares.hpp>

#include <component/processor/z80/z80.hpp>

namespace ares::PV1000 {
  #include <ares/inline.hpp>
  auto enumerate() -> vector<string>;
  auto load(Node::System& node, string name) -> bool;


  struct Region {
    inline static auto NTSCJ() -> bool;
  };

  #include <pv1000/controller/controller.hpp>
  #include <pv1000/cpu/cpu.hpp>
  #include <pv1000/vdp/vdp.hpp>
  #include <pv1000/system/system.hpp>
  #include <pv1000/cartridge/cartridge.hpp>
}