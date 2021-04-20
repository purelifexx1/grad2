#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "gcode_decoder.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_bt_browse_import_clicked();

    void on_bt_process_clicked();

    void on_bt_browse_save_clicked();

    void on_bt_save_file_clicked();

    void on_bt_test_clicked();

    void on_bt_set_offset_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
