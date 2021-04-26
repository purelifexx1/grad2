#include "vision.h"
#include "ui_vision.h"
#include "detect.h"
#include "calib.h"
#include "define_parameter.h"



Vision::Vision(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Vision)
{
    ui->setupUi(this);
    VideoCapture = new detect(this);
    CalibFrame = new Calib(this);
    connect(VideoCapture, &detect::newPixmapCaptured, this, [&]()
    {
        ui->CameraFrame->setPixmap(VideoCapture->pixmap().scaled(640,480));
        CalibFrame -> buffer.clear();
        CalibFrame -> buffer = VideoCapture ->buffer ;
        CalibFrame -> Set = true;
    });
    connect(CalibFrame, &Calib::newPixmapCaptured, this, [&]()
    {
        //CalibFrame->Trans_buffer
//        for (size_t i = 0; i < CalibFrame -> Trans_buffer.size(); ++i)
//        {
//            qDebug()<<"Toa do da gui cua vat"  << ": X = "<< CalibFrame-> Trans_buffer[i][1]<< " Y = "<< CalibFrame-> Trans_buffer[i][2] <<endl;
//        }
        if( enable == true)
        {
            for (size_t i = 0; i < CalibFrame -> Send_buffer.size(); ++i)
            {

                count_object ++;
                qDebug()<<"Toa do da gui cua vat" << count_object << ": X = "<< CalibFrame-> Send_buffer[i][1]<< " Y = "<< CalibFrame-> Send_buffer[i][2]<<
                            "ID ="<< (ObjectType)CalibFrame->Send_buffer[i][0] << "Theta = " << CalibFrame->Send_buffer[i][4] <<endl;
                if (ui->checkBox_2->isChecked()== true)
                {
                    send_packet(CalibFrame->Send_buffer[i][1], CalibFrame->Send_buffer[i][2], CalibFrame->Send_buffer[i][4],
                                                (ObjectType)CalibFrame->Send_buffer[i][0] ,mSerial);

                }

//                if(CalibFrame-> Send_buffer[i][2] < Y_LEVEL && CalibFrame-> Send_buffer[i][5]==0)
//                {
//                    count_object ++;
//                    qDebug()<<"Toa do da gui cua vat" << count_object << ": X = "<< CalibFrame-> Send_buffer[i][1]<< " Y = "<< CalibFrame-> Send_buffer[i][2] <<endl;
//                    //qDebug()<<" X1 = "<< VideoCapture ->buffer[i][1]<< " Y1 = "<< VideoCapture ->buffer[i][2] <<endl;
//                    //qDebug()<<" X2 = "<< CalibFrame ->buffer[i][1]<< " Y2 = "<< CalibFrame ->buffer[i][2] <<endl;
//                    CalibFrame->Send_buffer[i][5] = 2; // had sent = true
//                    if (ui->checkBox_2->isChecked()== true)
//                    {
//                        send_packet(CalibFrame->Send_buffer[i][1], CalibFrame->Send_buffer[i][2], 0, mSerial);
//                    }

//                }

            }
        }
        CalibFrame-> Send_buffer.clear();



    });
}

Vision::~Vision()
{
    delete ui;
}

void Vision::send_packet(double x, double y, double roll, ObjectType flag_type, QSerialPort* mSerial)
{
    QByteArray command;
    command.append(0x28);
    command.append("00");
    command.append(0x01);
    command.append(24);
    ADD_VALUE(&command, x, SCARA_COR_VALUE_DOUBLE);
    ADD_VALUE(&command, y, SCARA_COR_VALUE_DOUBLE);
    ADD_VALUE(&command, roll, SCARA_COR_VALUE_DOUBLE);
    command.append(flag_type);
    command.append("})");
    PACKET_DEFINE_LENGTH(command);
    mSerial->write(command, command.length());
}

void Vision::on_CameraOn_Button_clicked()
{
    CalibFrame->start(QThread::HighPriority);

    VideoCapture->start(QThread::HighPriority);
}

void Vision::on_CameraOff_Button_clicked()
{
    VideoCapture->terminate();
    CalibFrame -> terminate();
}



void Vision::on_checkBox_stateChanged(int arg1)
{
    if(ui->checkBox->isChecked() == true){
        enable = true;
    }else{
        enable = false;
    }
}


void Vision::on_ResetPoint_clicked()
{
    CalibFrame->current_point = 0;
}
