#include "dxl.h"
#include "imu.h"
#include "utils.h"
#include "func.h"
#include <sbus.h>

SbusRx sbus_rx(&SBUS_SERIAL);
SbusRx sbus_tx(&SBUS_SERIAL);

#if REMOTE
#include <sstream>
#include <vector>
#endif

const uint8_t btGndPin = 12;
const uint8_t btPwrPin = 11;

void minmax_swing();

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
  dxl.shutdown(31, 0b00010010);
  dxl.torqueEnable(31, true);
  /*
  // put your setup code here, to run once:
  pinMode(trigger_pins[0], OUTPUT);
  pinMode(btGndPin, INPUT);
  pinMode(btPwrPin, INPUT);  

  DEBUG_SERIAL.begin(115200);
  delay(1000);
  BLUETOOTH_SERIAL.begin(115200);
  delay(1000);

  DEBUG_SERIAL.println("CP0");
  
  
  set_joint_position_limit(rotator_ids, ROTATOR_MIN_POS, ROTATOR_MAX_POS);

  DEBUG_SERIAL.println("CP1");
  imu_setup();
  imu_calibration(MAG_CALI_DISABLE);
  
  print_joint_positions();  
  delay(2000);

  
  DEBUG_SERIAL.println("CP4");
  initialize_joint_positions(rotator_ids, ROTATOR_MIN_POS+1);
  DEBUG_SERIAL.println("CP5");
  delay(2000);
  
  */  
  sbus_rx.Begin();  
  sbus_tx.Begin(); 
}

void loop() {
  
  /*
  //put your main code here, to run repeatedly:
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
  #else

  if (imu_update()) {
    static uint32_t prev_ms = millis();
    if (millis() > prev_ms + 500) {
      print_rpy();
      minmax_swing();
      print_joint_positions();
      prev_ms = millis();
    }
  } else {
    //DEBUG_SERIAL.println("IMU not updating!!!!");
  }

  #endif
}

void minmax_swing() {
  static int arm_goal_position = ARM_JOINT_MAX_POS;
  DEBUG_SERIAL.print("arm goal position = ");
  DEBUG_SERIAL.println(arm_goal_position);
  for (uint8_t id: arm_joint_ids) {
    if (!dxl.moving(id)) {
      dxl.goalPosition(id, arm_goal_position);
      dxl.verbose(id);
      arm_goal_position = (arm_goal_position == ARM_JOINT_MAX_POS) ? ARM_JOINT_MIN_POS : ARM_JOINT_MAX_POS;
    }
  }

  static int hand_goal_position = HAND_JOINT_MAX_POS;
  DEBUG_SERIAL.print("hand goal position = ");
  DEBUG_SERIAL.println(hand_goal_position);
  for (uint8_t id: hand_joint_ids) {
    if (!dxl.moving(id)) {
      dxl.goalPosition(id, hand_goal_position);
      dxl.verbose(id);
      hand_goal_position = (hand_goal_position == HAND_JOINT_MAX_POS) ? HAND_JOINT_MIN_POS : HAND_JOINT_MAX_POS;
    }
  }
  */ 
   if (sbus_rx.Read()){
    Serial.print(sbus_rx.rx_channels()[0]);Serial.print("\t");    
    Serial.print(sbus_rx.rx_channels()[1]);Serial.print("\t");
    Serial.print(sbus_rx.rx_channels()[2]);Serial.print("\t");
    Serial.print(sbus_rx.rx_channels()[3]);Serial.print("\t");
    Serial.print(sbus_rx.rx_channels()[4]);Serial.print("\t");
    Serial.println(sbus_rx.rx_channels()[5]);
    dxl.goalPosition(11, map(sbus_rx.rx_channels()[0], 100, 1900, ARM_JOINT_MIN_POS, ARM_JOINT_MAX_POS));
    dxl.goalPosition(21, map(sbus_rx.rx_channels()[2], 490, 1560, HAND_JOINT_MIN_POS, HAND_JOINT_MAX_POS));
    dxl.goalPwm(31, map(sbus_rx.rx_channels()[3], 385, 1663, -885, 885));    
   }
}