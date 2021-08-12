struct PV1000 : Emulator {
  PV1000();
  auto load(Menu) -> void override;
  auto load() -> bool override;
  auto save() -> bool override;
  auto pak(ares::Node::Object) -> shared_pointer<vfs::directory> override;
};

PV1000::PV1000() {
  manufacturer = "Casio";
  name = "PV-1000";

  for(auto id : range(2)) {
   InputPort port{string{"Controller Port ", 1 + id}};

   { InputDevice device{"Gamepad"};
     device.digital("Up",     virtualPorts[0].pad.up);
     device.digital("Down",   virtualPorts[0].pad.down);
     device.digital("Left",   virtualPorts[0].pad.left);
     device.digital("Right",  virtualPorts[0].pad.right);
     device.digital("1",      virtualPorts[0].pad.a);
     device.digital("2",      virtualPorts[0].pad.b);
     device.digital("Start",  virtualPorts[0].pad.start);
     device.digital("Select", virtualPorts[0].pad.select);
     port.append(device); }

     ports.append(port);
  }
}

auto PV1000::load() -> bool {
  game = mia::Medium::create("PV-1000");
  if(!game->load(Emulator::load(game, configuration.game))) return false;

  system = mia::System::create("PV-1000");
  if(!system->load()) return false;

  if(!ares::PV1000::load(root, "[Casio] PV-1000")) return false;

  if(auto port = root->find<ares::Node::Port>("Cartridge Slot")) {
    port->allocate();
    port->connect();
  }

  return true;
}

auto PV1000::save() -> bool {
  root->save();
  system->save(system->location);
  game->save(game->location);
  return true;
}

auto PV1000::pak(ares::Node::Object node) -> shared_pointer<vfs::directory> {
  if(node->name() == "PV-1000") return system->pak;
  if(node->name() == "PV-1000 Cartridge") return game->pak;
  return {};
}
