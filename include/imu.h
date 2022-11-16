#pragma once
#include <Arduino.h>
#include <MPU9250.h>
#include <deque>

#define MAG_CALI_DISABLE 0

extern MPU9250 mpu;

void imu_setup();
void imu_calibration(bool calibrate_mag= true);
bool imu_update();

void print_acc_gyro_calibration();
void print_mag_calibration();
void print_rpy();
void print_acc();

float average(std::deque<float>& deq);
void calc_pos();
void print_vel();
void print_pos();