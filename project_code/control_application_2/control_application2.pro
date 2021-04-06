QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    calib.cpp \
    detect.cpp \
    define_parameter.cpp \
    gcode_decoder.cpp \
    main.cpp \
    mainwindow.cpp \
    packet_handler.cpp \
    receivethread.cpp \
    vision.cpp

HEADERS += \
    calib.h \
    detect.h \
    define_parameter.h \
    gcode_decoder.h \
    mainwindow.h \
    packet_handler.h \
    receivethread.h \
    vision.h

FORMS += \
    mainwindow.ui \
    vision.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target



win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../Downloads/long/opencv4.5.1/build/x64/vc15/lib/ -lopencv_world451
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../Downloads/long/opencv4.5.1/build/x64/vc15/lib/ -lopencv_world451d
else:unix: LIBS += -L$$PWD/../../../../../Downloads/long/opencv4.5.1/build/x64/vc15/lib/ -lopencv_world451

INCLUDEPATH += $$PWD/../../../../../Downloads/long/opencv4.5.1/build/include
DEPENDPATH += $$PWD/../../../../../Downloads/long/opencv4.5.1/build/include
