auto CPU::mdr() const -> n8 {
  return (bus.mdr | bus.pullup) & ~bus.pulldown;
}

auto CPU::read(n16 address) -> n8 {
  n8 data = mdr();
  if(address >= 0xc000 && bus.ramEnable) data = ram.read(address);
  // if(bus.biosEnable) data = bios.read(address, data); No bios
  data = cartridge.read(address, data);
  return bus.mdr = data;
}

auto CPU::write(n16 address, n8 data) -> void {
  bus.mdr = data;
  if(address >= 0xc000 && bus.ramEnable) ram.write(address, data);
  //if(bus.biosEnable) bios.write(address, data);
  if(bus.cartridgeEnable) cartridge.write(address, data);
}

auto CPU::in(n16 address) -> n8 {
  n8 data = mdr();
  if(0);


  else if((address & 0xc1) == 0x00) {
    data = mdr();
  }

  else if((address & 0xc1) == 0x40) {
    data = vdp.vcounterQuery();
  }

  else if((address & 0xc1) == 0x41) {
    data = vdp.hcounterQuery();
  }

  else if((address & 0xc1) == 0x80) {
    data = vdp.data();
  }

  else if((address & 0xc1) == 0x81) {
    data = vdp.status();
  }

  else if((address & 0xc1) == 0xc0) {
    auto port1 = controllerPort1.read();
    auto port2 = controllerPort2.read();
    data.bit(0,5) = port1.bit(0,5);
    data.bit(6,7) = port2.bit(0,1);
    if(controllerPort1.trOutput()) data.bit(5) = controllerPort1.trLevel;
  }

  else if((address & 0xc1) == 0xc1) {
    auto port1 = controllerPort1.read();
    auto port2 = controllerPort2.read();
    data.bit(0,3) = port2.bit(2,5);
    data.bit(4)   = Region::NTSCJ() ? 1 : !system.controls.reset->value();
    data.bit(5)   = 1;
    data.bit(6)   = port1.bit(6);
    data.bit(7)   = port2.bit(6);
    if(controllerPort2.trOutput()) data.bit(3) = controllerPort2.trLevel;
    if(controllerPort1.thOutput()) data.bit(6) = controllerPort1.thLevel;
    if(controllerPort2.thOutput()) data.bit(7) = controllerPort2.thLevel;
  }

  debugger.in(address, data);
  return data;
}

auto CPU::out(n16 address, n8 data) -> void {
  if(0);


  else if((address & 0xc1) == 0x00) {
    bus.ioEnable        = !data.bit(2);
    bus.biosEnable      = !data.bit(3);
    bus.ramEnable       = !data.bit(4);
    bus.cardEnable      = !data.bit(5);
    bus.cartridgeEnable = !data.bit(6);
    bus.expansionEnable = !data.bit(7);
  }

  else if((address & 0xc1) == 0x01) {
    auto thLevel1 = controllerPort1.thLevel;
    auto thLevel2 = controllerPort2.thLevel;

    controllerPort1.trDirection = data.bit(0);
    controllerPort1.thDirection = data.bit(1);
    controllerPort2.trDirection = data.bit(2);
    controllerPort2.thDirection = data.bit(3);
    //NTSC-J region sets thLevel to thDirection
    controllerPort1.thLevel = controllerPort1.thDirection;
    controllerPort2.thLevel = controllerPort2.thDirection;


    if(!thLevel1 && controllerPort1.thLevel) vdp.hcounterLatch();
    if(!thLevel2 && controllerPort2.thLevel) vdp.hcounterLatch();
  }

  else if((address & 0xc1) == 0x80) {
    vdp.data(data);
  }

  else if((address & 0xc1) == 0x81) {
    vdp.control(data);
  }

  debugger.out(address, data);
  return;
}
