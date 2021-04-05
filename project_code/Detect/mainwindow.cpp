#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "detect.h"
#include "calib.h"

#define Y_LEVEL 80 //90.7
#define LOOP_SPACING 1

int loop_count = 0;


QSerialPort *mSerial;
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
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
                qDebug()<<"Toa do da gui cua vat" << count_object << ": X = "<< CalibFrame-> Send_buffer[i][1]<< " Y = "<< CalibFrame-> Send_buffer[i][2] <<endl;
                if (ui->checkBox_2->isChecked()== true)
                {
                    send_packet(CalibFrame->Send_buffer[i][1], CalibFrame->Send_buffer[i][2], 0, mSerial);
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

MainWindow::~MainWindow()
{
    delete ui;
    VideoCapture -> terminate();
    CalibFrame -> terminate();
}

void MainWindow::send_packet(double x, double y, double roll, QSerialPort* mSerial)
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
void MainWindow::on_CameraOn_Button_clicked()
{
    CalibFrame->start(QThread::HighPriority);
    VideoCapture->start(QThread::HighPriority);
}

void MainWindow::on_CameraOff_Button_clicked()
{
    VideoCapture->terminate();
    CalibFrame -> terminate();
}

void MainWindow::on_pushButton_clicked()
{
    ui->comboBox->clear();
        const auto list_of_port = QSerialPortInfo::availablePorts();
            for (const QSerialPortInfo &port : list_of_port)
                ui->comboBox->addItem(port.portName());
}

void MainWindow::on_pushButton_2_clicked()
{
    if (ui->pushButton_2->text() == "Connect"){
            ui->pushButton_2->setText("Disconnect");
            ui->pushButton_2->setStyleSheet("background-color:green");
            mSerial = new QSerialPort(this);
            mSerial->setPortName(ui->comboBox->currentText());
            mSerial->setBaudRate(QSerialPort::Baud115200);
            mSerial->setDataBits(QSerialPort::Data8);
            mSerial->setParity(QSerialPort::NoParity);
            mSerial->setStopBits(QSerialPort::OneStop);
            mSerial->setFlowControl(QSerialPort::NoFlowControl);
            mSerial->open(QIODevice::ReadWrite);
//            Received_Thread = new ReceiveThread(this);
//            Received_Thread->set_serial_object(mSerial);
//            connect(Received_Thread, SIGNAL(packet_received(QByteArray)), this, SLOT(received_callback(QByteArray)));
//            Received_Thread->start();

        }else if(ui->pushButton_2->text() == "Disconnect"){
            ui->pushButton_2->setText("Connect");
            ui->pushButton_2->setStyleSheet("background-color:red");
            ui->checkBox->setChecked(false);
            //Received_Thread->stop = true;
            mSerial->close();
//            delete mSerial;
//            delete Received_Thread;
        }
}

void MainWindow::on_checkBox_stateChanged(int arg1)
{
    if(ui->checkBox->isChecked() == true){
        enable = true;
    }else{
        enable = false;
    }
}
