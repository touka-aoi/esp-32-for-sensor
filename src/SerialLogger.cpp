#include "SerialLogger.hpp"

// コンストラクタ
// シリアル通信を開始する
SerialLogger::SerialLogger() { Serial.begin(SERIAL_LOGGER_BAUD_RATE); }

// Infoレベルのロギング
void SerialLogger::Info(String message)
{
  Serial.print(" [INFO] ");
  Serial.println(message);
}

// Errorレベルのロギング
void SerialLogger::Error(String message)
{
  Serial.print(" [ERROR] ");
  Serial.println(message);
}

SerialLogger Logger;