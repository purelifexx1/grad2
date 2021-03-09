#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <qstring.h>
#include <QDebug>
#include <QtSerialPort/QtSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QFileDialog>
#include <QFile>

QSerialPort *mSerial;
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->bt_connect->setStyleSheet("background-color:red");
    ui->bt_connect->setText("Connect");
    connect(_packet_handler, SIGNAL(on_display_event(Display_packet)), this, SLOT(display_event(Display_packet)));
    QString temp = "bt_inc_";
    for(int i = 1; i <= 8; i++){
        QPushButton *object_bt = this->findChild<QPushButton*>(temp + QString::number(i));
        connect(object_bt, SIGNAL(clicked()), this, SLOT(on_bt_keycommand()));
    }
    connect(ui->bt_start_test, SIGNAL(clicked()), this, SLOT(on_bt_testmt()));
    connect(ui->bt_stop_test, SIGNAL(clicked()), this, SLOT(on_bt_testmt()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::display_event(Display_packet data)
{
    switch (data.action_id) {
        case DISPLAY_POSITION:
            ui->tb_x_cur_cor->setText(QString::number(data.RealData.x));
            ui->tb_y_cur_cor->setText(QString::number(data.RealData.y));
            ui->tb_z_cur_cor->setText(QString::number(data.RealData.z));
            ui->tb_roll_cur_cor->setText(QString::number(data.RealData.roll*180.0f/3.141592f));
            ui->tb_theta1_cur_val->setText(QString::number(data.RealData.theta1));
            ui->tb_theta2_cur_val->setText(QString::number(data.RealData.theta2));
            ui->tb_theta4_cur_val->setText(QString::number(data.RealData.theta4));
            ui->tb_d3_cur_val->setText(QString::number(data.RealData.D3));
            log_console("Position data received");
            log_console("--------------------------");
        break;
        case DISPLAY_RPD_DETAIL:
            if(_packet_handler->number_of_packet != 0){
                _packet_handler->number_of_packet--;
            }
            QString detail_string;
            for(int t = 0; t < data.Reference_String.length(); t++){
                switch (data.Reference_String.at(t)) {
                case MANUAL_SPEED:{
                    detail_string += system_parameter->DETAIL_STATUS[data.Reference_String.at(t)] +
                            ": " + QString::number(data.Reference_String.at(t+1)) + "; ";
                    t++;
                }
                break;
                default:{
                    detail_string += system_parameter->DETAIL_STATUS[data.Reference_String.at(t)] + "; ";
                }
                break;
                }
            }
            log_console(system_parameter->RDP_String[data.Respond_Type]
                    + " | Command ID: " + system_parameter->COMMAND_STRING[data.Command_ID]
                    + " | Detail: " + detail_string);
            if(_packet_handler->number_of_packet == 0){
                log_console("--------------------------");
            }
        break;
    }

}

void MainWindow::on_bt_refresh_clicked()
{
    ui->com_list->clear();
    const auto list_of_port = QSerialPortInfo::availablePorts();
        for (const QSerialPortInfo &port : list_of_port)
            ui->com_list->addItem(port.portName());
}

void MainWindow::on_bt_connect_clicked()
{
    if (ui->bt_connect->text() == "Connect"){
        ui->bt_connect->setText("Disconnect");
        ui->bt_connect->setStyleSheet("background-color:green");
        mSerial = new QSerialPort(this);
        mSerial->setPortName(ui->com_list->currentText());
        mSerial->setBaudRate(QSerialPort::Baud115200);
        mSerial->setDataBits(QSerialPort::Data8);
        mSerial->setParity(QSerialPort::NoParity);
        mSerial->setStopBits(QSerialPort::OneStop);
        mSerial->setFlowControl(QSerialPort::NoFlowControl);
        mSerial->open(QIODevice::ReadWrite);
        Received_Thread = new ReceiveThread(this);
        Received_Thread->set_serial_object(mSerial);
        connect(Received_Thread, SIGNAL(packet_received(QByteArray)), this, SLOT(received_callback(QByteArray)));
        Received_Thread->start();

    }else if(ui->bt_connect->text() == "Disconnect"){
        ui->bt_connect->setText("Connect");
        ui->bt_connect->setStyleSheet("background-color:red");
        Received_Thread->stop = true;
        mSerial->close();
        delete mSerial;
        delete Received_Thread;
    }
}

void MainWindow::received_callback(QByteArray data)
{
    qDebug() << data;
    _packet_handler->categorize(data);
}

void MainWindow::on_bt_robot_stop_clicked()
{
    QByteArray command;
    command.append(START_CHAR);
    command.append('\0');
    command.append(COMMAND_TRANSMISION);
    command.append(CMD_STOPNOW);
    command.append(RECEIVE_END);
    command[1] = command.length() - 2;
    mSerial->write(command, command.length());
}


void MainWindow::on_bt_scan_limit_clicked()
{
    QByteArray command;
    command.append(START_CHAR);
    command.append('\0');
    command.append(COMMAND_TRANSMISION);
    command.append(CMD_SCAN_LIMIT);
    command.append(RECEIVE_END);
    command[1] = command.length() - 2;
    mSerial->write(command, command.length());
}

void MainWindow::on_bt_home_clicked()
{
    QByteArray command;
    command.append(START_CHAR);
    command.append('\0');
    command.append(COMMAND_TRANSMISION);
    command.append(CMD_MOVE_LINE);
    ADD_VALUE(&command, 200.0, SCARA_COR_VALUE_TEXT);
    ADD_VALUE(&command, -75.0, SCARA_COR_VALUE_DOUBLE);
    ADD_VALUE(&command, 133.291, SCARA_COR_VALUE_DOUBLE);
    ADD_VALUE(&command, 0.0, SCARA_COR_VALUE_DOUBLE);
    ADD_VALUE(&command, 0.0, SCARA_COR_VALUE_DOUBLE);
    command.append(DUTY_MODE_INIT_QVT);
    ADD_VALUE(&command, 2.0, SCARA_COR_VALUE_DOUBLE);
    command.append(DUTY_COORDINATES_ABS);
    command.append(DUTY_TRAJECTORY_LSPB);
    command.append(RECEIVE_END);
    command[1] = command.length() - 2;
    mSerial->write(command, command.length());
}

void MainWindow::on_bt_movL_clicked()
{
    QByteArray command;
    command.append(START_CHAR);
    command.append('\0');
    command.append(COMMAND_TRANSMISION);
    command.append(CMD_MOVE_LINE);
    ADD_VALUE(&command, ui->tb_x_cor->text(), SCARA_COR_VALUE_TEXT);
    ADD_VALUE(&command, ui->tb_y_cor->text(), SCARA_COR_VALUE_TEXT);
    ADD_VALUE(&command, ui->tb_z_cor->text(), SCARA_COR_VALUE_TEXT);
    ADD_VALUE(&command, ui->tb_roll_ang->text(), SCARA_COR_VALUE_TEXT);
    if(ui->tb_v_factor->text().toDouble() > 1 || ui->tb_v_factor->text().toDouble() < 0){
        log_console("Invalid for velocity factor");
        return;
    }else{
        ADD_VALUE(&command, ui->tb_v_factor->text(), SCARA_COR_VALUE_TEXT);
    }

    if(ui->rb_qva->isChecked() == true) {
        command.append(DUTY_MODE_INIT_QVA);
        ADD_VALUE(&command, ui->tb_a_factor->text(), SCARA_COR_VALUE_TEXT);
    }else if(ui->rb_qvt->isChecked() == true){
        command.append(DUTY_MODE_INIT_QVT);
        ADD_VALUE(&command, ui->tb_time->text(), SCARA_COR_VALUE_TEXT);
    }else if(ui->rb_qt->isChecked() == true){
        command.append(DUTY_MODE_INIT_QT);
        ADD_VALUE(&command, ui->tb_time->text(), SCARA_COR_VALUE_TEXT);
    }
    if(ui->rb_abs->isChecked() == true){
       command.append(DUTY_COORDINATES_ABS);
    }else if(ui->rb_inc->isChecked() == true){
       command.append(DUTY_COORDINATES_REL);
    }
    if(ui->rb_lspb->isChecked() == true){
       command.append(DUTY_TRAJECTORY_LSPB);
    }else if(ui->rb_scur->isChecked() == true){
       command.append(DUTY_TRAJECTORY_SCURVE);
    }else if(ui->rb_linear->isChecked() == true){
        command.append(DUTY_TRAJECTORY_LINEAR);
    }
    command.append(RECEIVE_END);
    command[1] = command.length() - 2;
    mSerial->write(command, command.length());
}

void MainWindow::on_bt_model_setting_clicked()
{
    QByteArray command;
    command.append(START_CHAR);
    command.append('\0');
    command.append(COMMAND_TRANSMISION);
    command.append(CMD_SETTING);
    if(ui->rb_abs->isChecked() == true){
        command.append(DUTY_COORDINATES_ABS);
    }else if(ui->rb_inc->isChecked() == false){
        command.append(DUTY_COORDINATES_REL);
    }

    if(ui->rb_lspb->isChecked() == true){
        command.append(DUTY_TRAJECTORY_LSPB);
    }else if(ui->rb_scur->isChecked() == false){
        command.append(DUTY_TRAJECTORY_SCURVE);
    }
    command.append(RECEIVE_END);
    command[1] = command.length() - 2;
    mSerial->write(command, command.length());
}

void MainWindow::on_bt_on_magnet_clicked()
{
    QByteArray command;
    command.append(START_CHAR);
    command.append('\0');
    command.append(COMMAND_TRANSMISION);
    command.append(CMD_OUTPUT);
    command.append('\1');
    command.append(RECEIVE_END);
    command[1] = command.length() - 2;
    mSerial->write(command, command.length());
}

void MainWindow::on_bt_off_magnet_clicked()
{
    QByteArray command;
    command.append(START_CHAR);
    command.append('\0');
    command.append(COMMAND_TRANSMISION);
    command.append(CMD_OUTPUT);
    command.append('\0');
    command.append(RECEIVE_END);
    command[1] = command.length() - 2;
    mSerial->write(command, command.length());
}

void MainWindow::on_bt_read_position_clicked()
{
    QByteArray command;
    command.append(START_CHAR);
    command.append('\0');
    command.append(COMMAND_TRANSMISION);
    command.append(CMD_READ_POSITION);
    if(ui->rb_real_type->isChecked() == true){
        if(ui->cb_update_true_pos->isChecked() == true){
            command.append(REAL_POSITION_DATA_PLUS_UPDATE);
        }else{
            command.append(REAL_POSITION_DATA);
        }
    }else if(ui->rb_estimate_type->isChecked() == true){
        command.append(ESTIMATE_POSITION_DATA);
    }
    command.append(RECEIVE_END);
    command[1] = command.length() - 2;
    mSerial->write(command, command.length());
}

void MainWindow::on_bt_keycommand()
{
    QByteArray command;
    command.append(START_CHAR);
    command.append('\0');
    command.append(COMMAND_TRANSMISION);
    command.append(CMD_KEYBOARD);
    QPushButton *obj_sender = (QPushButton*)sender();
    uint8_t selection = (uint8_t)obj_sender->objectName().split('_')[2].toInt() - 1;
    command.append(selection);
    command.append(RECEIVE_END);
    command[1] = command.length() - 2;
    mSerial->write(command, command.length());
}

void MainWindow::on_bt_key_setsp_clicked()
{
    QByteArray command;
    command.append(START_CHAR);
    command.append('\0');
    command.append(COMMAND_TRANSMISION);
    command.append(CMD_KEY_SPEED);
    uint8_t key_speed = (uint8_t)ui->tb_key_setsp->text().toInt();
    key_speed = (key_speed > SHIFT_KEY_MAX)?SHIFT_KEY_MAX:key_speed;
    key_speed = (key_speed < SHIFT_KEY_MIN)?SHIFT_KEY_MIN:key_speed;
    command.append(key_speed);
    command.append(RECEIVE_END);
    command[1] = command.length() - 2;
    mSerial->write(command, command.length());
}

void MainWindow::on_bt_set_method_clicked()
{
    QByteArray command;
    command.append(START_CHAR);
    command.append('\0');
    command.append(COMMAND_TRANSMISION);
    command.append(CMD_METHOD_CHANGE);
    if(ui->rb_manual->isChecked() == true){
        command.append(SCARA_METHOD_MANUAL);
    }else if(ui->rb_semi_auto->isChecked() == true){
        command.append(SCARA_METHOD_SEMI_AUTO);
    }else if(ui->rb_auto->isChecked() == true){
        command.append(SCARA_METHOD_GCODE);
    }else if(ui->rb_test->isChecked() == true){
        command.append(SCARA_METHOD_TEST);
    }else if(ui->rb_pick_and_place->isChecked() == true){
        command.append(SCARA_METHOD_PICK_AND_PLACE);
    }else{
        log_console("Please select method");
        log_console("--------------------------");
        return;
    }
    command.append(RECEIVE_END);
    command[1] = command.length() - 2;
    mSerial->write(command, command.length());
}

void MainWindow::on_bt_clear_console_clicked()
{
    ui->tb_console->clear();
}

void MainWindow::on_bt_testmt()
{
    QByteArray command;
    command.append(START_CHAR);
    command.append('\0');
    command.append(COMMAND_TRANSMISION);
    command.append(CMD_MOTOR_TEST);
    uint8_t sign = (ui->hs_testmt_sign->value() == 1)?1:0;
    uint8_t pos = (ui->rb_test_mt1->isChecked()==true)?0:
                  (ui->rb_test_mt2->isChecked()==true)?1:
                  (ui->rb_test_mt3->isChecked()==true)?2:
                  (ui->rb_test_mt4->isChecked()==true)?3:0;
    SCARA_TestMode test_mode = SCARA_TestMode(pos*2 + sign);   
    QPushButton *obj_sender = (QPushButton*)sender();
    if(obj_sender->objectName() == "bt_start_test"){
       command.append(test_mode);
    }else if(obj_sender->objectName() == "bt_stop_test"){
       command.append(SCARA_TEST_MOTOR_STOP);
    }
    command.append(RECEIVE_END);
    command[1] = command.length() -2;
    mSerial->write(command, command.length());
}

void MainWindow::on_testing_clicked()
{
//    QByteArray command;
//    command.append(START_CHAR);
//    command.append(COMMAND_TRANSMISION);
//    command.append(CMD_OBJECT_DETECTED);
//    ADD_VALUE(&command, "338.4816", SCARA_COR_VALUE);
//    ADD_VALUE(&command, "-3.7336", SCARA_COR_VALUE);
//    ADD_VALUE(&command, "0.0", SCARA_COR_VALUE);
//    command.append(VIETNAM_FLAG);
//    command.append(RECEIVE_END);
//    mSerial->write(command, command.length());
    Gcode_Decoder_DTC_TypeDef Gcode_DTC;
    gcode->Init_Current_Data(0, 0, 0, 0.5);
    QString file_directory = QFileDialog::getOpenFileName(this, "Open A File", "D:\\");
    QFile file(file_directory);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        log_console("Cannot open a file");
        return;
    }
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        Gcode_DTC = gcode->Process_Line(line);
        if(Gcode_DTC != GCODE_OK){
            log_console("Failed to read Gcode file. DTC code: " + QString::number(Gcode_DTC));
            return;
        }
    }
    file.close();

    Gcode_DTC = gcode->Process_Compress_Gcode_Data();
    if(Gcode_DTC == GCODE_OK){
        log_console("Gcode file compresses successfully");
    }else{
        log_console("Failed to compress Gcode file. DTC code: " + QString::number(Gcode_DTC));
        return;
    }
    Gcode_DTC = gcode->Write_Data_To_File("D:/gcode/file.gcode");
    if(Gcode_DTC == GCODE_OK){
        log_console("Gcode file writes successfully");
    }else{
        log_console("Failed to write Gcode file. DTC code: " + QString::number(Gcode_DTC));
    }
    gcode->package_data();
    QByteArray send_packet;
    for(int c = 0; c < gcode->data_packet.count(); c++){
        send_packet.append(gcode->data_packet.at(c));
    }
    mSerial->write(send_packet, send_packet.length());
    gcode->Clear_Data();
    qDebug()<<"end";
}


void MainWindow::on_bt_conveyor_sp_clicked()
{
    QByteArray command;
    command.append(START_CHAR);
    command.append('\0');
    command.append(COMMAND_TRANSMISION);
    command.append(CMD_SETUP_CONVEYOR_SPEED);
    ADD_VALUE(&command, ui->tb_conveyor_sp->text(), SCARA_COR_VALUE_TEXT);
    command.append(RECEIVE_END);
    command[1] = command.length() - 2;
    mSerial->write(command, command.length());
}
