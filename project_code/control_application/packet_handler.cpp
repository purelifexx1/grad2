#include "packet_handler.h"

packet_handler::packet_handler()
{

}

void packet_handler::categorize(QByteArray packet)
{

    int packet_length = packet.length();
    if(packet.at(0) == 0x28 && packet.at(packet_length-1) == 0x29){              
        //full packet receive
        archive_buffer.clear();
        packet_extract(packet);
        //routing(packet.mid(1, packet_length - 2));
    }else if(packet.at(0) == 0x28 && packet.at(packet_length-1) != 0x29){
        //packet to long, receive first half
        archive_buffer.append(packet);
        archive_status = true;
    }else if(packet.at(0) != 0x28 && packet.at(packet_length-1) == 0x29){
        //receive rest
        if(archive_status == true){
            archive_status = false;
            archive_buffer.append(packet);
            packet_extract(archive_buffer);
            //routing(archive_buffer.mid(1, archive_buffer.length() - 2));
            archive_buffer.clear();
        }else{
            //error packet format
        }
    }else{
        //error packet format
    }
}

void packet_handler::packet_extract(QByteArray packet)
{
    QList<QByteArray> processed_packet_list;
    QList<QByteArray> raw_packet_list = packet.split(')');
    int packet_ptr = -1;
    //using traditional for loop is much faster than foreach loop when applying for list object
    for(int i = 0; i < raw_packet_list.count() - 1; i++){ //detach the last empty component
        if(raw_packet_list.at(i).at(0) == 0x28){
            processed_packet_list.append(raw_packet_list.at(i));
            packet_ptr++;
        }else{
            processed_packet_list[packet_ptr].append(raw_packet_list.at(i));
        }
    }
    //action for each packet
    number_of_packet = processed_packet_list.count();
    for(int i = 0; i < processed_packet_list.count(); i++){
        routing(processed_packet_list[i].mid(1, processed_packet_list[i].length()-1));
    }

}

void packet_handler::routing(QByteArray packet)
{
    int packet_length = packet.length();
    switch (packet.at(0)){ // Transmission protocol
        case FILE_TRANSMISION:

        break;
        case COMMAND_TRANSMISION:
            // error format
        break;
        case RESPONSE_TRANSMISION:
            switch (packet.at(1)) { // RPD type
                case RPD_IDLE:

                break;
                case RPD_BUSY:

                break;
                case RPD_POSITION:
                    if(packet.at(2) == CMD_READ_POSITION){ // check if CMD_ID is correct
                        Scara_position_received(packet.mid(3, packet_length-3));
                    }else{
                        // incorrect packet format
                    }
                break;
                case RPD_START:
                case RPD_RUNNING:
                case RPD_DONE:
                case RPD_OK:
                case RPD_DUTY:
                case RPD_STOP:
                case RPD_ERROR:
                    Detail_Statusr_Handler(packet.mid(1, packet_length-1));
                break;

                case NUM_OF_RESPOND:

                break;
            }
        break;
    }
}

void packet_handler::Scara_position_received(QByteArray data)
{
    Scara_Position_RawData *RawData = reinterpret_cast<Scara_Position_RawData*>(data.data());
    Display_packet display_packet;
    display_packet.RealData.theta1 = (double)(RawData->raw_theta1*SCARA_INVERSE_SCALE);
    display_packet.RealData.theta2 = (double)(RawData->raw_theta2*SCARA_INVERSE_SCALE);
    display_packet.RealData.theta4 = (double)(RawData->raw_theta4*SCARA_INVERSE_SCALE);
    display_packet.RealData.D3 = (double)(RawData->raw_D3*SCARA_INVERSE_SCALE);
    display_packet.RealData.x = (double)(RawData->raw_x*SCARA_INVERSE_SCALE);
    display_packet.RealData.y = (double)(RawData->raw_y*SCARA_INVERSE_SCALE);
    display_packet.RealData.z = (double)(RawData->raw_z*SCARA_INVERSE_SCALE);
    display_packet.RealData.roll = (double)(RawData->raw_roll*SCARA_INVERSE_SCALE);
    display_packet.action_id = DISPLAY_POSITION;
    emit on_display_event(display_packet);
}

void packet_handler::Detail_Statusr_Handler(QByteArray data)
{
    Display_packet display_packet;
    display_packet.Respond_Type = (Robot_RespondTypedef)data.at(0);
    display_packet.action_id = DISPLAY_RPD_DETAIL;
    display_packet.Command_ID = (Robot_CommandTypedef)data.at(1);
    int packet_length = data.length();
    for(int i = 2; i < packet_length; i++){
        display_packet.Reference_String.append((Response_ID)data.at(i));
    }
    emit on_display_event(display_packet);
}
