#ifndef VISION_H
#define VISION_H

#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QMainWindow>
#include <receivethread.h>
#include "define_parameter.h"

QT_BEGIN_NAMESPACE
namespace Ui {class Vision;}
QT_END_NAMESPACE

class Calib;
class detect;
class Vision : public QMainWindow
{
    Q_OBJECT

public:
    explicit Vision(QWidget *parent = nullptr);
    ~Vision();
    size_t numof_current_point, numof_detected_point = 0;
    std::vector<bool> has_sent;

private slots:
    void on_CameraOn_Button_clicked();

    void on_CameraOff_Button_clicked();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_checkBox_stateChanged(int arg1);


private:
    Ui::Vision *ui;
    detect *VideoCapture;
    Calib  *CalibFrame;
    bool enable=false;
    int notFoundCount = 0;
    QByteArray command;
    int count_object;
    void send_packet(double x, double y, double roll, ObjectType flag_type, QSerialPort* mSerial);
};

#endif // VISION_H


