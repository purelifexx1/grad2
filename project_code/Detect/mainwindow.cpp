#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "detect.h"
#include "calib.h"

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

    });
}

MainWindow::~MainWindow()
{
    delete ui;
    VideoCapture -> terminate();
    CalibFrame -> terminate();
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
