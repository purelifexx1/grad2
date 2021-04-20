#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QFile>
#include <QtDebug>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    global_ui = ui;
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_bt_browse_import_clicked()
{
    QString file_directory = QFileDialog::getOpenFileName(this, "Open A File", "D:/gcode\\");
    ui->tb_import_file->setText(file_directory);
}

void MainWindow::on_bt_process_clicked()
{
    gcode->Clear_Data();
    Gcode_Decoder_DTC_TypeDef Gcode_DTC;
    try {
       gcode->Init_Current_Data(0, 0, 0, ui->tb_initial_speed->text().toDouble());
    }  catch (QString exp) {
        //log_console("Gcode Initial Speed invalid value");
        return;
    }
    QFile file(ui->tb_import_file->text());
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        //log_console("Cannot open a file");
        qDebug() << "error loading file";
        return;
    }
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        Gcode_DTC = gcode->Process_Line(line);
        if(Gcode_DTC != GCODE_OK){
            //log_console("Failed to read Gcode file. DTC code: " + QString::number(Gcode_DTC));
            return;
        }
    }
    file.close();

}

void MainWindow::on_bt_browse_save_clicked()
{
    QString filePath = QFileDialog::getExistingDirectory(this, "Open Save Directory");
    ui->tb_save_directory->setText(filePath);
}

void MainWindow::on_bt_save_file_clicked()
{
    Gcode_Decoder_DTC_TypeDef Gcode_DTC;
    Gcode_DTC = gcode->JobFile_Converter();
    if(Gcode_DTC != GCODE_OK){
        qDebug() << "Jobfile convert fail";
        return;
    }
}

void MainWindow::on_bt_test_clicked()
{
    char buffer[10] = {0};
    int t = sizeof(buffer);
    for(int i = 0; i < sizeof (buffer); i++){
        if(buffer[i] == 0) {
            buffer[i] = '2';
        }
    }
    qDebug() << "end";
}

void MainWindow::on_bt_set_offset_clicked()
{
    gcode->Gcode_Set_Offset(ui->tb_x_offset->text().toDouble(),ui->tb_y_offset->text().toDouble(),ui->tb_z_offset->text().toDouble(),
                            ui->tb_rx_offset->text().toDouble(),ui->tb_ry_offset->text().toDouble(),ui->tb_rz_offset->text().toDouble());
}
