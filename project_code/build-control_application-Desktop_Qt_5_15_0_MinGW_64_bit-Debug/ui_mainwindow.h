/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.15.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QComboBox *com_list;
    QPushButton *bt_connect;
    QPushButton *bt_refresh;
    QGroupBox *groupBox;
    QPushButton *bt_robot_stop;
    QPushButton *bt_scan_limit;
    QPushButton *bt_inc_1;
    QPushButton *bt_inc_2;
    QPushButton *bt_inc_4;
    QPushButton *bt_inc_3;
    QPushButton *bt_inc_6;
    QPushButton *bt_inc_5;
    QPushButton *bt_home;
    QPushButton *bt_key_setsp;
    QLineEdit *tb_key_setsp;
    QPushButton *bt_inc_7;
    QPushButton *bt_inc_8;
    QGroupBox *groupBox_2;
    QLineEdit *tb_x_cor;
    QLineEdit *tb_y_cor;
    QLineEdit *tb_z_cor;
    QLineEdit *tb_roll_ang;
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;
    QLabel *label_4;
    QLabel *label_5;
    QLabel *label_6;
    QLineEdit *tb_a_factor;
    QLineEdit *tb_v_factor;
    QLabel *label_7;
    QLineEdit *tb_time;
    QPushButton *bt_movL;
    QGroupBox *groupBox_3;
    QRadioButton *rb_qva;
    QRadioButton *rb_qvt;
    QRadioButton *rb_qt;
    QGroupBox *groupBox_4;
    QRadioButton *rb_inc;
    QRadioButton *rb_abs;
    QGroupBox *groupBox_5;
    QRadioButton *rb_scur;
    QRadioButton *rb_lspb;
    QRadioButton *rb_linear;
    QPushButton *bt_model_setting;
    QGroupBox *groupBox_6;
    QPushButton *bt_off_magnet;
    QPushButton *bt_on_magnet;
    QGroupBox *groupBox_10;
    QLineEdit *tb_conveyor_sp;
    QPushButton *bt_conveyor_sp;
    QGroupBox *groupBox_7;
    QLineEdit *tb_x_cur_cor;
    QLineEdit *tb_y_cur_cor;
    QLineEdit *tb_z_cur_cor;
    QLineEdit *tb_roll_cur_cor;
    QLineEdit *tb_theta1_cur_val;
    QLineEdit *tb_theta2_cur_val;
    QLineEdit *tb_d3_cur_val;
    QLineEdit *tb_theta4_cur_val;
    QLabel *label_8;
    QLabel *label_9;
    QLabel *label_10;
    QLabel *label_11;
    QLabel *label_12;
    QLabel *label_13;
    QLabel *label_14;
    QLabel *label_15;
    QPushButton *bt_read_position;
    QRadioButton *rb_real_type;
    QRadioButton *rb_estimate_type;
    QCheckBox *cb_update_true_pos;
    QTextEdit *tb_console;
    QPushButton *bt_clear_console;
    QPushButton *testing;
    QTabWidget *tabWidget;
    QWidget *tab;
    QGroupBox *groupBox_8;
    QRadioButton *rb_manual;
    QRadioButton *rb_semi_auto;
    QRadioButton *rb_auto;
    QPushButton *bt_set_method;
    QRadioButton *rb_test;
    QRadioButton *rb_pick_and_place;
    QWidget *tab_2;
    QGroupBox *groupBox_9;
    QRadioButton *rb_test_mt1;
    QRadioButton *rb_test_mt2;
    QRadioButton *rb_test_mt3;
    QRadioButton *rb_test_mt4;
    QPushButton *bt_start_test;
    QSlider *hs_testmt_sign;
    QLabel *label_16;
    QLabel *label_17;
    QPushButton *bt_stop_test;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1079, 440);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        com_list = new QComboBox(centralwidget);
        com_list->setObjectName(QString::fromUtf8("com_list"));
        com_list->setGeometry(QRect(21, 10, 111, 21));
        bt_connect = new QPushButton(centralwidget);
        bt_connect->setObjectName(QString::fromUtf8("bt_connect"));
        bt_connect->setGeometry(QRect(230, 10, 80, 21));
        bt_refresh = new QPushButton(centralwidget);
        bt_refresh->setObjectName(QString::fromUtf8("bt_refresh"));
        bt_refresh->setGeometry(QRect(140, 10, 80, 21));
        groupBox = new QGroupBox(centralwidget);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(10, 30, 311, 181));
        bt_robot_stop = new QPushButton(groupBox);
        bt_robot_stop->setObjectName(QString::fromUtf8("bt_robot_stop"));
        bt_robot_stop->setGeometry(QRect(20, 30, 80, 21));
        bt_scan_limit = new QPushButton(groupBox);
        bt_scan_limit->setObjectName(QString::fromUtf8("bt_scan_limit"));
        bt_scan_limit->setGeometry(QRect(110, 30, 80, 21));
        bt_inc_1 = new QPushButton(groupBox);
        bt_inc_1->setObjectName(QString::fromUtf8("bt_inc_1"));
        bt_inc_1->setGeometry(QRect(20, 60, 80, 21));
        bt_inc_2 = new QPushButton(groupBox);
        bt_inc_2->setObjectName(QString::fromUtf8("bt_inc_2"));
        bt_inc_2->setGeometry(QRect(110, 60, 80, 21));
        bt_inc_4 = new QPushButton(groupBox);
        bt_inc_4->setObjectName(QString::fromUtf8("bt_inc_4"));
        bt_inc_4->setGeometry(QRect(110, 90, 80, 21));
        bt_inc_3 = new QPushButton(groupBox);
        bt_inc_3->setObjectName(QString::fromUtf8("bt_inc_3"));
        bt_inc_3->setGeometry(QRect(20, 90, 80, 21));
        bt_inc_6 = new QPushButton(groupBox);
        bt_inc_6->setObjectName(QString::fromUtf8("bt_inc_6"));
        bt_inc_6->setGeometry(QRect(110, 120, 80, 21));
        bt_inc_5 = new QPushButton(groupBox);
        bt_inc_5->setObjectName(QString::fromUtf8("bt_inc_5"));
        bt_inc_5->setGeometry(QRect(20, 120, 80, 21));
        bt_home = new QPushButton(groupBox);
        bt_home->setObjectName(QString::fromUtf8("bt_home"));
        bt_home->setGeometry(QRect(200, 30, 80, 21));
        bt_key_setsp = new QPushButton(groupBox);
        bt_key_setsp->setObjectName(QString::fromUtf8("bt_key_setsp"));
        bt_key_setsp->setGeometry(QRect(220, 130, 80, 21));
        tb_key_setsp = new QLineEdit(groupBox);
        tb_key_setsp->setObjectName(QString::fromUtf8("tb_key_setsp"));
        tb_key_setsp->setGeometry(QRect(220, 100, 81, 21));
        bt_inc_7 = new QPushButton(groupBox);
        bt_inc_7->setObjectName(QString::fromUtf8("bt_inc_7"));
        bt_inc_7->setGeometry(QRect(20, 150, 80, 21));
        bt_inc_8 = new QPushButton(groupBox);
        bt_inc_8->setObjectName(QString::fromUtf8("bt_inc_8"));
        bt_inc_8->setGeometry(QRect(110, 150, 80, 21));
        groupBox_2 = new QGroupBox(centralwidget);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        groupBox_2->setGeometry(QRect(330, 10, 391, 351));
        tb_x_cor = new QLineEdit(groupBox_2);
        tb_x_cor->setObjectName(QString::fromUtf8("tb_x_cor"));
        tb_x_cor->setGeometry(QRect(10, 30, 113, 21));
        tb_y_cor = new QLineEdit(groupBox_2);
        tb_y_cor->setObjectName(QString::fromUtf8("tb_y_cor"));
        tb_y_cor->setGeometry(QRect(10, 60, 113, 21));
        tb_z_cor = new QLineEdit(groupBox_2);
        tb_z_cor->setObjectName(QString::fromUtf8("tb_z_cor"));
        tb_z_cor->setGeometry(QRect(10, 90, 113, 21));
        tb_roll_ang = new QLineEdit(groupBox_2);
        tb_roll_ang->setObjectName(QString::fromUtf8("tb_roll_ang"));
        tb_roll_ang->setGeometry(QRect(10, 120, 113, 21));
        label = new QLabel(groupBox_2);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(140, 60, 47, 13));
        label_2 = new QLabel(groupBox_2);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(140, 30, 47, 13));
        label_3 = new QLabel(groupBox_2);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(140, 120, 47, 13));
        label_4 = new QLabel(groupBox_2);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(140, 90, 47, 13));
        label_5 = new QLabel(groupBox_2);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(300, 60, 47, 13));
        label_6 = new QLabel(groupBox_2);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setGeometry(QRect(300, 30, 61, 16));
        tb_a_factor = new QLineEdit(groupBox_2);
        tb_a_factor->setObjectName(QString::fromUtf8("tb_a_factor"));
        tb_a_factor->setGeometry(QRect(180, 60, 113, 21));
        tb_v_factor = new QLineEdit(groupBox_2);
        tb_v_factor->setObjectName(QString::fromUtf8("tb_v_factor"));
        tb_v_factor->setGeometry(QRect(180, 30, 113, 21));
        label_7 = new QLabel(groupBox_2);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setGeometry(QRect(300, 90, 47, 13));
        tb_time = new QLineEdit(groupBox_2);
        tb_time->setObjectName(QString::fromUtf8("tb_time"));
        tb_time->setGeometry(QRect(180, 90, 113, 21));
        bt_movL = new QPushButton(groupBox_2);
        bt_movL->setObjectName(QString::fromUtf8("bt_movL"));
        bt_movL->setGeometry(QRect(190, 120, 80, 21));
        groupBox_3 = new QGroupBox(groupBox_2);
        groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
        groupBox_3->setGeometry(QRect(20, 150, 151, 41));
        rb_qva = new QRadioButton(groupBox_3);
        rb_qva->setObjectName(QString::fromUtf8("rb_qva"));
        rb_qva->setGeometry(QRect(60, 20, 51, 19));
        rb_qvt = new QRadioButton(groupBox_3);
        rb_qvt->setObjectName(QString::fromUtf8("rb_qvt"));
        rb_qvt->setGeometry(QRect(10, 20, 41, 19));
        rb_qvt->setCheckable(true);
        rb_qvt->setChecked(false);
        rb_qt = new QRadioButton(groupBox_3);
        rb_qt->setObjectName(QString::fromUtf8("rb_qt"));
        rb_qt->setGeometry(QRect(110, 20, 41, 19));
        groupBox_4 = new QGroupBox(groupBox_2);
        groupBox_4->setObjectName(QString::fromUtf8("groupBox_4"));
        groupBox_4->setGeometry(QRect(230, 160, 151, 41));
        rb_inc = new QRadioButton(groupBox_4);
        rb_inc->setObjectName(QString::fromUtf8("rb_inc"));
        rb_inc->setGeometry(QRect(0, 20, 84, 19));
        rb_abs = new QRadioButton(groupBox_4);
        rb_abs->setObjectName(QString::fromUtf8("rb_abs"));
        rb_abs->setGeometry(QRect(80, 20, 84, 19));
        rb_abs->setChecked(true);
        groupBox_5 = new QGroupBox(groupBox_2);
        groupBox_5->setObjectName(QString::fromUtf8("groupBox_5"));
        groupBox_5->setGeometry(QRect(200, 210, 161, 41));
        rb_scur = new QRadioButton(groupBox_5);
        rb_scur->setObjectName(QString::fromUtf8("rb_scur"));
        rb_scur->setGeometry(QRect(50, 20, 61, 19));
        rb_lspb = new QRadioButton(groupBox_5);
        rb_lspb->setObjectName(QString::fromUtf8("rb_lspb"));
        rb_lspb->setGeometry(QRect(0, 20, 51, 19));
        rb_lspb->setChecked(true);
        rb_linear = new QRadioButton(groupBox_5);
        rb_linear->setObjectName(QString::fromUtf8("rb_linear"));
        rb_linear->setGeometry(QRect(110, 20, 51, 19));
        bt_model_setting = new QPushButton(groupBox_2);
        bt_model_setting->setObjectName(QString::fromUtf8("bt_model_setting"));
        bt_model_setting->setGeometry(QRect(290, 260, 80, 21));
        groupBox_6 = new QGroupBox(groupBox_2);
        groupBox_6->setObjectName(QString::fromUtf8("groupBox_6"));
        groupBox_6->setGeometry(QRect(20, 200, 171, 51));
        bt_off_magnet = new QPushButton(groupBox_6);
        bt_off_magnet->setObjectName(QString::fromUtf8("bt_off_magnet"));
        bt_off_magnet->setGeometry(QRect(90, 20, 80, 21));
        bt_on_magnet = new QPushButton(groupBox_6);
        bt_on_magnet->setObjectName(QString::fromUtf8("bt_on_magnet"));
        bt_on_magnet->setGeometry(QRect(0, 20, 80, 21));
        groupBox_10 = new QGroupBox(groupBox_2);
        groupBox_10->setObjectName(QString::fromUtf8("groupBox_10"));
        groupBox_10->setGeometry(QRect(20, 260, 181, 71));
        tb_conveyor_sp = new QLineEdit(groupBox_10);
        tb_conveyor_sp->setObjectName(QString::fromUtf8("tb_conveyor_sp"));
        tb_conveyor_sp->setGeometry(QRect(10, 20, 113, 21));
        bt_conveyor_sp = new QPushButton(groupBox_10);
        bt_conveyor_sp->setObjectName(QString::fromUtf8("bt_conveyor_sp"));
        bt_conveyor_sp->setGeometry(QRect(10, 40, 41, 21));
        groupBox_7 = new QGroupBox(centralwidget);
        groupBox_7->setObjectName(QString::fromUtf8("groupBox_7"));
        groupBox_7->setGeometry(QRect(10, 210, 311, 181));
        tb_x_cur_cor = new QLineEdit(groupBox_7);
        tb_x_cur_cor->setObjectName(QString::fromUtf8("tb_x_cur_cor"));
        tb_x_cur_cor->setGeometry(QRect(10, 20, 81, 21));
        tb_y_cur_cor = new QLineEdit(groupBox_7);
        tb_y_cur_cor->setObjectName(QString::fromUtf8("tb_y_cur_cor"));
        tb_y_cur_cor->setGeometry(QRect(10, 50, 81, 21));
        tb_z_cur_cor = new QLineEdit(groupBox_7);
        tb_z_cur_cor->setObjectName(QString::fromUtf8("tb_z_cur_cor"));
        tb_z_cur_cor->setGeometry(QRect(10, 80, 81, 21));
        tb_roll_cur_cor = new QLineEdit(groupBox_7);
        tb_roll_cur_cor->setObjectName(QString::fromUtf8("tb_roll_cur_cor"));
        tb_roll_cur_cor->setGeometry(QRect(10, 110, 81, 21));
        tb_theta1_cur_val = new QLineEdit(groupBox_7);
        tb_theta1_cur_val->setObjectName(QString::fromUtf8("tb_theta1_cur_val"));
        tb_theta1_cur_val->setGeometry(QRect(160, 20, 81, 21));
        tb_theta2_cur_val = new QLineEdit(groupBox_7);
        tb_theta2_cur_val->setObjectName(QString::fromUtf8("tb_theta2_cur_val"));
        tb_theta2_cur_val->setGeometry(QRect(160, 50, 81, 21));
        tb_d3_cur_val = new QLineEdit(groupBox_7);
        tb_d3_cur_val->setObjectName(QString::fromUtf8("tb_d3_cur_val"));
        tb_d3_cur_val->setGeometry(QRect(160, 80, 81, 21));
        tb_theta4_cur_val = new QLineEdit(groupBox_7);
        tb_theta4_cur_val->setObjectName(QString::fromUtf8("tb_theta4_cur_val"));
        tb_theta4_cur_val->setGeometry(QRect(160, 110, 81, 21));
        label_8 = new QLabel(groupBox_7);
        label_8->setObjectName(QString::fromUtf8("label_8"));
        label_8->setGeometry(QRect(100, 20, 47, 13));
        label_9 = new QLabel(groupBox_7);
        label_9->setObjectName(QString::fromUtf8("label_9"));
        label_9->setGeometry(QRect(100, 50, 47, 13));
        label_10 = new QLabel(groupBox_7);
        label_10->setObjectName(QString::fromUtf8("label_10"));
        label_10->setGeometry(QRect(100, 80, 47, 13));
        label_11 = new QLabel(groupBox_7);
        label_11->setObjectName(QString::fromUtf8("label_11"));
        label_11->setGeometry(QRect(100, 110, 47, 13));
        label_12 = new QLabel(groupBox_7);
        label_12->setObjectName(QString::fromUtf8("label_12"));
        label_12->setGeometry(QRect(250, 110, 47, 13));
        label_13 = new QLabel(groupBox_7);
        label_13->setObjectName(QString::fromUtf8("label_13"));
        label_13->setGeometry(QRect(250, 20, 47, 13));
        label_14 = new QLabel(groupBox_7);
        label_14->setObjectName(QString::fromUtf8("label_14"));
        label_14->setGeometry(QRect(250, 80, 47, 13));
        label_15 = new QLabel(groupBox_7);
        label_15->setObjectName(QString::fromUtf8("label_15"));
        label_15->setGeometry(QRect(250, 50, 47, 13));
        bt_read_position = new QPushButton(groupBox_7);
        bt_read_position->setObjectName(QString::fromUtf8("bt_read_position"));
        bt_read_position->setGeometry(QRect(10, 140, 80, 21));
        rb_real_type = new QRadioButton(groupBox_7);
        rb_real_type->setObjectName(QString::fromUtf8("rb_real_type"));
        rb_real_type->setGeometry(QRect(100, 140, 71, 19));
        rb_estimate_type = new QRadioButton(groupBox_7);
        rb_estimate_type->setObjectName(QString::fromUtf8("rb_estimate_type"));
        rb_estimate_type->setGeometry(QRect(100, 160, 91, 19));
        rb_estimate_type->setChecked(true);
        cb_update_true_pos = new QCheckBox(groupBox_7);
        cb_update_true_pos->setObjectName(QString::fromUtf8("cb_update_true_pos"));
        cb_update_true_pos->setGeometry(QRect(190, 140, 121, 19));
        tb_console = new QTextEdit(centralwidget);
        tb_console->setObjectName(QString::fromUtf8("tb_console"));
        tb_console->setGeometry(QRect(730, 40, 261, 181));
        bt_clear_console = new QPushButton(centralwidget);
        bt_clear_console->setObjectName(QString::fromUtf8("bt_clear_console"));
        bt_clear_console->setGeometry(QRect(920, 10, 80, 21));
        testing = new QPushButton(centralwidget);
        testing->setObjectName(QString::fromUtf8("testing"));
        testing->setGeometry(QRect(1000, 170, 80, 21));
        tabWidget = new QTabWidget(centralwidget);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tabWidget->setGeometry(QRect(730, 230, 241, 141));
        tab = new QWidget();
        tab->setObjectName(QString::fromUtf8("tab"));
        groupBox_8 = new QGroupBox(tab);
        groupBox_8->setObjectName(QString::fromUtf8("groupBox_8"));
        groupBox_8->setGeometry(QRect(10, 0, 211, 111));
        rb_manual = new QRadioButton(groupBox_8);
        rb_manual->setObjectName(QString::fromUtf8("rb_manual"));
        rb_manual->setGeometry(QRect(10, 20, 84, 19));
        rb_semi_auto = new QRadioButton(groupBox_8);
        rb_semi_auto->setObjectName(QString::fromUtf8("rb_semi_auto"));
        rb_semi_auto->setGeometry(QRect(10, 40, 84, 19));
        rb_semi_auto->setChecked(true);
        rb_auto = new QRadioButton(groupBox_8);
        rb_auto->setObjectName(QString::fromUtf8("rb_auto"));
        rb_auto->setGeometry(QRect(10, 60, 84, 19));
        bt_set_method = new QPushButton(groupBox_8);
        bt_set_method->setObjectName(QString::fromUtf8("bt_set_method"));
        bt_set_method->setGeometry(QRect(10, 80, 80, 21));
        rb_test = new QRadioButton(groupBox_8);
        rb_test->setObjectName(QString::fromUtf8("rb_test"));
        rb_test->setGeometry(QRect(90, 20, 84, 19));
        rb_pick_and_place = new QRadioButton(groupBox_8);
        rb_pick_and_place->setObjectName(QString::fromUtf8("rb_pick_and_place"));
        rb_pick_and_place->setGeometry(QRect(90, 40, 101, 17));
        tabWidget->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QString::fromUtf8("tab_2"));
        groupBox_9 = new QGroupBox(tab_2);
        groupBox_9->setObjectName(QString::fromUtf8("groupBox_9"));
        groupBox_9->setGeometry(QRect(0, 0, 231, 101));
        rb_test_mt1 = new QRadioButton(groupBox_9);
        rb_test_mt1->setObjectName(QString::fromUtf8("rb_test_mt1"));
        rb_test_mt1->setGeometry(QRect(10, 20, 61, 19));
        rb_test_mt2 = new QRadioButton(groupBox_9);
        rb_test_mt2->setObjectName(QString::fromUtf8("rb_test_mt2"));
        rb_test_mt2->setGeometry(QRect(10, 40, 61, 19));
        rb_test_mt3 = new QRadioButton(groupBox_9);
        rb_test_mt3->setObjectName(QString::fromUtf8("rb_test_mt3"));
        rb_test_mt3->setGeometry(QRect(10, 60, 61, 19));
        rb_test_mt4 = new QRadioButton(groupBox_9);
        rb_test_mt4->setObjectName(QString::fromUtf8("rb_test_mt4"));
        rb_test_mt4->setGeometry(QRect(10, 80, 61, 19));
        bt_start_test = new QPushButton(groupBox_9);
        bt_start_test->setObjectName(QString::fromUtf8("bt_start_test"));
        bt_start_test->setGeometry(QRect(80, 40, 61, 21));
        hs_testmt_sign = new QSlider(groupBox_9);
        hs_testmt_sign->setObjectName(QString::fromUtf8("hs_testmt_sign"));
        hs_testmt_sign->setGeometry(QRect(130, 70, 41, 16));
        hs_testmt_sign->setMaximum(1);
        hs_testmt_sign->setOrientation(Qt::Horizontal);
        label_16 = new QLabel(groupBox_9);
        label_16->setObjectName(QString::fromUtf8("label_16"));
        label_16->setGeometry(QRect(80, 70, 47, 13));
        label_17 = new QLabel(groupBox_9);
        label_17->setObjectName(QString::fromUtf8("label_17"));
        label_17->setGeometry(QRect(180, 70, 47, 13));
        bt_stop_test = new QPushButton(groupBox_9);
        bt_stop_test->setObjectName(QString::fromUtf8("bt_stop_test"));
        bt_stop_test->setGeometry(QRect(150, 40, 61, 21));
        tabWidget->addTab(tab_2, QString());
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 1079, 21));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        bt_connect->setText(QCoreApplication::translate("MainWindow", "Connect", nullptr));
        bt_refresh->setText(QCoreApplication::translate("MainWindow", "Refresh", nullptr));
        groupBox->setTitle(QCoreApplication::translate("MainWindow", "Robot command", nullptr));
        bt_robot_stop->setText(QCoreApplication::translate("MainWindow", "Stop", nullptr));
        bt_scan_limit->setText(QCoreApplication::translate("MainWindow", "scan limit", nullptr));
        bt_inc_1->setText(QCoreApplication::translate("MainWindow", "Inc x", nullptr));
        bt_inc_2->setText(QCoreApplication::translate("MainWindow", "Dec x", nullptr));
        bt_inc_4->setText(QCoreApplication::translate("MainWindow", "Dec y", nullptr));
        bt_inc_3->setText(QCoreApplication::translate("MainWindow", "Inc y", nullptr));
        bt_inc_6->setText(QCoreApplication::translate("MainWindow", "Dec z", nullptr));
        bt_inc_5->setText(QCoreApplication::translate("MainWindow", "Inc z", nullptr));
        bt_home->setText(QCoreApplication::translate("MainWindow", "Move Home", nullptr));
        bt_key_setsp->setText(QCoreApplication::translate("MainWindow", "Set key speed", nullptr));
        bt_inc_7->setText(QCoreApplication::translate("MainWindow", "Inc roll", nullptr));
        bt_inc_8->setText(QCoreApplication::translate("MainWindow", "Dec roll", nullptr));
        groupBox_2->setTitle(QCoreApplication::translate("MainWindow", "GroupBox", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "Y", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "X", nullptr));
        label_3->setText(QCoreApplication::translate("MainWindow", "Roll", nullptr));
        label_4->setText(QCoreApplication::translate("MainWindow", "Z", nullptr));
        label_5->setText(QCoreApplication::translate("MainWindow", "A factor", nullptr));
        label_6->setText(QCoreApplication::translate("MainWindow", "V factor(%)", nullptr));
        label_7->setText(QCoreApplication::translate("MainWindow", "Time", nullptr));
        bt_movL->setText(QCoreApplication::translate("MainWindow", "Move Line", nullptr));
        groupBox_3->setTitle(QCoreApplication::translate("MainWindow", "Calculate model", nullptr));
        rb_qva->setText(QCoreApplication::translate("MainWindow", "QVA", nullptr));
        rb_qvt->setText(QCoreApplication::translate("MainWindow", "QVT", nullptr));
        rb_qt->setText(QCoreApplication::translate("MainWindow", "QT", nullptr));
        groupBox_4->setTitle(QCoreApplication::translate("MainWindow", "Move method", nullptr));
        rb_inc->setText(QCoreApplication::translate("MainWindow", "Incremental", nullptr));
        rb_abs->setText(QCoreApplication::translate("MainWindow", "Absolute", nullptr));
        groupBox_5->setTitle(QCoreApplication::translate("MainWindow", "Trajectory planning model", nullptr));
        rb_scur->setText(QCoreApplication::translate("MainWindow", "SCurve", nullptr));
        rb_lspb->setText(QCoreApplication::translate("MainWindow", "LSPB", nullptr));
        rb_linear->setText(QCoreApplication::translate("MainWindow", "Linear", nullptr));
        bt_model_setting->setText(QCoreApplication::translate("MainWindow", "Set", nullptr));
        groupBox_6->setTitle(QCoreApplication::translate("MainWindow", "Output shaft", nullptr));
        bt_off_magnet->setText(QCoreApplication::translate("MainWindow", "Magnet off", nullptr));
        bt_on_magnet->setText(QCoreApplication::translate("MainWindow", "Magnet on", nullptr));
        groupBox_10->setTitle(QCoreApplication::translate("MainWindow", "GroupBox", nullptr));
        bt_conveyor_sp->setText(QCoreApplication::translate("MainWindow", "Set", nullptr));
        groupBox_7->setTitle(QCoreApplication::translate("MainWindow", "Current position", nullptr));
        label_8->setText(QCoreApplication::translate("MainWindow", "X", nullptr));
        label_9->setText(QCoreApplication::translate("MainWindow", "Y", nullptr));
        label_10->setText(QCoreApplication::translate("MainWindow", "Z", nullptr));
        label_11->setText(QCoreApplication::translate("MainWindow", "Roll", nullptr));
        label_12->setText(QCoreApplication::translate("MainWindow", "theta4", nullptr));
        label_13->setText(QCoreApplication::translate("MainWindow", "theta1", nullptr));
        label_14->setText(QCoreApplication::translate("MainWindow", "D3", nullptr));
        label_15->setText(QCoreApplication::translate("MainWindow", "theta2", nullptr));
        bt_read_position->setText(QCoreApplication::translate("MainWindow", "Read position", nullptr));
        rb_real_type->setText(QCoreApplication::translate("MainWindow", "Real Type", nullptr));
        rb_estimate_type->setText(QCoreApplication::translate("MainWindow", "Estimate Type", nullptr));
        cb_update_true_pos->setText(QCoreApplication::translate("MainWindow", "Update True Position", nullptr));
        bt_clear_console->setText(QCoreApplication::translate("MainWindow", "Clear console", nullptr));
        testing->setText(QCoreApplication::translate("MainWindow", "PushButton", nullptr));
        groupBox_8->setTitle(QCoreApplication::translate("MainWindow", "Change method", nullptr));
        rb_manual->setText(QCoreApplication::translate("MainWindow", "MANUAL", nullptr));
        rb_semi_auto->setText(QCoreApplication::translate("MainWindow", "SEMI-AUTO", nullptr));
        rb_auto->setText(QCoreApplication::translate("MainWindow", "AUTO", nullptr));
        bt_set_method->setText(QCoreApplication::translate("MainWindow", "Set method", nullptr));
        rb_test->setText(QCoreApplication::translate("MainWindow", "TEST", nullptr));
        rb_pick_and_place->setText(QCoreApplication::translate("MainWindow", "PICK AND PLACE", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab), QCoreApplication::translate("MainWindow", "Tab 1", nullptr));
        groupBox_9->setTitle(QCoreApplication::translate("MainWindow", "Test motor", nullptr));
        rb_test_mt1->setText(QCoreApplication::translate("MainWindow", "Motor1", nullptr));
        rb_test_mt2->setText(QCoreApplication::translate("MainWindow", "Motor2", nullptr));
        rb_test_mt3->setText(QCoreApplication::translate("MainWindow", "Motor3", nullptr));
        rb_test_mt4->setText(QCoreApplication::translate("MainWindow", "Motor4", nullptr));
        bt_start_test->setText(QCoreApplication::translate("MainWindow", "Start test", nullptr));
        label_16->setText(QCoreApplication::translate("MainWindow", "Positive", nullptr));
        label_17->setText(QCoreApplication::translate("MainWindow", "Negative", nullptr));
        bt_stop_test->setText(QCoreApplication::translate("MainWindow", "Stop test", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QCoreApplication::translate("MainWindow", "Tab 2", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
