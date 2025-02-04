struct WonderSwan : Emulator {
  WonderSwan();
  auto load(Menu) -> void override;
  auto load() -> bool override;
  auto save() -> bool override;
  auto pak(ares::Node::Object) -> shared_pointer<vfs::directory> override;
};

WonderSwan::WonderSwan() {
  manufacturer = "Bandai";
  name = "WonderSwan";

  { InputPort port{"WonderSwan"};

  { InputDevice device{"Controls"};
  
    // Horizontal position
    device.digital("X1",    virtualPorts[0].pad.up);    // Up (horizontal)
    device.digital("X2",    virtualPorts[0].pad.right); // Right (horizontal)
    device.digital("X3",    virtualPorts[0].pad.down);  // Down (horizontal)
    device.digital("X4",    virtualPorts[0].pad.left);  // Left (horizontal)
    device.digital("A",     virtualPorts[0].pad.a);
    device.digital("B",     virtualPorts[0].pad.b);

    // Vertical position
    device.digital("Y1",    virtualPorts[0].pad.rleft);  // Left (vertical)
    device.digital("Y2",    virtualPorts[0].pad.rup);    // Up (vertical)
    device.digital("Y3",    virtualPorts[0].pad.rright); // Right (vertical)
    device.digital("Y4",    virtualPorts[0].pad.rdown);  // Down (vertical)
    device.digital("Start", virtualPorts[0].pad.start);
    port.append(device); }

    ports.append(port);
  }
}

auto WonderSwan::load(Menu menu) -> void {
  Menu orientationMenu{&menu};
  orientationMenu.setText("Orientation").setIcon(Icon::Device::Display);
  if(auto orientations = root->find<ares::Node::Setting::String>("PPU/Screen/Orientation")) {
    Group group;
    for(auto& orientation : orientations->readAllowedValues()) {
      MenuRadioItem item{&orientationMenu};
      item.setText(orientation);
      item.onActivate([=] {
        if(auto orientations = root->find<ares::Node::Setting::String>("PPU/Screen/Orientation")) {
          orientations->setValue(orientation);
        }
      });
      group.append(item);
    }
  }
}

auto WonderSwan::load() -> bool {
  game = mia::Medium::create("WonderSwan");
  if(!game->load(Emulator::load(game, configuration.game))) return false;

  system = mia::System::create("WonderSwan");
  if(!system->load()) return false;

  if(!ares::WonderSwan::load(root, "[Bandai] WonderSwan")) return false;

  if(auto port = root->find<ares::Node::Port>("Cartridge Slot")) {
    port->allocate();
    port->connect();
  }

  return true;
}

auto WonderSwan::save() -> bool {
  root->save();
  system->save(system->location);
  game->save(game->location);
  return true;
}

auto WonderSwan::pak(ares::Node::Object node) -> shared_pointer<vfs::directory> {
  if(node->name() == "WonderSwan") return system->pak;
  if(node->name() == "WonderSwan Cartridge") return game->pak;
  return {};
}
