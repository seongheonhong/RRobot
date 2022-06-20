#include "dxl.h"
#include "imu.h"
#include "utils.h"

#if REMOTE
#include <sstream>
#include <vector>
#endif

void minmax_swing();

void setup() {
  // put your setup code here, to run once:
  DEBUG_SERIAL.begin(9600);
  delay(1000);
  BLUETOOTH_SERIAL.begin(9600);
  delay(1000);

  dxl_setup();
  set_dxl_parameters();
  set_joint_position_limit(arm_joint_ids, ARM_JOINT_MIN_POS, ARM_JOINT_MAX_POS);
  set_joint_position_limit(hand_joint_ids, HAND_JOINT_MIN_POS, HAND_JOINT_MAX_POS);

  imu_setup();
  imu_calibration(MAG_CALI_DISABLE);

  initialize_joint_positions(arm_joint_ids, ARM_JOINT_MIN_POS);
  initialize_joint_positions(hand_joint_ids, HAND_JOINT_MIN_POS);
  delay(2000);

  print_joint_positions();
}

void loop() {
  //put your main code here, to run repeatedly:
  #if REMOTE

  String input_str = "";
  char input_char;
  if (BLUETOOTH_SERIAL.available()) {
    while (BLUETOOTH_SERIAL.available()) {
      input_char = BLUETOOTH_SERIAL.read();
      input_str += input_char;
    }
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
}