/********************************************************************************
** Form generated from reading UI file 'vision.ui'
**
** Created by: Qt User Interface Compiler version 5.15.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_VISION_H
#define UI_VISION_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Vision
{
public:
    QWidget *centralwidget;
    QPushButton *pushButton;
    QLabel *CameraFrame;
    QCheckBox *checkBox_2;
    QPushButton *CameraOff_Button;
    QPushButton *pushButton_2;
    QCheckBox *checkBox;
    QPushButton *CameraOn_Button;
    QComboBox *comboBox;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *Vision)
    {
        if (Vision->objectName().isEmpty())
            Vision->setObjectName(QString::fromUtf8("Vision"));
        Vision->resize(671, 600);
        centralwidget = new QWidget(Vision);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        pushButton = new QPushButton(centralwidget);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setGeometry(QRect(105, 9, 75, 23));
        CameraFrame = new QLabel(centralwidget);
        CameraFrame->setObjectName(QString::fromUtf8("CameraFrame"));
        CameraFrame->setGeometry(QRect(9, 38, 640, 480));
        CameraFrame->setMinimumSize(QSize(640, 480));
        CameraFrame->setFrameShape(QFrame::Box);
        checkBox_2 = new QCheckBox(centralwidget);
        checkBox_2->setObjectName(QString::fromUtf8("checkBox_2"));
        checkBox_2->setGeometry(QRect(580, 10, 81, 17));
        CameraOff_Button = new QPushButton(centralwidget);
        CameraOff_Button->setObjectName(QString::fromUtf8("CameraOff_Button"));
        CameraOff_Button->setGeometry(QRect(295, 9, 75, 23));
        pushButton_2 = new QPushButton(centralwidget);
        pushButton_2->setObjectName(QString::fromUtf8("pushButton_2"));
        pushButton_2->setGeometry(QRect(200, 9, 75, 23));
        checkBox = new QCheckBox(centralwidget);
        checkBox->setObjectName(QString::fromUtf8("checkBox"));
        checkBox->setGeometry(QRect(510, 10, 55, 17));
        CameraOn_Button = new QPushButton(centralwidget);
        CameraOn_Button->setObjectName(QString::fromUtf8("CameraOn_Button"));
        CameraOn_Button->setGeometry(QRect(9, 9, 75, 23));
        comboBox = new QComboBox(centralwidget);
        comboBox->setObjectName(QString::fromUtf8("comboBox"));
        comboBox->setGeometry(QRect(400, 10, 69, 20));
        Vision->setCentralWidget(centralwidget);
        menubar = new QMenuBar(Vision);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 671, 21));
        Vision->setMenuBar(menubar);
        statusbar = new QStatusBar(Vision);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        Vision->setStatusBar(statusbar);

        retranslateUi(Vision);

        QMetaObject::connectSlotsByName(Vision);
    } // setupUi

    void retranslateUi(QMainWindow *Vision)
    {
        Vision->setWindowTitle(QCoreApplication::translate("Vision", "MainWindow", nullptr));
        pushButton->setText(QCoreApplication::translate("Vision", "referesh", nullptr));
        CameraFrame->setText(QString());
        checkBox_2->setText(QCoreApplication::translate("Vision", "send enable", nullptr));
        CameraOff_Button->setText(QCoreApplication::translate("Vision", "Camera Off", nullptr));
        pushButton_2->setText(QCoreApplication::translate("Vision", "Connect", nullptr));
        checkBox->setText(QCoreApplication::translate("Vision", "enable", nullptr));
        CameraOn_Button->setText(QCoreApplication::translate("Vision", "Camera On", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Vision: public Ui_Vision {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_VISION_H
