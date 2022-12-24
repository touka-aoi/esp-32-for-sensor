#include <Arduino.h>

#ifndef SERIALLOGGER_H
#define SERIALLOGGER_H

#ifndef SERIAL_LOGGER_BAUD_RATE
#define SERIAL_LOGGER_BAUD_RATE 9600
#endif

class SerialLogger
{
  public: 
    SerialLogger(); // コンストラクタ
    void Info(String message); // Info
    void Error(String message); // Error

};

extern SerialLogger Logger;

#endif // SERIALLOGGER_H
