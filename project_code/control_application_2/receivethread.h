#ifndef RECEIVETHREAD_H
#define RECEIVETHREAD_H

#include <QThread>
#include <QtCore>
#include <QtSerialPort/QSerialPort>
#define over_length_packet 1
#define receive_success 0
#define IDLE_state 2
class ReceiveThread : public QThread
{
    Q_OBJECT
public:
    ReceiveThread(QObject *parent);
    void set_serial_object(QSerialPort *object_serial);
    void run();
    bool stop = false;
    uint8_t receive_flag = IDLE_state;
    int attemp = 0;
signals:
    void packet_received(QByteArray);

private:
    QSerialPort *mSerial;
};

#endif // RECEIVETHREAD_H
