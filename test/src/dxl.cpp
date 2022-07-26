#include "dxl.h"
#include "utils.h"

Dynamixel dxl(PIN_RTS);

void dxl_setup() {
  DXL_SERIAL.begin(DXL_SERIAL_BAUDRATE, SERIAL_8N1_HALF_DUPLEX);
  dxl.attach(DXL_SERIAL, DXL_SERIAL_BAUDRATE);
  for (uint8_t id: joint_ids) {
    dxl.addModel<DxlModel::X>(id);
  }
  delay(2000);

  bool connected = true;
  for (uint8_t id: joint_ids) {
    if (!dxl.ping(id)) { connected = false; break; }
    dxl.torqueEnable(id, false);
  }

  if (connected) {
    DEBUG_SERIAL.println("DXL setup done!");
  } else {
    DEBUG_SERIAL.println("DXL setup failed!");
  }    
}

void set_dxl_parameters() {
  for (uint8_t id: hand_joint_ids) {
    dxl.positionPGain(id, 400);
    dxl.positionDGain(id, 500);
    dxl.positionIGain(id, 40);
    dxl.profileVelocity(id, 300);
    dxl.profileAcceleration(id, 300);
    delay(10);
  }
  for (uint8_t id: arm_joint_ids) {
    dxl.positionPGain(id, 400);
    dxl.positionDGain(id, 500);
    dxl.positionIGain(id, 40);
    dxl.profileVelocity(id, 300);
    dxl.profileAcceleration(id, 300);
    delay(10);
  }
  for (uint8_t id: rotator_ids) {
    dxl.positionPGain(id, 1600);    
    dxl.positionIGain(id, 120);    
    delay(10);
  }
}

void set_joint_position_limit(std::vector<uint8_t> id_set, int min, int max) {
  for (uint8_t id: id_set) {
    dxl.torqueEnable(id, false);
    dxl.minPositionLimit(id, min);
    dxl.maxPositionLimit(id, max);    
    dxl.verbose(id);
    delay(10);
  }
}

void initialize_joint_positions(std::vector<uint8_t> id_set, int position) {
  for (uint8_t id: id_set) {
    dxl.torqueEnable(id, true);
    dxl.goalPosition(id, position);
    dxl.verbose(id);
    delay(10);
  }
  while (1) {
    bool initialized = true;
    for (uint8_t id: id_set) {
      initialized = initialized && !dxl.moving(id);
    }
    if (initialized) { break; }
  }
}

void print_joint_positions() {
  for (uint8_t id: joint_ids) {
    int pos = dxl.presentPosition(id);
    if (pos == -1) {
      DEBUG_SERIAL.printf("MOTOR%d ERROR: %d\t", id, dxl.hardwareErrorStatus(id));
      continue;
    }
    DEBUG_SERIAL.printf("MOTOR%d: %d\t", id, pos);
    dxl.verbose(id);
  }
  DEBUG_SERIAL.println();
}

void set_joint_operating_modes(){
  for (uint8_t id: arm_joint_ids){
    dxl.operatingMode(id, 0x03);  
  }
  for (uint8_t id: hand_joint_ids){
    dxl.operatingMode(id, 0x03);  
  }    
  for (uint8_t id: rotator_ids){
    dxl.operatingMode(id, 0x03);  
  }    
  for (uint8_t id: pulley_ids){
    dxl.operatingMode(id, 0x10);  
  }    
}