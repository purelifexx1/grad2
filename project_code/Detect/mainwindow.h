#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class Calib;
class detect;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private slots:
    void on_CameraOn_Button_clicked();

    void on_CameraOff_Button_clicked();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_checkBox_stateChanged(int arg1);

private:
    Ui::MainWindow *ui;
    detect *VideoCapture;
    Calib  *CalibFrame;
    bool enable=false;
    QByteArray command;
    void send_packet(double x, double y, double roll, QSerialPort* mSerial);

};
#endif // MAINWINDOW_H
