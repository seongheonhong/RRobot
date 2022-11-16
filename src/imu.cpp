#include "imu.h"
#include "utils.h"

MPU9250 mpu;

float yaw=0, pitch=0, roll=0; // z(downward), y, x
std::deque<float> acc_x, acc_y, acc_z;
float vel_x=0, vel_y=0, vel_z=0;
float pos_x=0, pos_y=0, pos_z=0;

void imu_setup() {
  Wire.begin();
  
  MPU9250Setting setting;
  setting.accel_fs_sel = ACCEL_FS_SEL::A16G;
  setting.gyro_fs_sel = GYRO_FS_SEL::G2000DPS;
  setting.mag_output_bits = MAG_OUTPUT_BITS::M16BITS;
  setting.fifo_sample_rate = FIFO_SAMPLE_RATE::SMPL_200HZ;
  setting.gyro_fchoice = 0x03;
  setting.gyro_dlpf_cfg = GYRO_DLPF_CFG::DLPF_41HZ;
  setting.accel_fchoice = 0x01;
  setting.accel_dlpf_cfg = ACCEL_DLPF_CFG::DLPF_45HZ;

  if (!mpu.setup(0x68, setting)) {
    while (1) {
      DEBUG_SERIAL.println("IMU setup failed!");
      delay(5000);
    }
  }
  DEBUG_SERIAL.println("IMU setup done!");
}

void imu_calibration(bool calibrate_mag) {
  DEBUG_SERIAL.println("Accel Gyro calibration will start in 5sec.");
  DEBUG_SERIAL.println("Please leave the device still on the flat plane.");
  mpu.verbose(true);
  delay(5000);
  mpu.calibrateAccelGyro();
  print_acc_gyro_calibration();

  if (calibrate_mag) {
    DEBUG_SERIAL.println("Mag calibration will start in 5sec.");
    DEBUG_SERIAL.println("Please Wave device in a figure eight until done.");
    delay(5000);
    mpu.calibrateMag();
    print_mag_calibration();
  }
  mpu.verbose(false);
}

bool imu_update() {
  if (mpu.update()) {
    yaw = mpu.getYaw();
    pitch = mpu.getPitch();
    roll = mpu.getRoll();
    acc_x.push_back(mpu.getLinearAccX());
    acc_y.push_back(mpu.getLinearAccY());
    acc_z.push_back(mpu.getLinearAccZ());
    return true;
  } else {
    return false;
  }
}

void print_acc_gyro_calibration() {
  DEBUG_SERIAL.println("< calibration parameters >");
  DEBUG_SERIAL.println("accel bias [g]: ");
  DEBUG_SERIAL.print(mpu.getAccBiasX() * 1000.f / (float)MPU9250::CALIB_ACCEL_SENSITIVITY);
  DEBUG_SERIAL.print(", ");
  DEBUG_SERIAL.print(mpu.getAccBiasY() * 1000.f / (float)MPU9250::CALIB_ACCEL_SENSITIVITY);
  DEBUG_SERIAL.print(", ");
  DEBUG_SERIAL.print(mpu.getAccBiasZ() * 1000.f / (float)MPU9250::CALIB_ACCEL_SENSITIVITY);
  DEBUG_SERIAL.println();
  DEBUG_SERIAL.println("gyro bias [deg/s]: ");
  DEBUG_SERIAL.print(mpu.getGyroBiasX() / (float)MPU9250::CALIB_GYRO_SENSITIVITY);
  DEBUG_SERIAL.print(", ");
  DEBUG_SERIAL.print(mpu.getGyroBiasY() / (float)MPU9250::CALIB_GYRO_SENSITIVITY);
  DEBUG_SERIAL.print(", ");
  DEBUG_SERIAL.print(mpu.getGyroBiasZ() / (float)MPU9250::CALIB_GYRO_SENSITIVITY);
  DEBUG_SERIAL.println();
}

void print_mag_calibration() {
  DEBUG_SERIAL.println("mag bias [mG]: ");
  DEBUG_SERIAL.print(mpu.getMagBiasX());
  DEBUG_SERIAL.print(", ");
  DEBUG_SERIAL.print(mpu.getMagBiasY());
  DEBUG_SERIAL.print(", ");
  DEBUG_SERIAL.print(mpu.getMagBiasZ());
  DEBUG_SERIAL.println();
  DEBUG_SERIAL.println("mag scale []: ");
  DEBUG_SERIAL.print(mpu.getMagScaleX());
  DEBUG_SERIAL.print(", ");
  DEBUG_SERIAL.print(mpu.getMagScaleY());
  DEBUG_SERIAL.print(", ");
  DEBUG_SERIAL.print(mpu.getMagScaleZ());
  DEBUG_SERIAL.println();
}

void print_rpy() {
  DEBUG_SERIAL.print("Yaw, Pitch, Roll: ");
  DEBUG_SERIAL.print(yaw, 2);
  DEBUG_SERIAL.print(", ");
  DEBUG_SERIAL.print(pitch, 2);
  DEBUG_SERIAL.print(", ");
  DEBUG_SERIAL.print(roll, 2);
  DEBUG_SERIAL.println();
}

void print_acc() {
  DEBUG_SERIAL.print("ACC X, Y, Z: ");
  DEBUG_SERIAL.print(average(acc_x), 2);
  DEBUG_SERIAL.print(", ");
  DEBUG_SERIAL.print(average(acc_y), 2);
  DEBUG_SERIAL.print(", ");
  DEBUG_SERIAL.print(average(acc_z), 2);
  DEBUG_SERIAL.println();
}

float average(std::deque<float>& deq) {
  float sum=0;
  for(auto& element: deq) {
    sum += element;
  }
  return sum/(float)deq.size();
}

void calc_pos() {
  static uint32_t past_ms = millis();
  if (acc_x.size() == 50) {
    float time_step = (float)(millis() - past_ms)*0.001f;
    vel_x += average(acc_x)*time_step;
    vel_y += average(acc_y)*time_step;
    vel_z += average(acc_z)*time_step;

    pos_x += vel_x*time_step;
    pos_y += vel_y*time_step;
    pos_z += vel_z*time_step;

    acc_x.pop_front();
    acc_y.pop_front();
    acc_z.pop_front();
    past_ms = millis();
  }
}

void print_pos() {
  DEBUG_SERIAL.print("POS X, Y, Z: ");
  DEBUG_SERIAL.print(pos_x, 2);
  DEBUG_SERIAL.print(", ");
  DEBUG_SERIAL.print(pos_y, 2);
  DEBUG_SERIAL.print(", ");
  DEBUG_SERIAL.print(pos_z, 2);
  DEBUG_SERIAL.println();
}

void print_vel() {
  DEBUG_SERIAL.print("VEL X, Y, Z: ");
  DEBUG_SERIAL.print(vel_x, 2);
  DEBUG_SERIAL.print(", ");
  DEBUG_SERIAL.print(vel_y, 2);
  DEBUG_SERIAL.print(", ");
  DEBUG_SERIAL.print(vel_z, 2);
  DEBUG_SERIAL.println();
}
