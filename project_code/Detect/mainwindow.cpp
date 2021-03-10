#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "detect.h"
#include "calib.h"

#define Y_LEVEL 150.7
#define LOOP_SPACING 3
int numof_current_point = 0, numof_detected_point = 0;
int loop_count = 0;
std::vector<bool> has_sent;

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
        CalibFrame -> buffer = VideoCapture ->buffer ;
        CalibFrame -> Set = 1;
    });
    connect(CalibFrame, &Calib::newPixmapCaptured, this, [&]()
    {
        //CalibFrame->Trans_buffer

        if(loop_count++ > LOOP_SPACING && enable == true){
            numof_detected_point = CalibFrame->Trans_buffer.size();
            loop_count = 0;
            if(numof_detected_point > numof_current_point){
                numof_current_point++;
                //for(int i = 0; i < CalibFrame->Trans_buffer.size(); i++)
                has_sent.push_back(false);
            }else if(numof_detected_point == numof_current_point){
                std::vector<double> temper_vector;
                for(int i = 0; i < CalibFrame->Trans_buffer.size(); i++)
                    temper_vector.push_back(CalibFrame->Trans_buffer[i][2]);
                std::sort(temper_vector.begin(), temper_vector.end(), [](double x, double y){return x < y;});
                for(int i = 0; i < temper_vector.size(); i++){
                    if(temper_vector[i] < Y_LEVEL && has_sent[i] == false) {
                        send_packet(CalibFrame->Trans_buffer[i][1], CalibFrame->Trans_buffer[i][2], 0, mSerial);
                        has_sent[i] = true;
                    }
                }
            }else if(numof_detected_point < numof_current_point){
                numof_current_point--;
                has_sent.erase(has_sent.begin());
            }
        }

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
