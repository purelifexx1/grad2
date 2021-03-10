QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    calib.cpp \
    detect.cpp \
    main.cpp \
    mainwindow.cpp \
    object_detect_handler.cpp

HEADERS += \
    calib.h \
    detect.h \
    mainwindow.h \
    object_detect_handler.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../opencv4.5.1/build/x64/vc15/lib/ -lopencv_world451
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../opencv4.5.1/build/x64/vc15/lib/ -lopencv_world451d
else:unix: LIBS += -L$$PWD/../../opencv4.5.1/build/x64/vc15/lib/ -lopencv_world451

INCLUDEPATH += $$PWD/../../opencv4.5.1/build/include
DEPENDPATH += $$PWD/../../opencv4.5.1/build/include


