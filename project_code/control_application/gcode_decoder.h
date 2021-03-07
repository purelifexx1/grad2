#ifndef GCODE_DECODER_H
#define GCODE_DECODER_H

#include <QMainWindow>
#include <QObject>
#include <QWidget>
#include "define_parameter.h"
#include <QFile>
#include "packet_handler.h"

typedef enum
{
    G00,
    G01,
    G02,
    G03,

    NUM_OF_COMMAND
}GCode_TypeDef;

typedef enum
{
    X_COR,
    Y_COR,
    Z_COR,
    I_COR,
    J_COR,
    FEED,
    Value_Check_Typedef_NUM
}Value_Check_Typedef;
typedef struct
{
    double          X = 0;
    double          Y = 0;
    double          Z = 0;
    double          I = 0;
    double          J = 0;
    double          Feed = 0;
    bool            check[Value_Check_Typedef_NUM] = {false, false, false, false, false, false};
    GCode_TypeDef   Command;
}GCode_Coordinate_TypeDef;
typedef enum
{
    FIRST_PACKET,                 //these contains in 4 bit low
    LINEAR_TYPE,   //G00, G01     //these contains in 4 bit low
    ARC_CW_TYPE,   //G02          //these contains in 4 bit low
    ARC_AW_TYPE,   //G03          //these contains in 4 bit low
    UP_Z,                         //these contains in 4 bit high
    DOWN_Z                        //these contains in 4 bit high
}Packet_Command_TypeDef;
class Gcode_Decoder: public QObject
{
    Q_OBJECT
public:
    Gcode_Decoder();
    void Clear_Data();
    void Write_Data_To_File(QString path);
    void Init_Current_Data(double x, double y, double z, double feed);
    void Process_Line(QString Line);
    Gcode_Decoder_DTC_TypeDef package_data();
    Gcode_Decoder_DTC_TypeDef Process_Compress_Gcode_Data();
    double calculate_linear_distance(GCode_Coordinate_TypeDef start, GCode_Coordinate_TypeDef end);
    double calculate_circle_distance(GCode_Coordinate_TypeDef start, GCode_Coordinate_TypeDef end);

    GCode_Coordinate_TypeDef        current_data;
    QList<GCode_Coordinate_TypeDef> raw_data;
    QList<GCode_Coordinate_TypeDef> compact_data;
    QList<QByteArray>               data_packet;
    std::vector<QByteArray>         ert;
private:
    QString Command_String[4] = {"G00", "G01", "G02", "G03"};
    double lastI, lastJ;
};

extern Gcode_Decoder *gcode;

#endif // GCODE_DECODER_H
