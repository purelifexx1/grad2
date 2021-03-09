#define Y_LEVEL 78.8
#define LOOP_SPACING 3
#define OBTAIN_DATA

int loop_count = 0;
int numof_current_point = 0;
int numof_detected_point = 0;
typedef struct{
    std::vector<double> y_coor;
    std::vector<bool>   has_sent;
}Object_TypeDef;
Object_TypeDef Object;
void detect_loop_handler()
{
    
    if(loop_count++ > LOOP_SPACING){
        loop_count = 0;

        //numof_detected_point = ....
        if(numof_detected_point > numof_current_point){
            numof_current_point++;
            Object.y_coor.push_back(OBTAIN_DATA.y);
            Object.has_sent.push_back(false);
        }else if(numof_detected_point == numof_current_point){
            std::vector<double> temper_vector;
            for(int i = 0; i < OBTAIN_DATA.y.size(); i++){
                temper_vector.push_back(OBTAIN_DATA.y[i]);
            }
            std::sort()
        }else(numof_detected_point < numof_current_point){
            numof_current_point--;
            std::erase(Object)
        }
    }
}

void object_detected(double x, double y, double roll, QSerial* mSerial)
{
    QByteArray command;
    command.append(0x28);
    command.append('\0');
    command.append(0x01);
    command.append(24);
    int32_t number = (int32_t)(x * 1000000);
    command.append(reinterpret_cast<const char*>(&number), sizeof(number));
    number = (int32_t)(y * 1000000);
    command.append(reinterpret_cast<const char*>(&number), sizeof(number));
    number = (int32_t)(roll* 1000000);
    command.append(reinterpret_cast<const char*>(&number), sizeof(number));
    command.append('\0');
    command.append("})");
    command[1] = command.length() - 2;
    mSerial->write(command, command.length());
}