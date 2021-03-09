#include "gcode_decoder.h"

Gcode_Decoder::Gcode_Decoder()
{

}

void Gcode_Decoder::Clear_Data()
{
    raw_data.clear();
    compact_data.clear();
    data_packet.clear();
}
Gcode_Decoder_DTC_TypeDef Gcode_Decoder::Process_Line(QString Line)
{
    if(Line.startsWith("G0")){
        GCode_Coordinate_TypeDef temper_coordinate;
        QString temper_line = Line.split("(").at(0);
        if(temper_line.contains(')')){
            //Gcode error line format
            return GCODE_ERROR_LINE_FORMAT;
        }
        QStringList temper_string = temper_line.split(" "); // seperate the comment section that start with '('

        QString component = "";

        //categorize command
        for(int i = 0; i <= 3; i++){
            if(temper_string.at(0) == Command_String[i]){
                temper_coordinate.Command = (GCode_TypeDef)i;
                break;
            }
        }

        //find X coordinate
        int index = 0;
        index = temper_string.indexOf(QRegExp("^X.+"));
        if(index != -1) {
            component = temper_string.at(index);
            if(!component.isEmpty()){
                temper_coordinate.X = component.mid(1, component.length() - 1).toDouble();
            }
        }else{
            temper_coordinate.X = raw_data.last().X;
        }

        //find Y coordinate
        index = temper_string.indexOf(QRegExp("^Y.+"));
        if(index != -1) {
            component = temper_string.at(index);
            if(!component.isEmpty()){
                temper_coordinate.Y = component.mid(1, component.length() - 1).toDouble();
            }
        }else{
            temper_coordinate.Y = raw_data.last().Y;
        }

        //find Z coordinate
        index = temper_string.indexOf(QRegExp("^Z.+"));
        if(index != -1) {
            component = temper_string.at(index);
            if(!component.isEmpty()){
                temper_coordinate.Z = component.mid(1, component.length() - 1).toDouble();
            }
        }else{
            temper_coordinate.Z = raw_data.last().Z;
        }

        //find Feed rate
        index = temper_string.indexOf(QRegExp("^F.+"));
        if(index != -1) {
            component = temper_string.at(index);
            if(!component.isEmpty()){
                temper_coordinate.Feed = component.mid(1, component.length() - 1).toDouble();
            }
        }else{
            temper_coordinate.Feed = raw_data.last().Feed;
        }

        temper_coordinate.check[X_COR] = true;
        temper_coordinate.check[Y_COR] = true;
        temper_coordinate.check[Z_COR] = true;
        temper_coordinate.check[FEED] = true;

        //find I and J coordinate if the command is G02 or G03
        if(temper_coordinate.Command == G02 || temper_coordinate.Command == G03){
            index = temper_string.indexOf(QRegExp("^I.+"));
            if(index != -1) {
                component = temper_string.at(index);
                if(!component.isEmpty()){
                    temper_coordinate.check[I_COR] = true;
                    temper_coordinate.I = component.mid(1, component.length() - 1).toDouble();
                    lastI = temper_coordinate.I;
                }
            }else{
                temper_coordinate.I = lastI;
            }

            index = temper_string.indexOf(QRegExp("^J.+"));
            if(index != -1) {
                component = temper_string.at(index);
                if(!component.isEmpty()){
                    temper_coordinate.check[J_COR] = true;
                    temper_coordinate.J = component.mid(1, component.length() - 1).toDouble();
                    lastJ = temper_coordinate.J;
                }
            }else{
                temper_coordinate.J = lastJ;
            }
            temper_coordinate.check[I_COR] = true;
            temper_coordinate.check[J_COR] = true;
        }
        raw_data.append(temper_coordinate);
    }
    return GCODE_OK;
}

Gcode_Decoder_DTC_TypeDef Gcode_Decoder::Process_Compress_Gcode_Data()
{
    //check if current data has been init or not
    if(current_data.check[X_COR] == false || current_data.check[Y_COR] == false || current_data.check[Z_COR] == false || current_data.check[FEED] == false)
        return CURRENT_DATA_UNINIT;
    int count = 1;
    int check_count = -2;
    Gcode_Compress_State_TypeDef current_state = ROUTE_COMMAND;
    GCode_Coordinate_TypeDef  last_data = current_data;
    double current_s = 0;
    while(count < raw_data.count()){
        switch (current_state) {
        case ROUTE_COMMAND:{
            if(raw_data.at(count).Command == G02 || raw_data.at(count).Command == G03){
                current_state = STATE_CIRCLE;
            }else if(raw_data.at(count).Command == G00 || raw_data.at(count).Command == G01){
                current_state = STATE_LINEAR;
            }
        }
        break;
        case STATE_LINEAR:{
            current_s = calculate_linear_distance(current_data, raw_data.at(count));
            if(current_s / current_data.Feed > MINIMUM_LINEAR_TIME_FRAME){
                raw_data[count].Command = G01;
                compact_data.append(raw_data.at(count));
                current_data = raw_data.at(count);
                current_state = ROUTE_COMMAND;
            }else{
                current_state = ROUTE_COMMAND;
            }
            count++;
        }
        break;
        case STATE_CIRCLE:{
            current_s = calculate_circle_distance(raw_data.at(count-1), raw_data.at(count));
            if(current_s / current_data.Feed > MINIMUM_CIRCLE_TIME_FRAME){
                compact_data.removeLast();
                compact_data.append(raw_data.at(count-1));
                compact_data.append(raw_data.at(count));
                current_data = raw_data.at(count);
                count++;
                current_state = ROUTE_COMMAND;
            }else{
                current_state = STATE_LINEAR;
            }
        }
        break;
        }
        check_count++; //count value must end before check_count to ensure successful compression
        if(check_count > raw_data.count()*2) return GCODE_PROCESS_ERROR; //make sure the algorithm wont stuck in the loop
    }
    return GCODE_OK;
}

Gcode_Decoder_DTC_TypeDef Gcode_Decoder::package_data()
{

    QByteArray temper_array;
    double Min_Z, Max_Z;
    std::vector<double> Z_cor;
    for(int i = 0; i < compact_data.count(); i++){
        Z_cor.push_back(compact_data.at(i).Z);
    }
    std::sort(Z_cor.begin(), Z_cor.end(), [](double x, double y){ return (x<y);});
    Min_Z = Z_cor.at(0);
    Max_Z = *(Z_cor.end()-1);

    //init first packet
    temper_array.append(START_CHAR);
    temper_array.append('\0');
    temper_array.append(FILE_TRANSMISION);
    temper_array.append(FIRST_PACKET);
    ADD_VALUE(&temper_array, Min_Z, SCARA_COR_VALUE_DOUBLE);
    ADD_VALUE(&temper_array, Max_Z, SCARA_COR_VALUE_DOUBLE);
    ADD_VALUE(&temper_array, compact_data.count(), INT32_VALUE);
    temper_array.append(RECEIVE_END);
    temper_array[1] = temper_array.length() - 2;
    data_packet.append(temper_array);
    temper_array.clear();

    //init 10 point in a row to 1 packet
    int count = 0;
    Packet_Command_TypeDef tool_height;
    for(int i = 0; i < compact_data.count(); i++){
        if(count == 0){
            temper_array.append(START_CHAR);
            temper_array.append('\0');
            temper_array.append(FILE_TRANSMISION);
        }

        if(abs(compact_data.at(i).Z - Max_Z) < 0.001){
            tool_height = UP_Z;
        }else if(abs(compact_data.at(i).Z - Min_Z) < 0.001){
            tool_height = DOWN_Z;
        }else{
            return UNMATCH_Z_HEIGHT;
        }

        switch(compact_data.at(i).Command){
            case G00:
            case G01:{
                temper_array.append(tool_height << 4 | LINEAR_TYPE);
                ADD_VALUE(&temper_array, compact_data.at(i).X, SCARA_COR_VALUE_DOUBLE);
                ADD_VALUE(&temper_array, compact_data.at(i).Y, SCARA_COR_VALUE_DOUBLE);
                ADD_VALUE(&temper_array, compact_data.at(i).Feed, SCARA_COR_VALUE_DOUBLE);
            }
            break;
            case G02:{
                temper_array.append(tool_height << 4 | ARC_CW_TYPE);
                ADD_VALUE(&temper_array, compact_data.at(i).X, SCARA_COR_VALUE_DOUBLE);
                ADD_VALUE(&temper_array, compact_data.at(i).Y, SCARA_COR_VALUE_DOUBLE);
                ADD_VALUE(&temper_array, compact_data.at(i).Feed, SCARA_COR_VALUE_DOUBLE);
                ADD_VALUE(&temper_array, compact_data.at(i).I, SCARA_COR_VALUE_DOUBLE);
                ADD_VALUE(&temper_array, compact_data.at(i).J, SCARA_COR_VALUE_DOUBLE);
            }
            break;
            case G03:{
                temper_array.append(tool_height << 4 | ARC_AW_TYPE);
                ADD_VALUE(&temper_array, compact_data.at(i).X, SCARA_COR_VALUE_DOUBLE);
                ADD_VALUE(&temper_array, compact_data.at(i).Y, SCARA_COR_VALUE_DOUBLE);
                ADD_VALUE(&temper_array, compact_data.at(i).Feed, SCARA_COR_VALUE_DOUBLE);
                ADD_VALUE(&temper_array, compact_data.at(i).I, SCARA_COR_VALUE_DOUBLE);
                ADD_VALUE(&temper_array, compact_data.at(i).J, SCARA_COR_VALUE_DOUBLE);
            }
            break;
        }             

        if(count == 9){
            temper_array.append(RECEIVE_END);
            temper_array[1] = temper_array.length() - 2;
            data_packet.append(temper_array);
            temper_array.clear();
        }
        count = (count+1)%10;
    }
    if(count != 0){ //uneven element in 1 packet
        temper_array.append(RECEIVE_END);
        temper_array[1] = temper_array.length() - 2;
        data_packet.append(temper_array);
        temper_array.clear();
    }
    return GCODE_OK;

}
double Gcode_Decoder::calculate_circle_distance(GCode_Coordinate_TypeDef start, GCode_Coordinate_TypeDef end)
{
    double x_center, y_center, start_x1, start_y1, end_x1, end_y1;
    double angle_start, angle_end, radius, denta_angle;
    radius   = sqrt(end.I*end.I + end.J*end.J);
    x_center = start.X + end.I;
    y_center = start.Y + end.J;
    start_x1 = -end.I;
    start_y1 = -end.J;
    end_x1   = end.X - x_center;
    end_y1   = end.Y - y_center;
    angle_start = atan2(start_y1, start_x1);
    angle_end   = atan2(end_y1, end_x1);
    denta_angle = angle_end - angle_start;
    switch (end.Command) {
        case G02:
            if(denta_angle > 0){
                return (2*M_PI - abs(denta_angle))*radius;
            }else{
                return abs(denta_angle)*radius;
            }
        break;
        case G03:
            if(denta_angle > 0){
                return  abs(denta_angle)*radius;
            }else{
                return (2*M_PI - abs(denta_angle))*radius;
            }
        break;
    }
}

double Gcode_Decoder::calculate_linear_distance(GCode_Coordinate_TypeDef start, GCode_Coordinate_TypeDef end)
{
    //start coordinate must be init
    double denta_x, denta_y, denta_z;
    denta_x = end.X - start.X;
    denta_y = end.Y - start.Y;
    denta_z = end.Z - start.Z;
    return sqrt(denta_x*denta_x + denta_y*denta_y + denta_z*denta_z);
}

void Gcode_Decoder::Init_Current_Data(double x, double y, double z, double feed)
{
    current_data.X = x;
    current_data.Y = y;
    current_data.Z = z;
    current_data.Feed = feed;
    current_data.check[X_COR] = true;
    current_data.check[Y_COR] = true;
    current_data.check[Z_COR] = true;
    current_data.check[FEED] = true;
    raw_data.append(current_data);
}

Gcode_Decoder_DTC_TypeDef Gcode_Decoder::Write_Data_To_File(QString path)
{
    QString file_content = "";
    for(int i = 0; i < compact_data.count(); i++){
        switch (compact_data.at(i).Command) {
            case G00:
                file_content.append("G0" + QString::number(compact_data.at(i).Command) + " X" +
                               QString::number(compact_data.at(i).X) + " Y" + QString::number(compact_data.at(i).Y)
                               + " Z" + QString::number(compact_data.at(i).Z));
                file_content.append("\r\n");
            break;
            case G01:
                file_content.append("G0" + QString::number(compact_data.at(i).Command) + " X" +
                           QString::number(compact_data.at(i).X) + " Y" + QString::number(compact_data.at(i).Y)
                           + " Z" + QString::number(compact_data.at(i).Z) + " F" + QString::number(compact_data.at(i).Feed));
                file_content.append("\r\n");
            break;
            case G02:
            case G03:
                file_content.append("G0" + QString::number(compact_data.at(i).Command) + " X" +
                           QString::number(compact_data.at(i).X) + " Y" + QString::number(compact_data.at(i).Y)
                           + " Z" + QString::number(compact_data.at(i).Z) +
                               + " I" +QString::number(compact_data.at(i).I) +" J" +QString::number(compact_data.at(i).J) +
                               " F" + QString::number(compact_data.at(i).Feed));
                file_content.append("\r\n");
            break;
        }
    }
    QFile file(path);
    if(file.open(QIODevice::WriteOnly | QIODevice::Text)){
        file.write(file_content.toStdString().c_str(), file_content.length());
        file.close();
    }else{
        return ERROR_WRITE_FILE;
    }
    return GCODE_OK;
}
Gcode_Decoder *gcode = new Gcode_Decoder();
