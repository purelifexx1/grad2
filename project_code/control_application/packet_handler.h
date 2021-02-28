#ifndef PACKET_HANDLER_H
#define PACKET_HANDLER_H

#include <QObject>
#include <QtCore>
#include "define_parameter.h"

class packet_handler: public QObject
{
    Q_OBJECT
public:
    packet_handler();
    define_parameter *system_parameter;
    void categorize(QByteArray packet);
    int32_t number_of_packet;
signals:
    void on_display_event(Display_packet);
private:
    void routing(QByteArray packet);
    void packet_extract(QByteArray packet);
    void Scara_position_received(QByteArray data);
    void Detail_Statusr_Handler(QByteArray data);
    QByteArray archive_buffer;
    bool archive_status = false;
};

#endif // PACKET_HANDLER_H
