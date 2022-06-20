#include <Arduino.h>
#include <Dynamixel.h>

#define BLUETOOTH_ON 0
#define MOTOR_BAUD_CHANGE 0

#if BLUETOOTH_ON

#define BLUETOOTH_SERIAL Serial3
#define DXL_SERIAL Serial2
#define DXL_BAUDRATE 3000000
#define TARGET_ID 1

enum OPMODE {
  NONE,
  INITIALIZE,
  MANUAL,
  DEBUG,
  HANDS_TO_SKY
};

Dynamixel dxl(11);

String read_keyboard_input() {
  String input_str = "";
  char input_char;
  if (BLUETOOTH_SERIAL.available()) {
    while (BLUETOOTH_SERIAL.available()) {
      input_char = BLUETOOTH_SERIAL.read();
      input_str += input_char;
      if (input_char == 0x0a) break;
    } 
  }
  return input_str;
} 

void setup() {
  BLUETOOTH_SERIAL.begin(9600);
  delay(1000);

  DXL_SERIAL.begin(DXL_BAUDRATE, SERIAL_HALF_DUPLEX);
  dxl.attach(DXL_SERIAL, DXL_BAUDRATE);
  dxl.addModel<DxlModel::X>(TARGET_ID);
  delay(1000);

  dxl.torqueEnable(TARGET_ID, true);
  delay(1000);
}

void loop() {

  static int op_mode = NONE;
  static bool print_statement = true;
  static bool type_opmode = false;

  if (print_statement) {
    switch (op_mode)
    {
    case NONE:
      BLUETOOTH_SERIAL.println("SELECT OPERATION");
      BLUETOOTH_SERIAL.println("1: INITIALIZE, 2: MANUAL, 3: DEBUG, 4: HANDS_TO_SKY");
      type_opmode = true;
      break;
    case INITIALIZE:
      BLUETOOTH_SERIAL.println("initializing joint positions...");
      break;
    case MANUAL:
      BLUETOOTH_SERIAL.println("manually set joint positions");
      break;
    case DEBUG:
      BLUETOOTH_SERIAL.println("debug");
      break;
    case HANDS_TO_SKY:
      BLUETOOTH_SERIAL.println("hands to sky!");
      break;
    default:
      break;
    }
    print_statement = false;
  }

  if (type_opmode) {
    String str = read_keyboard_input();
    if (str.length() > 0) {
      op_mode = str.toInt();
      type_opmode = false;
      print_statement = true;
    }
  }

  switch (op_mode)
  {
  case NONE:
    break;
  case INITIALIZE:
    if (!dxl.moving(TARGET_ID)) {
      dxl.goalPosition(TARGET_ID, 0);
      dxl.verbose(TARGET_ID);
    }
    break;
  case MANUAL:
    if (!dxl.moving(TARGET_ID)) {
      String goal_str = read_keyboard_input();
      if (goal_str.length() > 0) {
        dxl.goalPosition(TARGET_ID, goal_str.toInt());
        dxl.verbose(TARGET_ID); 
      }
    }
    break;
  case DEBUG:
    BLUETOOTH_SERIAL.println("debug");
    break;
  case HANDS_TO_SKY:
    BLUETOOTH_SERIAL.println("hands to sky!");
    break;
  default:
    break;
  }

}

#else

#if MOTOR_BAUD_CHANGE

#define DEBUG_SERIAL Serial
#define DEBUG_BAUDRATE 9600
#define DXL_SERIAL Serial2
#define DXL_BAUDRATE 3000000

Dynamixel dxl(11);

void setup() {
  // put your setup code here, to run once:
  DEBUG_SERIAL.begin(DEBUG_BAUDRATE);

  pinMode(6, OUTPUT);
  DXL_SERIAL.begin(DXL_BAUDRATE, SERIAL_HALF_DUPLEX);
  dxl.attach(DXL_SERIAL, DXL_BAUDRATE);
  
  dxl.addModel<DxlModel::X>(11);
  dxl.addModel<DxlModel::X>(22);
  delay(2000);

  dxl.torqueEnable(11, false);
  dxl.torqueEnable(22, false);
  //dxl.baudrate(11, 5);
  //dxl.baudrate(22, 5);
  delay(1000);
  DEBUG_SERIAL.printf("MOTOR %d: %d\n", 11, dxl.baudrate(11));
  DEBUG_SERIAL.printf("MOTOR %d: %d\n", 22, dxl.baudrate(22));
  delay(3000);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(6, LOW);
}

#else

#define DEBUG_SERIAL Serial
#define DEBUG_BAUDRATE 9600
#define DXL_SERIAL Serial2
#define DXL_BAUDRATE 3000000
#define WIRE_MOTOR_ID 22
#define DIR_MOTOR_ID 11
#define ARM_MOTOR_ID 3
#define HAND_MOTOR_ID 6
#define SOLENOID_PIN 4

#include <vector>

std::vector<int> motor_ids = {ARM_MOTOR_ID, HAND_MOTOR_ID, DIR_MOTOR_ID, WIRE_MOTOR_ID};
std::vector<int> motor_goals = {2048, 3072, 2300, 1370};
bool solenoid_on = false;
bool print_on = false;
bool initialized = false;
Dynamixel dxl(11);

void read_goal_pos() {
  String input_str = "";
  char input_char;

  int idx = 0;
  if (DEBUG_SERIAL.available()) {
    while (DEBUG_SERIAL.available()) {
      input_char = DEBUG_SERIAL.read();

      if (input_char == 's') {
        solenoid_on = true;
        break;
      } else if (input_char == 'p') {
        print_on = true;
        break;
      } else {
        initialized = true;
        if (input_char == ',' || input_char == 0x0d) {
          if (input_str.toInt() == 0) continue;
          motor_goals.at(idx++) = input_str.toInt();
          input_str = "";
          if (idx > 3) {
            break;
          }
        } else if (input_char >= 0x30 && input_char < 0x3a) {
          input_str += input_char;
        }
      }
    } 
  }
} 

void setup() {
  // put your setup code here, to run once:
  pinMode(SOLENOID_PIN, OUTPUT);
  pinMode(6, OUTPUT);
  DEBUG_SERIAL.begin(DEBUG_BAUDRATE);

  DXL_SERIAL.begin(DXL_BAUDRATE, SERIAL_HALF_DUPLEX);
  dxl.attach(DXL_SERIAL, DXL_BAUDRATE);
  
  for (auto &id: motor_ids) {
    dxl.addModel<DxlModel::X>(id);
  }
  delay(2000);

  for (auto &id: motor_ids) {
    if (!dxl.ping(id)) {
      DEBUG_SERIAL.println("FUCK!!!!!!");
    }
    dxl.torqueEnable(id, false);
  }

  //DEBUG_SERIAL.printf("ERROR: %d\n", dxl.hardwareErrorStatus(3));
  dxl.positionPGain(3, 5000);
  dxl.positionDGain(3, 5000);
  dxl.positionIGain(3, 400);
  dxl.profileVelocity(3, 10000);
  dxl.profileAcceleration(3, 10000);
  // dxl.profileVelocity(TARGET_ID, 500);
  // dxl.profileAcceleration(TARGET_ID, 500);

  // for (auto &id: motor_ids) {
  //   dxl.operatingMode(id, 4);
  //   //dxl.minPositionLimit(id, 0);
  //   //dxl.maxPositionLimit(id, 4095);
  // }
  // DEBUG_SERIAL.print("profile velocity = ");
  // DEBUG_SERIAL.println(dxl.profileVelocity(TARGET_ID));

  // DEBUG_SERIAL.print("profile acc = ");
  // DEBUG_SERIAL.println(dxl.profileAcceleration(TARGET_ID));

  // DEBUG_SERIAL.print("min pos = ");
  // DEBUG_SERIAL.println(dxl.minPositionLimit(TARGET_ID));
  // DEBUG_SERIAL.print("max pos = ");
  // DEBUG_SERIAL.println(dxl.maxPositionLimit(TARGET_ID));

  // DEBUG_SERIAL.print("position P gain = ");
  // DEBUG_SERIAL.println(dxl.positionPGain(TARGET_ID));
  // DEBUG_SERIAL.print("position I gain = ");
  // DEBUG_SERIAL.println(dxl.positionIGain(TARGET_ID));
  // DEBUG_SERIAL.print("positioj D gain = ");
  // DEBUG_SERIAL.println(dxl.positionDGain(TARGET_ID));

  for (auto &id: motor_ids) {
    dxl.torqueEnable(id, true);
  }
  delay(3000);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(6, LOW);
  read_goal_pos();

  if (initialized) {
    for (uint8_t i=0; i<4; i++) {
      if (!dxl.moving(motor_ids.at(i))) {
        DEBUG_SERIAL.printf("motor %d goal: %d\n", motor_ids.at(i), motor_goals.at(i));
        dxl.goalPosition(motor_ids.at(i), motor_goals.at(i));
        dxl.verbose(motor_ids.at(i));
      }
    }
  }

  if (solenoid_on) {
    digitalWrite(SOLENOID_PIN, HIGH);
    delay(500);
    digitalWrite(SOLENOID_PIN, LOW);
    solenoid_on = false;
  }

  if (print_on) {
    DEBUG_SERIAL.printf("motor3: %d, motor6: %d, motor11: %d, motor22: %d\n", dxl.presentPosition(3), dxl.presentPosition(6), dxl.presentPosition(11), dxl.presentPosition(22));
    print_on = false;
  }
  // DEBUG_SERIAL.print("current pos = ");
  // DEBUG_SERIAL.println(dxl.presentPosition(TARGET_ID));
  // dxl.verbose(TARGET_ID);
  delay(500);
}
#endif

#endif