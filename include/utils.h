#pragma once
#include <Arduino.h>
#include <vector>
#include "dxl.h"

#define ARM_JOINT_MIN_POS                   512
#define ARM_JOINT_MAX_POS                   3584
#define ARM_JOINT_INITIAL_POS               2048
#define HAND_JOINT_MIN_POS                  1024
#define HAND_JOINT_MAX_POS                  3072
#define HAND_JOINT_INITIAL_POS              2048
#define ROTATOR_MIN_POS                     2200
#define ROTATOR_MAX_POS                     3072
#define ROTATOR_INTIAL_POS                  2400

extern std::vector<uint8_t> joint_ids;
extern std::vector<uint8_t> arm_joint_ids;
extern std::vector<uint8_t> hand_joint_ids;
extern std::vector<uint8_t> rotator_ids;
extern std::vector<uint8_t> pulley_ids;
extern std::vector<uint8_t> trigger_pins;