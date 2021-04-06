/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../control_application_2/mainwindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_MainWindow_t {
    QByteArrayData data[37];
    char stringdata0[763];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MainWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MainWindow_t qt_meta_stringdata_MainWindow = {
    {
QT_MOC_LITERAL(0, 0, 10), // "MainWindow"
QT_MOC_LITERAL(1, 11, 21), // "on_bt_refresh_clicked"
QT_MOC_LITERAL(2, 33, 0), // ""
QT_MOC_LITERAL(3, 34, 21), // "on_bt_connect_clicked"
QT_MOC_LITERAL(4, 56, 24), // "on_bt_robot_stop_clicked"
QT_MOC_LITERAL(5, 81, 24), // "on_bt_scan_limit_clicked"
QT_MOC_LITERAL(6, 106, 18), // "on_bt_home_clicked"
QT_MOC_LITERAL(7, 125, 18), // "on_bt_movL_clicked"
QT_MOC_LITERAL(8, 144, 27), // "on_bt_model_setting_clicked"
QT_MOC_LITERAL(9, 172, 23), // "on_bt_on_magnet_clicked"
QT_MOC_LITERAL(10, 196, 24), // "on_bt_off_magnet_clicked"
QT_MOC_LITERAL(11, 221, 27), // "on_bt_read_position_clicked"
QT_MOC_LITERAL(12, 249, 16), // "on_bt_keycommand"
QT_MOC_LITERAL(13, 266, 23), // "on_bt_key_setsp_clicked"
QT_MOC_LITERAL(14, 290, 24), // "on_bt_set_method_clicked"
QT_MOC_LITERAL(15, 315, 27), // "on_bt_clear_console_clicked"
QT_MOC_LITERAL(16, 343, 18), // "on_testing_clicked"
QT_MOC_LITERAL(17, 362, 12), // "on_bt_testmt"
QT_MOC_LITERAL(18, 375, 25), // "on_bt_conveyor_sp_clicked"
QT_MOC_LITERAL(19, 401, 20), // "on_bt_browse_clicked"
QT_MOC_LITERAL(20, 422, 21), // "on_bt_process_clicked"
QT_MOC_LITERAL(21, 444, 25), // "on_bt_gcode_start_clicked"
QT_MOC_LITERAL(22, 470, 24), // "on_bt_gcode_stop_clicked"
QT_MOC_LITERAL(23, 495, 25), // "on_bt_gcode_pause_clicked"
QT_MOC_LITERAL(24, 521, 26), // "on_bt_gcode_resume_clicked"
QT_MOC_LITERAL(25, 548, 29), // "on_bt_gcode_configure_clicked"
QT_MOC_LITERAL(26, 578, 19), // "on_bt_movC1_clicked"
QT_MOC_LITERAL(27, 598, 19), // "on_bt_movC2_clicked"
QT_MOC_LITERAL(28, 618, 21), // "on_bt_sw_test_clicked"
QT_MOC_LITERAL(29, 640, 32), // "on_cb_enable_smooth_stateChanged"
QT_MOC_LITERAL(30, 673, 4), // "arg1"
QT_MOC_LITERAL(31, 678, 23), // "on_VisionButton_clicked"
QT_MOC_LITERAL(32, 702, 17), // "received_callback"
QT_MOC_LITERAL(33, 720, 8), // "log_data"
QT_MOC_LITERAL(34, 729, 13), // "display_event"
QT_MOC_LITERAL(35, 743, 14), // "Display_packet"
QT_MOC_LITERAL(36, 758, 4) // "data"

    },
    "MainWindow\0on_bt_refresh_clicked\0\0"
    "on_bt_connect_clicked\0on_bt_robot_stop_clicked\0"
    "on_bt_scan_limit_clicked\0on_bt_home_clicked\0"
    "on_bt_movL_clicked\0on_bt_model_setting_clicked\0"
    "on_bt_on_magnet_clicked\0"
    "on_bt_off_magnet_clicked\0"
    "on_bt_read_position_clicked\0"
    "on_bt_keycommand\0on_bt_key_setsp_clicked\0"
    "on_bt_set_method_clicked\0"
    "on_bt_clear_console_clicked\0"
    "on_testing_clicked\0on_bt_testmt\0"
    "on_bt_conveyor_sp_clicked\0"
    "on_bt_browse_clicked\0on_bt_process_clicked\0"
    "on_bt_gcode_start_clicked\0"
    "on_bt_gcode_stop_clicked\0"
    "on_bt_gcode_pause_clicked\0"
    "on_bt_gcode_resume_clicked\0"
    "on_bt_gcode_configure_clicked\0"
    "on_bt_movC1_clicked\0on_bt_movC2_clicked\0"
    "on_bt_sw_test_clicked\0"
    "on_cb_enable_smooth_stateChanged\0arg1\0"
    "on_VisionButton_clicked\0received_callback\0"
    "log_data\0display_event\0Display_packet\0"
    "data"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainWindow[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      31,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,  169,    2, 0x08 /* Private */,
       3,    0,  170,    2, 0x08 /* Private */,
       4,    0,  171,    2, 0x08 /* Private */,
       5,    0,  172,    2, 0x08 /* Private */,
       6,    0,  173,    2, 0x08 /* Private */,
       7,    0,  174,    2, 0x08 /* Private */,
       8,    0,  175,    2, 0x08 /* Private */,
       9,    0,  176,    2, 0x08 /* Private */,
      10,    0,  177,    2, 0x08 /* Private */,
      11,    0,  178,    2, 0x08 /* Private */,
      12,    0,  179,    2, 0x08 /* Private */,
      13,    0,  180,    2, 0x08 /* Private */,
      14,    0,  181,    2, 0x08 /* Private */,
      15,    0,  182,    2, 0x08 /* Private */,
      16,    0,  183,    2, 0x08 /* Private */,
      17,    0,  184,    2, 0x08 /* Private */,
      18,    0,  185,    2, 0x08 /* Private */,
      19,    0,  186,    2, 0x08 /* Private */,
      20,    0,  187,    2, 0x08 /* Private */,
      21,    0,  188,    2, 0x08 /* Private */,
      22,    0,  189,    2, 0x08 /* Private */,
      23,    0,  190,    2, 0x08 /* Private */,
      24,    0,  191,    2, 0x08 /* Private */,
      25,    0,  192,    2, 0x08 /* Private */,
      26,    0,  193,    2, 0x08 /* Private */,
      27,    0,  194,    2, 0x08 /* Private */,
      28,    0,  195,    2, 0x08 /* Private */,
      29,    1,  196,    2, 0x08 /* Private */,
      31,    0,  199,    2, 0x08 /* Private */,
      32,    1,  200,    2, 0x0a /* Public */,
      34,    1,  203,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   30,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QByteArray,   33,
    QMetaType::Void, 0x80000000 | 35,   36,

       0        // eod
};

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MainWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->on_bt_refresh_clicked(); break;
        case 1: _t->on_bt_connect_clicked(); break;
        case 2: _t->on_bt_robot_stop_clicked(); break;
        case 3: _t->on_bt_scan_limit_clicked(); break;
        case 4: _t->on_bt_home_clicked(); break;
        case 5: _t->on_bt_movL_clicked(); break;
        case 6: _t->on_bt_model_setting_clicked(); break;
        case 7: _t->on_bt_on_magnet_clicked(); break;
        case 8: _t->on_bt_off_magnet_clicked(); break;
        case 9: _t->on_bt_read_position_clicked(); break;
        case 10: _t->on_bt_keycommand(); break;
        case 11: _t->on_bt_key_setsp_clicked(); break;
        case 12: _t->on_bt_set_method_clicked(); break;
        case 13: _t->on_bt_clear_console_clicked(); break;
        case 14: _t->on_testing_clicked(); break;
        case 15: _t->on_bt_testmt(); break;
        case 16: _t->on_bt_conveyor_sp_clicked(); break;
        case 17: _t->on_bt_browse_clicked(); break;
        case 18: _t->on_bt_process_clicked(); break;
        case 19: _t->on_bt_gcode_start_clicked(); break;
        case 20: _t->on_bt_gcode_stop_clicked(); break;
        case 21: _t->on_bt_gcode_pause_clicked(); break;
        case 22: _t->on_bt_gcode_resume_clicked(); break;
        case 23: _t->on_bt_gcode_configure_clicked(); break;
        case 24: _t->on_bt_movC1_clicked(); break;
        case 25: _t->on_bt_movC2_clicked(); break;
        case 26: _t->on_bt_sw_test_clicked(); break;
        case 27: _t->on_cb_enable_smooth_stateChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 28: _t->on_VisionButton_clicked(); break;
        case 29: _t->received_callback((*reinterpret_cast< QByteArray(*)>(_a[1]))); break;
        case 30: _t->display_event((*reinterpret_cast< Display_packet(*)>(_a[1]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject MainWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_MainWindow.data,
    qt_meta_data_MainWindow,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 31)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 31;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 31)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 31;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
