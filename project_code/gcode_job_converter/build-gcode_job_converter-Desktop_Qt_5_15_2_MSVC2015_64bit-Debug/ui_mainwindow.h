/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QLineEdit *tb_import_file;
    QPushButton *bt_process;
    QLineEdit *tb_save_directory;
    QPushButton *bt_browse_import;
    QPushButton *bt_browse_save;
    QPushButton *bt_save_file;
    QLineEdit *tb_file_name;
    QLabel *label;
    QLabel *label_2;
    QLineEdit *tb_initial_speed;
    QLabel *label_3;
    QPushButton *bt_test;
    QLineEdit *tb_x_offset;
    QLineEdit *tb_y_offset;
    QLineEdit *tb_z_offset;
    QLineEdit *tb_ry_offset;
    QLineEdit *tb_rz_offset;
    QLineEdit *tb_rx_offset;
    QLabel *label_4;
    QLabel *label_5;
    QLabel *label_6;
    QLabel *label_7;
    QLabel *label_8;
    QLabel *label_9;
    QPushButton *bt_set_offset;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(692, 318);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        tb_import_file = new QLineEdit(centralwidget);
        tb_import_file->setObjectName(QString::fromUtf8("tb_import_file"));
        tb_import_file->setGeometry(QRect(22, 50, 191, 21));
        bt_process = new QPushButton(centralwidget);
        bt_process->setObjectName(QString::fromUtf8("bt_process"));
        bt_process->setGeometry(QRect(20, 80, 80, 21));
        tb_save_directory = new QLineEdit(centralwidget);
        tb_save_directory->setObjectName(QString::fromUtf8("tb_save_directory"));
        tb_save_directory->setGeometry(QRect(342, 50, 241, 21));
        bt_browse_import = new QPushButton(centralwidget);
        bt_browse_import->setObjectName(QString::fromUtf8("bt_browse_import"));
        bt_browse_import->setGeometry(QRect(220, 50, 80, 21));
        bt_browse_save = new QPushButton(centralwidget);
        bt_browse_save->setObjectName(QString::fromUtf8("bt_browse_save"));
        bt_browse_save->setGeometry(QRect(590, 50, 80, 21));
        bt_save_file = new QPushButton(centralwidget);
        bt_save_file->setObjectName(QString::fromUtf8("bt_save_file"));
        bt_save_file->setGeometry(QRect(520, 130, 80, 21));
        tb_file_name = new QLineEdit(centralwidget);
        tb_file_name->setObjectName(QString::fromUtf8("tb_file_name"));
        tb_file_name->setGeometry(QRect(342, 130, 171, 21));
        label = new QLabel(centralwidget);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(400, 70, 71, 21));
        label_2 = new QLabel(centralwidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(400, 150, 71, 21));
        tb_initial_speed = new QLineEdit(centralwidget);
        tb_initial_speed->setObjectName(QString::fromUtf8("tb_initial_speed"));
        tb_initial_speed->setGeometry(QRect(20, 130, 113, 21));
        label_3 = new QLabel(centralwidget);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(140, 130, 71, 21));
        bt_test = new QPushButton(centralwidget);
        bt_test->setObjectName(QString::fromUtf8("bt_test"));
        bt_test->setGeometry(QRect(230, 130, 80, 21));
        tb_x_offset = new QLineEdit(centralwidget);
        tb_x_offset->setObjectName(QString::fromUtf8("tb_x_offset"));
        tb_x_offset->setGeometry(QRect(40, 170, 81, 21));
        tb_y_offset = new QLineEdit(centralwidget);
        tb_y_offset->setObjectName(QString::fromUtf8("tb_y_offset"));
        tb_y_offset->setGeometry(QRect(130, 170, 81, 21));
        tb_z_offset = new QLineEdit(centralwidget);
        tb_z_offset->setObjectName(QString::fromUtf8("tb_z_offset"));
        tb_z_offset->setGeometry(QRect(220, 170, 81, 21));
        tb_ry_offset = new QLineEdit(centralwidget);
        tb_ry_offset->setObjectName(QString::fromUtf8("tb_ry_offset"));
        tb_ry_offset->setGeometry(QRect(130, 220, 81, 21));
        tb_rz_offset = new QLineEdit(centralwidget);
        tb_rz_offset->setObjectName(QString::fromUtf8("tb_rz_offset"));
        tb_rz_offset->setGeometry(QRect(220, 220, 81, 21));
        tb_rx_offset = new QLineEdit(centralwidget);
        tb_rx_offset->setObjectName(QString::fromUtf8("tb_rx_offset"));
        tb_rx_offset->setGeometry(QRect(40, 220, 81, 21));
        label_4 = new QLabel(centralwidget);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(40, 190, 47, 13));
        label_5 = new QLabel(centralwidget);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(130, 190, 47, 13));
        label_6 = new QLabel(centralwidget);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setGeometry(QRect(220, 190, 47, 13));
        label_7 = new QLabel(centralwidget);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setGeometry(QRect(220, 240, 47, 13));
        label_8 = new QLabel(centralwidget);
        label_8->setObjectName(QString::fromUtf8("label_8"));
        label_8->setGeometry(QRect(130, 240, 47, 13));
        label_9 = new QLabel(centralwidget);
        label_9->setObjectName(QString::fromUtf8("label_9"));
        label_9->setGeometry(QRect(40, 240, 47, 13));
        bt_set_offset = new QPushButton(centralwidget);
        bt_set_offset->setObjectName(QString::fromUtf8("bt_set_offset"));
        bt_set_offset->setGeometry(QRect(310, 170, 80, 21));
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 692, 20));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        bt_process->setText(QCoreApplication::translate("MainWindow", "Process", nullptr));
        bt_browse_import->setText(QCoreApplication::translate("MainWindow", "browse", nullptr));
        bt_browse_save->setText(QCoreApplication::translate("MainWindow", "browse", nullptr));
        bt_save_file->setText(QCoreApplication::translate("MainWindow", "Save file", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "Save directory", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "File name", nullptr));
        label_3->setText(QCoreApplication::translate("MainWindow", "Initial Speed", nullptr));
        bt_test->setText(QCoreApplication::translate("MainWindow", "Test button", nullptr));
        label_4->setText(QCoreApplication::translate("MainWindow", "X", nullptr));
        label_5->setText(QCoreApplication::translate("MainWindow", "Y", nullptr));
        label_6->setText(QCoreApplication::translate("MainWindow", "Z", nullptr));
        label_7->setText(QCoreApplication::translate("MainWindow", "Rz", nullptr));
        label_8->setText(QCoreApplication::translate("MainWindow", "Ry", nullptr));
        label_9->setText(QCoreApplication::translate("MainWindow", "Rx", nullptr));
        bt_set_offset->setText(QCoreApplication::translate("MainWindow", "Set", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
