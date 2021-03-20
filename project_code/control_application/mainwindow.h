#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "receivethread.h"
#include "define_parameter.h"
#include "packet_handler.h"
#include "gcode_decoder.h"
#define log_console(content) ui->tb_console->append(content)
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    ReceiveThread *Received_Thread;    
    packet_handler *_packet_handler = new packet_handler();
    std::vector<uint8_t> FIFO_Buffer;

private slots:
    void on_bt_refresh_clicked();

    void on_bt_connect_clicked();

    void on_bt_robot_stop_clicked();
    void on_bt_scan_limit_clicked();

    void on_bt_home_clicked();

    void on_bt_movL_clicked();

    void on_bt_model_setting_clicked();

    void on_bt_on_magnet_clicked();

    void on_bt_off_magnet_clicked();

    void on_bt_read_position_clicked();

    void on_bt_keycommand();

    void on_bt_key_setsp_clicked();

    void on_bt_set_method_clicked();

    void on_bt_clear_console_clicked();

    void on_testing_clicked();

    void on_bt_testmt();

    void on_bt_conveyor_sp_clicked();

    void on_bt_browse_clicked();

    void on_bt_process_clicked();

    void on_bt_gcode_start_clicked();

    void on_bt_gcode_stop_clicked();

    void on_bt_gcode_pause_clicked();

    void on_bt_gcode_resume_clicked();

    void on_bt_gcode_configure_clicked();

    void on_bt_movC1_clicked();

public slots:
    void received_callback(QByteArray log_data);
    void display_event(Display_packet data);
private:
    Ui::MainWindow *ui;
    void object_detected(double x, double y, double roll);
    Coordinate_Receive_Handler_TypeDef MovC_ACK = DISPLAY_ONLY;
    void MovC_Hanlder(Coordinate_Receive_Handler_TypeDef type, Display_packet data);
};
#endif // MAINWINDOW_H
