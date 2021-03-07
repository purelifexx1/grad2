#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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

private:
    Ui::MainWindow *ui;
    detect *VideoCapture;
    Calib  *CalibFrame;

};
#endif // MAINWINDOW_H
