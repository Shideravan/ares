#include <pv1000/pv1000.hpp>

namespace ares::PV1000 {

auto enumerate() -> vector<string> {
  return {
    "[Casio] PV-1000",
  };
}

auto load(Node::System& node, string name) -> bool {
  if(!enumerate().find(name)) return false;
  return system.load(node, name);
}

Scheduler scheduler;
BIOS bios;
System system;
#include "controls.cpp"
#include "serialization.cpp"

auto System::game() -> string {
  if(cartridge.pak) {
    return cartridge.title();
  }

  return "(no cartridge connected)";
}

auto System::run() -> void {
  scheduler.enter();
  controls.poll();
}

auto System::load(Node::System& root, string name) -> bool {
  if(node) unload();

  information = {};
  if(name.find("PV-1000")) {
    information.name = "PV-1000";
    information.model = Model::PV1000;
    information.region = Region::NTSCJ;
  }

  node = Node::System::create(information.name);
  node->setGame({&System::game, this});
  node->setRun({&System::run, this});
  node->setPower({&System::power, this});
  node->setSave({&System::save, this});
  node->setUnload({&System::unload, this});
  node->setSerialize({&System::serialize, this});
  node->setUnserialize({&System::unserialize, this});
  root = node;
  if(!node->setPak(pak = platform->pak(node))) return false;

  scheduler.reset();
  controls.load(node);
  bios.load(node);
  cartridgeSlot.load(node);
  cpu.load(node);
  vdp.load(node);
  psg.load(node);
  if(Device::PV1000()) {
    controllerPort1.load(node);
    controllerPort2.load(node);
  }
  return true;
}

auto System::save() -> void {
  if(!node) return;
  cartridge.save();
}

auto System::unload() -> void {
  if(!node) return;
  save();
  cartridgeSlot.unload();
  cpu.unload();
  vdp.unload();
  psg.unload();
  if(Device::PV1000()) {
    controllerPort1.unload();
    controllerPort2.unload();
  }
  bios.unload();
  node.reset();
}

auto System::power(bool reset) -> void {
  for(auto& setting : node->find<Node::Setting::Setting>()) setting->setLatch();

  if(cartridge.pak) {
    information.pv = cartridge.pak->attribute("pv").boolean();
  }

  bios.power();
  cartridge.power();
  cpu.power();
  vdp.power();
  psg.power();
  if(Device::PV1000()) {
    controllerPort1.power();
    controllerPort2.power();
  }
  scheduler.power(cpu);
}

}
