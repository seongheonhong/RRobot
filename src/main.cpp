#include "rrobot.h"
#include "dxl.h"
#include "imu.h"
#include "utils.h"
#include "func.h"
#include <sbus.h>
#if REMOTE
  #include <sstream>
  #include <vector>
#endif
#define BLUETOOTH_SERIAL            Serial1
const uint8_t btGndPin = 12;
const uint8_t btPwrPin = 11;

SbusRx sbus_rx(&SBUS_SERIAL);
SbusRx sbus_tx(&SBUS_SERIAL);

void setup() {
  Serial.begin(1000000);  
  Serial.println("STARTED");
   
  dxl_setup();  
  delay(2000);
  set_dxl_parameters();  
  set_joint_position_limit(arm_joint_ids, ARM_JOINT_MIN_POS, ARM_JOINT_MAX_POS);
  set_joint_position_limit(hand_joint_ids, HAND_JOINT_MIN_POS, HAND_JOINT_MAX_POS);
  initialize_joint_positions(arm_joint_ids, ARM_JOINT_INITIAL_POS);
  initialize_joint_positions(hand_joint_ids, HAND_JOINT_INITIAL_POS);
    
  pinMode(btGndPin, INPUT);
  pinMode(btPwrPin, INPUT);    
  BLUETOOTH_SERIAL.begin(115200);
  
  imu_setup();
  imu_calibration(MAG_CALI_DISABLE);  
  print_joint_positions();  

  sbus_rx.Begin();  
  sbus_tx.Begin(); 
}

void loop() {

  #if REMOTE
  String input_str = "";    
  char ser_read;
  if (BLUETOOTH_SERIAL.available()) {
    while (BLUETOOTH_SERIAL.available()) {
      // ser_read = BLUETOOTH_SERIAL.read();
      // input_str += ser_read;
      input_str = BLUETOOTH_SERIAL.readStringUntil('\n');
    }
    processBTSerial(input_str, ' ');
  }
  else {
    delay(1);
  }    
  #endif
  if (imu_update()) {
    static uint32_t prev_ms = millis();
    if (millis() > prev_ms + 500) {
      print_rpy();            
      print_joint_positions();
      prev_ms = millis();
    }
  } else {
    DEBUG_SERIAL.println("IMU No-update Error");
  }


   if (sbus_rx.Read()){    
    Serial.printf("%04d\t%04d\t%04d\n", sbus_rx.rx_channels()[0], sbus_rx.rx_channels()[2], sbus_rx.rx_channels()[3]);
    dxl.goalPosition(11, map(sbus_rx.rx_channels()[0], 100, 1900, ARM_JOINT_MIN_POS, ARM_JOINT_MAX_POS));
    dxl.goalPosition(21, map(sbus_rx.rx_channels()[2], 490, 1560, HAND_JOINT_MIN_POS, HAND_JOINT_MAX_POS));
    dxl.goalPwm(31, map(sbus_rx.rx_channels()[3], 385, 1663, -885, 885));    
   }
}