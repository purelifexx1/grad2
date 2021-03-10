#ifndef OBJECT_DETECT_HANDLER_H
#define OBJECT_DETECT_HANDLER_H

#include <QMainWindow>
#include <QObject>
#include <QWidget>
#include <QtSerialPort/QSerialPort>
class object_detect_handler:public QMainWindow
{
    Q_OBJECT
public:
    object_detect_handler();
    void looping();
    void send_packet(double x, double y, double roll, QSerialPort* mSerial);
};

#endif // OBJECT_DETECT_HANDLER_H
