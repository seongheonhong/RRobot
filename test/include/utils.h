#pragma once
#include <Arduino.h>
#include <vector>

#define REMOTE 1

#define BLUETOOTH_SERIAL Serial3

#if REMOTE
#define DEBUG_SERIAL BLUETOOTH_SERIAL
#else
#define DEBUG_SERIAL Serial
#endif

#define ARM_JOINT_MIN_POS       512
#define ARM_JOINT_MAX_POS       3096
#define ARM_JOINT_INITIAL_POS   2048
#define HAND_JOINT_MIN_POS      1024
#define HAND_JOINT_MAX_POS      3072
#define HAND_JOINT_INITIAL_POS  2048

extern std::vector<uint8_t> joint_ids;
extern std::vector<uint8_t> arm_joint_ids;
extern std::vector<uint8_t> hand_joint_ids;
extern std::vector<uint8_t> trigger_pins;