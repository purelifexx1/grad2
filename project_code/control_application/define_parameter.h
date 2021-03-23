#ifndef DEFINE_PARAMETER_H
#define DEFINE_PARAMETER_H

#include <QtCore>
#define START_CHAR         0x28
#define RECEIVE_END        "})"
#define SCARA_FOWARD_SCALE 1000000
#define SCARA_INVERSE_SCALE 0.000001f
#define SHIFT_KEY_MAX       14
#define SHIFT_KEY_MIN       1
#define MINIMUM_LINEAR_TIME_FRAME  0.03
#define MINIMUM_CIRCLE_TIME_FRAME  0.05
#define ADD_VALUE(array, value, type) system_parameter->Convert_And_Append(array, value, type)
typedef enum
{
    DISPLAY_POSITION,
    DISPLAY_RPD_DETAIL,
    DISPLAY_GCODE_PROCESS,
}display_id ;

typedef enum
{
    MAGNET_ON,
    MAGNET_OFF,
    WRONG_VALUE_FOR_OUTPUT_LEVEL
}Response_Detail_code;

typedef enum
{
      SCARA_METHOD_MANUAL					= 0x00U,  /*!< Control by joy stick */
      SCARA_METHOD_SEMI_AUTO				= 0x01U,  /*!< Control by single command: MOVJ, MOVL, MOVC   */
      SCARA_METHOD_GCODE					= 0x02U,   /*!< Control by job file  */
      SCARA_METHOD_TEST                     = 0x03U,
      SCARA_METHOD_PICK_AND_PLACE           = 0x04U
}SCARA_MethodTypeDef;

typedef enum
{
    SCARA_TEST_MOTOR1_POS,
    SCARA_TEST_MOTOR1_NEG,
    SCARA_TEST_MOTOR2_POS,
    SCARA_TEST_MOTOR2_NEG,
    SCARA_TEST_MOTOR3_POS,
    SCARA_TEST_MOTOR3_NEG,
    SCARA_TEST_MOTOR4_POS,
    SCARA_TEST_MOTOR4_NEG,
    SCARA_TEST_MOTOR_STOP,
}SCARA_TestMode;
typedef enum
{
    ACCEPT_COMMAND,
    STUPID_CODE,
    WRONG_SPACE_TYPE,
    WRONG_TASK_TYPE,
    WRONG_JOINT_TYPE,
    WRONG_TRAJECTORY_TYPE,
    WRONG_PARAMETER,
    OVER_WORKSPACE,
    WRONG_MODE_INIT,
    OVER_VELOCITY,
    OVER_ACCELERATE,
    WRONG_JOINT_NUM,
    WRONG_COORDINATE,
    OUTPUT_ON,
    OUTPUT_OFF,
    WRONG_OUTPUT_VALUE,
    WRONG_READ_POSITION_TYPE,
    TEST_VALUE_SETTING,
    RELATIVE,
    LSPB,
    S_CURVE,
    CHECK_PARAMETER,
    MANUAL_SPEED,
    UNKNOW_COMMAND,
    MANUAL_METHOD   ,
    SEMI_AUTO_METHOD,
    AUTO_METHOD     ,
    TEST_METHOD     ,
    PICK_AND_PLACE_METHOD,
    OBJECT_DETECTED ,
    GCODE_TRANSFER_FINISH,
    GCODE_OFFSET_CONFIGURE,
    STOP_NOW        ,
    START_SCAN      ,
    BUSY            ,
    NOT_SCAN        ,
    INCORRECT_METHOD,

    NONE            ,
    NUM_OF_STATUS
}Response_ID;

typedef enum
{
    RPD_IDLE	,
    RPD_BUSY	,
    RPD_POSITION,
    RPD_START	,
    RPD_RUNNING	,
    RPD_DONE	,
    RPD_STOP	,
    RPD_ERROR	,
    RPD_OK 		,
    RPD_DUTY	,
    RDP_TRANSFER,
    RDP_GCODE_PROCESS,
    NUM_OF_RESPOND
}Robot_RespondTypedef;

typedef enum
{
      DUTY_MODE_INIT_QVA				= 0x00U,  /*!< Consume A max, determine T max */
      DUTY_MODE_INIT_QVT				= 0x01U,  /*!< Consume T max, determine A max   */
      DUTY_MODE_INIT_QV                 = 0x02U,
      DUTY_MODE_INIT_QT                 = 0x03U
}ModeInitTypeDef;

typedef enum
{
      DUTY_COORDINATES_ABS				= 0x00U,  /*!< Absolute position */
      DUTY_COORDINATES_REL				= 0x01U  /*!< Relative position*/
}CoordinatesTypeDef;

typedef enum
{
      DUTY_TRAJECTORY_LSPB				= 0x00U,  /*!< Trajectory planning LSBP */
      DUTY_TRAJECTORY_SCURVE			= 0x01U,  /*!< Trajectory planning S-curve */
      DUTY_TRAJECTORY_LINEAR            = 0x02U
}TrajectoryTypeDef;

typedef enum
{
    SCARA_KEY_X_INC,
    SCARA_KEY_X_DEC,
    SCARA_KEY_Y_INC,
    SCARA_KEY_Y_DEC,
    SCARA_KEY_Z_INC,
    SCARA_KEY_Z_DEC,
    SCARA_KEY_ROLL_INC,
    SCARA_KEY_ROLL_DEC,
    SCARA_KEY_VAR0_INC,
    SCARA_KEY_VAR0_DEC,
    SCARA_KEY_VAR1_INC,
    SCARA_KEY_VAR1_DEC,
    SCARA_KEY_VAR2_INC,
    SCARA_KEY_VAR2_DEC,
    SCARA_KEY_VAR3_INC,
    SCARA_KEY_VAR3_DEC,// 16 key board
}SCARA_KeyTypeDef;

typedef enum
{
    CMD_STOPNOW,
    CMD_SCAN_LIMIT,
    CMD_MOVE_HOME,
    CMD_MOVE_LINE,
    CMD_MOVE_CIRCLE,
    CMD_MOVE_JOINT,
    CMD_ROTATE_SINGLE,
    CMD_OUTPUT,
    CMD_READ_STATUS,
    CMD_READ_POSITION,
    CMD_TEST_METHOD_SETTING,
    CMD_METHOD_CHANGE,
    CMD_MOTOR_TEST,

    CMD_GCODE_STOP,
    CMD_GCODE_PAUSE,
    CMD_JOB_PUSH_MOVE_LINE,
    CMD_JOB_PUSH_MOVE_JOINT,
    CMD_GCODE_CONFIGURE,
    CMD_GCODE_RESUME,
    CMD_GCODE_RUN,// 7 job

    CMD_KEYBOARD,// 2 key board
    CMD_KEY_SPEED,

    CMD_ERROR,
    PROTOCOL_ERROR,

    CMD_OBJECT_DETECTED,
    CMD_SETUP_CONVEYOR_SPEED,
    CMD_GCODE,
    NUM_OF_COMMAND_STRING
}Robot_CommandTypedef ;

typedef enum
{
      SCARA_STATUS_OK				   ,
      SCARA_STATUS_ERROR			   ,
      SCARA_STATUS_ERROR_SPACE		   ,
      SCARA_STATUS_ERROR_TASK		   ,
      SCARA_STATUS_ERROR_JOINT		   ,
      SCARA_STATUS_ERROR_TRAJECTORY	   ,
      SCARA_STATUS_ERROR_PARA		   ,
      SCARA_STATUS_ERROR_OVER_WORKSPACE,
      SCARA_STATUS_ERROR_MODE_INIT 	   ,
      SCARA_STATUS_ERROR_OVER_VELOC	   ,
      SCARA_STATUS_ERROR_OVER_ACCEL	   ,
      SCARA_STATUS_ERROR_JOINT_NUM	   ,
      SCARA_STATUS_ERROR_COORDINATE	   ,
      NUM_OF_MAIN_TASK_STATUS
}SCARA_StatusTypeDef;

typedef enum
{
    FILE_TRANSMISION,
    COMMAND_TRANSMISION,
    RESPONSE_TRANSMISION
}Transfer_Protocol;

typedef enum
{
    FLOAT_STRING_VALUE,
    FLOAT_VALUE,
    DOUBLE_STRING_VALUE,
    DOUBLE_VALUE,
    BYTE_VALUE,
    INT16_VALUE,
    INT32_VALUE,
    SCARA_COR_VALUE_TEXT,
    SCARA_COR_VALUE_DOUBLE
}TypeDef_Conversion ;

typedef enum
{
    DISPLAY_ONLY,
    MOVC_TYPE1,
    MOVC_TYPE2
}Coordinate_Receive_Handler_TypeDef;
typedef struct
{
    double x;
    double y;
    double z;
    double roll;
    double theta1;
    double theta2;
    double D3;
    double theta4;
}Scara_Position_RealData;

typedef struct
{
    int32_t raw_x;
    int32_t raw_y;
    int32_t raw_z;
    int32_t raw_roll;
    int32_t raw_theta1;
    int32_t raw_theta2;
    int32_t raw_D3;
    int32_t raw_theta4;
}Scara_Position_RawData;

typedef struct
{
    display_id action_id;
    Scara_Position_RealData RealData;
    Robot_CommandTypedef Command_ID;
    Robot_RespondTypedef Respond_Type;
    QList<Response_ID> Reference_String;
    QByteArray         Contain_Data;
}Display_packet ;

typedef enum
{
    REAL_POSITION_DATA,
    REAL_POSITION_DATA_PLUS_UPDATE,
    ESTIMATE_POSITION_DATA
}Position_DataType;

typedef enum{
    VIETNAM_FLAG,
    JAPAN_FLAG,
    ENGLAND_FLAG,
    SWITZERLAND_FLAG,
    SWEDEN_FLAG,
    GERMANY_FLAG,
    NUM_OF_OBJECT
}ObjectType;

typedef enum{
    GCODE_ERROR_LINE_FORMAT,
    CURRENT_DATA_UNINIT,
    GCODE_PROCESS_ERROR,
    ERROR_WRITE_FILE,
    UNMATCH_Z_HEIGHT,
    GCODE_OK,
    NUM_OF_DTC
}Gcode_Decoder_DTC_TypeDef;

typedef enum{
    ROUTE_COMMAND,
    STATE_LINEAR,
    STATE_CIRCLE
}Gcode_Compress_State_TypeDef;

class define_parameter
{

public:
    QString DETAIL_STATUS[NUM_OF_STATUS]  = {"Accept Command",
                                             "Stupid Code",
                                             "Wrong Space Type",
                                             "Wrong Task Type",
                                             "Wrong Joint Type",
                                             "Wrong Trajectory Type",
                                             "Wrong Parameters",
                                             "Over Workspace",
                                             "Wrong Mode Init",
                                             "Over Velocity",
                                             "Over Accelerate",
                                             "Wrong Joint Num",
                                             "Wrong Coordinate type",
                                             "Output On",
                                             "Output Off",
                                             "Wrong output value",
                                             "Wrong position type requirement",
                                             "Test value setting successfully",
                                             "Relative",
                                             "LSPB",
                                             "S-CURVE",
                                             "Check parameter",
                                             "Manual speed ",
                                             "Unknow command",
                                             "Changed MANUAL Method",
                                             "Changed SEMI AUTO Method",
                                             "Changed AUTO Method",
                                             "Changed TEST Method",
                                             "Changed PICK_AND_PLACE Method",
                                             "Object Detected",
                                             "Gcode transfer process completed",
                                             "Gcode offset data configured succesfully",
                                             "Stop Now",
                                             "Start scan",
                                             "Busy",
                                             "Has not SCAN yet",
                                             "METHOD isn't correct",

                                             ""
                                            };
    QString COMMAND_STRING[NUM_OF_COMMAND_STRING] = {
        "CMD_STOPNOW",
        "CMD_SCAN_LIMIT",
        "CMD_MOVE_HOME",
        "CMD_MOVE_LINE",
        "CMD_MOVE_CIRCLE",
        "CMD_MOVE_JOINT",
        "CMD_ROTATE_SINGLE",
        "CMD_OUTPUT",
        "CMD_READ_STATUS",
        "CMD_READ_POSITION",
        "CMD_TEST_METHOD_SETTING",
        "CMD_METHOD_CHANGE",
        "CMD_MOTOR_TEST",

        "CMD_GCODE_STOP",
        "CMD_GCODE_PAUSE",
        "CMD_JOB_PUSH_MOVE_LINE",
        "CMD_JOB_PUSH_MOVE_JOINT",
        "CMD_GCODE_CONFIGURE",
        "CMD_GCODE_RESUME",
        "CMD_GCODE_RUN",// 7 job

        "CMD_KEYBOARD",// 2 key board
        "CMD_KEY_SPEED",

        "CMD_ERROR",
        "PROTOCOL_ERROR",

        "CMD_OBJECT_DETECTED",
        "CMD_SETUP_CONVEYOR_SPEED",
        "CMD_GCODE",
    };
    QString RDP_String[NUM_OF_RESPOND] = {
        "RPD_IDLE"	  ,
        "RPD_BUSY"	  ,
        "RPD_POSITION",
        "RPD_START"   ,
        "RPD_RUNNING" ,
        "RPD_DONE"	  ,
        "RPD_STOP"	  ,
        "RPD_ERROR"	  ,
        "RPD_OK" 	  ,
        "RPD_TRANSFER",
        "RPD_DUTY"
    };

    define_parameter();
    void Convert_And_Append(QByteArray *object_array, QVariant convert_object, TypeDef_Conversion input_type);
};
extern define_parameter *system_parameter;
#endif // DEFINE_PARAMETER_H
