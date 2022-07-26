#include "func.h"
#include "dxl.h"
#include "utils.h"

using namespace std;

vector<string> command_set = {"pos", "aim", "l", "w"};
vector<int16_t> command_idx;
vector<string> command_vec;
string _substring;

string read_until_delimiter(char delimiter);
void process_command(string serialRead, char delimiter, vector<int16_t> idx, vector<string> cmd_vec);

void processBTSerial(String serialRead, char delimiter) {        
    Serial.println(serialRead);
    string serRead = string(serialRead.c_str());    

    command_idx.clear();
    command_vec.clear();
    int idx = 0;        

    for (string cmd : command_set){
        while ((idx = serRead.find(cmd)) >= 0){            
            DEBUG_SERIAL.println("Inside Loop");
            command_idx.push_back(idx);
            command_vec.push_back(cmd);
            break;
        }
    }    
    process_command(serRead, delimiter, command_idx, command_vec);
    _substring = nullptr;
}
void process_command(string serRead, char delimiter, vector<int16_t> idx, vector<string> cmd_vec){    
    for (uint8_t i = 0; i < idx.size(); i++){        
        if (idx.size() == 1){
            _substring = serRead;            
        }
        else {
            _substring = serRead.substr(idx[i], idx[i+1]);
        }        
        DEBUG_SERIAL.println("---------------");        
        read_until_delimiter(delimiter);        
        uint8_t id = atoi(read_until_delimiter(delimiter).c_str());        
        DEBUG_SERIAL.println(cmd_vec[i].c_str());
        DEBUG_SERIAL.println(id);
        
        if (cmd_vec[i].compare("pos") == 0){                      
            uint16_t pos = atoi(read_until_delimiter(delimiter).c_str());            
            DEBUG_SERIAL.println(pos);
            dxl.goalPosition(id, pos);                
        }
        else if (cmd_vec[i].compare("aim") == 0){            
            float longitude = atof(read_until_delimiter(delimiter).c_str());            
            float latitude = atof(read_until_delimiter(delimiter).c_str());
            uint16_t arm_joint_pos = round(2048 + 1.25 * longitude * DXL_DEGREE2POS);
            uint16_t hand_joint_pos = round(2048 + latitude * DXL_DEGREE2POS);
            DEBUG_SERIAL.printf("%f, %f, %d, %d", longitude, latitude, arm_joint_pos, hand_joint_pos);
            dxl.goalPosition(arm_joint_ids[id], arm_joint_pos);
            dxl.goalPosition(hand_joint_ids[id], hand_joint_pos);
        }
        else if (cmd_vec[i].compare("l") == 0){            
            digitalWrite(trigger_pins[id], HIGH);
            delay(100);  
            digitalWrite(trigger_pins[id], LOW);
        }
        else if (cmd_vec[i].compare("w") == 0){
            dxl.torqueEnable(rotator_ids[id], true);
            dxl.goalPosition(rotator_ids[id], ROTATOR_MIN_POS);
            delay(500);
            dxl.torqueEnable(pulley_ids[id], true);
            delay(10);
            dxl.goalPwm(pulley_ids[id], -885);
            delay(8000);
            dxl.goalPwm(pulley_ids[id], 0);
            dxl.torqueEnable(pulley_ids[id], false);
            dxl.goalPosition(rotator_ids[id], ROTATOR_MAX_POS);
        }        
    }
}
string read_until_delimiter(char delimiter){    
    string result;            
    char buffer[120];
    //DEBUG_SERIAL.print("SUBSTRING IS: ");
    //DEBUG_SERIAL.println(String(_substring.c_str()));
    int idx = _substring.find(delimiter);
    //DEBUG_SERIAL.printf("RU_IDX: %d\n", idx);
    if (idx == -1) {
        result = _substring;
        _substring = nullptr;
    } 
    else {                
        _substring.copy(buffer, idx, 0);
        result = string(buffer);           
        _substring = (_substring.substr(idx + 1));
    }
    // DEBUG_SERIAL.println(String(result.c_str()));
    // DEBUG_SERIAL.println(String(_substring.c_str()));
    return result;
}