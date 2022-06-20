#pragma once
#include <Arduino.h>
#include <Dynamixel.h>
#include <vector>

#define DXL_SERIAL Serial2

const uint8_t PIN_RTS = 11;
extern Dynamixel dxl;

void dxl_setup();
void set_dxl_parameters();
void set_joint_position_limit(std::vector<uint8_t> id_set, int min, int max);
void initialize_joint_positions(std::vector<uint8_t> id_set, int position);
void print_joint_positions();