#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "detect.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    VideoCapture = new detect(this);
    connect(VideoCapture, &detect::newPixmapCaptured, this, [&]()
    {
        ui->CameraFrame->setPixmap(VideoCapture->pixmap().scaled(640,480));
    });
}

MainWindow::~MainWindow()
{
    delete ui;
    VideoCapture -> terminate();
}


void MainWindow::on_CameraOn_Button_clicked()
{
    VideoCapture->start(QThread::HighPriority);

}

void MainWindow::on_CameraOff_Button_clicked()
{
    VideoCapture->terminate();
}
