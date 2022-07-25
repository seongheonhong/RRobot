#include "func.h"
#include "dxl.h"
#include "utils.h"

using namespace std;

vector<String> command_set = {"pos"};
vector<uint8_t> command_idx;
vector<String> command_vec;

void processBTSerial(String serialRead, char delimiter) {        
    command_idx.clear();
    command_vec.clear();
    int idx = 0;    

    for (String cmd : command_set){
        while ((idx = serialRead.indexOf(cmd)) >= 0){
            command_idx.push_back(idx);
            command_vec.push_back(cmd);
            break;
        }
    }
    process_command();
}
void process_command(){
    // if (){f
    //     uint8_t id = atoi((*(it + 1)).c_str());
    //     uint16_t pos = atoi((*(it + 2)).c_str());            
    //     dxl.goalPosition(id, pos);                
    // }            
    // else if (strcmp(comstr, "aim")){                
    //     uint8_t id = atoi((*(it + 1)).c_str());                  
    //     float longitude = atof((*(it + 2)).c_str());
    //     float latitude = atof((*(it + 3)).c_str());
    //     uint16_t arm_joint_pos = round(2048 + 1.25 * latitude * DXL_DEGREE2POS);
    //     uint16_t hand_joint_pos = round(2048 + longitude * DXL_DEGREE2POS);
    //     dxl.goalPosition(arm_joint_ids[id], arm_joint_pos);
    //     dxl.goalPosition(hand_joint_ids[id], hand_joint_pos);
    // }
    // else if (strcmp(comstr, "l")){
    //     uint8_t id = atoi((*(it + 1)).c_str());
    //     digitalWrite(trigger_pins[id], HIGH);
    //     delay(100);
    //     digitalWrite(trigger_pins[id], LOW);
    // }
    
}