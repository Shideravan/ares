struct Mega32X : Emulator {
  Mega32X();
  auto load() -> bool override;
  auto save() -> bool override;
  auto pak(ares::Node::Object) -> shared_pointer<vfs::directory> override;

  shared_pointer<mia::Pak> disc;
  u32 regionID = 0;
};

Mega32X::Mega32X() {
  manufacturer = "Sega";
  name = "Mega 32X";

  for(auto id : range(2)) {
    InputPort port{string{"Controller Port ", 1 + id}};

  { InputDevice device{"Fighting Pad"};
    device.digital("Up",    virtualPorts[id].pad.up);
    device.digital("Down",  virtualPorts[id].pad.down);
    device.digital("Left",  virtualPorts[id].pad.left);
    device.digital("Right", virtualPorts[id].pad.right);
    device.digital("A",     virtualPorts[id].pad.a);
    device.digital("B",     virtualPorts[id].pad.b);
    device.digital("C",     virtualPorts[id].pad.c);
    device.digital("X",     virtualPorts[id].pad.x);
    device.digital("Y",     virtualPorts[id].pad.y);
    device.digital("Z",     virtualPorts[id].pad.z);
    device.digital("Mode",  virtualPorts[id].pad.mode);
    device.digital("Start", virtualPorts[id].pad.start);
    port.append(device); }

    ports.append(port);
  }
}

auto Mega32X::load() -> bool {
  game = mia::Medium::create("Mega 32X");
  if(!game->load(Emulator::load(game, configuration.game))) return false;

  auto region = Emulator::region();
  //if statements below are ordered by lowest to highest priority
  if(region == "PAL"   ) regionID = 2;
  if(region == "NTSC-J") regionID = 1;
  if(region == "NTSC-U") regionID = 0;

  string name;
  if(game->pak->attribute("megacd").boolean()) {
    //use Mega CD firmware settings
    vector<Firmware> firmware;
    for(auto& emulator : emulators) {
      if(emulator->name == "Mega CD") firmware = emulator->firmware;
    }
    if(!firmware) return false;  //should never occur
    name = "Mega CD 32X";
    system = mia::System::create("Mega CD 32X");
    if(!system->load(firmware[regionID].location)) return errorFirmware(firmware[regionID], "Mega CD"), false;

    disc = mia::Medium::create("Mega CD");
    if(!disc->load(Emulator::load(disc, configuration.game))) disc.reset();
  } else {
    name = "Mega 32X";
    system = mia::System::create("Mega 32X");
    if(!system->load()) return false;
  }

  if(!ares::MegaDrive::load(root, {"[Sega] ", name, " (", region, ")"})) return false;

  if(auto port = root->find<ares::Node::Port>("Cartridge Slot")) {
    port->allocate();
    port->connect();
  }

  if(auto port = root->find<ares::Node::Port>("Mega CD/Disc Tray")) {
    port->allocate();
    port->connect();
  }

  if(auto port = root->find<ares::Node::Port>("Controller Port 1")) {
    port->allocate("Fighting Pad");
    port->connect();
  }

  if(auto port = root->find<ares::Node::Port>("Controller Port 2")) {
    port->allocate("Fighting Pad");
    port->connect();
  }

  return true;
}

auto Mega32X::save() -> bool {
  root->save();
  system->save(system->location);
  game->save(game->location);
  if(disc) disc->save(disc->location);
  return true;
}

auto Mega32X::pak(ares::Node::Object node) -> shared_pointer<vfs::directory> {
  if(node->name() == "Mega Drive") return system->pak;
  if(node->name() == "Mega Drive Cartridge") return game->pak;
  if(node->name() == "Mega CD Disc" && disc) return disc->pak;
  return {};
}
