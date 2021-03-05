#include "define_parameter.h"

define_parameter::define_parameter()
{

}

void define_parameter::Convert_And_Append(QByteArray *object_array, QVariant convert_object, TypeDef_Conversion input_type)
{
    switch (input_type) {
        case FLOAT_STRING_VALUE:
        {
            QString temp = convert_object.toString();
            float number = temp.toFloat();
            object_array->append(reinterpret_cast<const char*>(&number), sizeof(number));
        }
        break;
        case FLOAT_VALUE:
        {
            float number = convert_object.toFloat();
            object_array->append(reinterpret_cast<const char*>(&number), sizeof(number));
        }
        break;
        case DOUBLE_STRING_VALUE:
        {
            QString temp = convert_object.toString();
            double number = temp.toDouble();
            object_array->append(reinterpret_cast<const char*>(&number), sizeof(number));
        }
        break;
        case DOUBLE_VALUE:
        {
            double number = convert_object.toDouble();
            object_array->append(reinterpret_cast<const char*>(&number), sizeof(number));
        }
        break;
        case BYTE_VALUE:
        {
            QChar number = convert_object.toChar();
            object_array->append(reinterpret_cast<const char*>(&number), 1);
        }
        break;
        case INT16_VALUE:
        {
            int16_t number = convert_object.toInt();
            object_array->append(reinterpret_cast<const char*>(&number), sizeof(number));
        }
        break;
        case SCARA_COR_VALUE_TEXT:
        {
            QString temp = convert_object.toString();
            int32_t number = (int32_t)(temp.toDouble()*SCARA_FOWARD_SCALE);
            object_array->append(reinterpret_cast<const char*>(&number), sizeof(number));
        }
        break;
        case SCARA_COR_VALUE_DOUBLE:
        {
            int32_t number = (int32_t)(convert_object.toDouble()*SCARA_FOWARD_SCALE);
            object_array->append(reinterpret_cast<const char*>(&number), sizeof(number));
        }
        break;
    }
}
define_parameter *system_parameter = new define_parameter();
