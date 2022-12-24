#include <Arduino.h>
#include <DHT.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>


#include "SerialLogger.hpp"
#include "iot_config.hpp"


// IOT_CONFIG
const char* ssid = IOT_CONFIG_WIFI_SSID;
const char* password = IOT_CONFIG_WIFI_PASSWORD;
const char* mqtt_server = IOT_CONFIG_MQTT_ADDRESS;
const int mqtt_port = IOT_CONFIG_MQTT_PORT;

// PORT
#define DELAYTIME 60000 // 30秒に一回

// ESP32 PIN Conifg
#define LED 32
#define DHTPIN 4
#define DHTTYPE DHT11 
#define LIGHTPIN 35

DHT dht(DHTPIN, DHTTYPE); // dhtインスタンスの作成
DynamicJsonDocument doc(1024); // Jsonシリアライズ
WiFiClient esp_client; 
PubSubClient ps_client(esp_client); // MQTTクライアント
struct tm timeInfo; // 時間構造体
char time_char[20]; // 時間格納用文字列

// WIFI接続
void connectToWiFi()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  // 接続まで待機
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }
  Logger.Info("Wifi Connected");
}

void initializeMqttClient()
{
  ps_client.setServer(mqtt_server, mqtt_port);
  // 接続できるまで待機
  ps_client.connect("ESP32Client");
  while (!ps_client.connected()) {
    delay(1000);
  }
  Logger.Info("MQTT Connected");
}

void initializeTime() 
{
  configTime(9 * 3600, 0, "ntp.nict.jp");
}

void initializeSensors() 
{
  pinMode(LED, OUTPUT);
  dht.begin();
}

void establishConnection() 
{
  connectToWiFi();
  initializeTime();
  initializeSensors();
  initializeMqttClient();
}

void setup() {
  Serial.begin(9600);
  establishConnection();
}

void loop() {

  // 接続していない場合接続する
  if (!WiFi.status() != WL_CONNECTED) {
    connectToWiFi();
  }

  if (!ps_client.connected()) {
    initializeMqttClient();
  }

  getLocalTime(&timeInfo);
  sprintf(time_char, "%04d/%02d/%02d %02d:%02d:%02d",
        timeInfo.tm_year + 1900, timeInfo.tm_mon + 1, timeInfo.tm_mday,
        timeInfo.tm_hour, timeInfo.tm_min, timeInfo.tm_sec);
  Logger.Info("Time: " + String(time_char));

  float humidity_value = dht.readHumidity();
  float temperature_value = dht.readTemperature();

  if (isnan(humidity_value) || isnan(temperature_value)) {
    Logger.Error("取得に失敗しました。");
  }

  Logger.Info("Humidity: " + String(humidity_value));
  Logger.Info("Tempreture: " + String(temperature_value));

  float light_value = analogRead(LIGHTPIN);
  Logger.Info("Light: " + String(light_value));


  doc["Light"] = light_value;
  doc["Tempreture"] = temperature_value;
  doc["Humidity"] = humidity_value;
  doc["time"] = time_char;

  char buffer[256];
  serializeJson(doc, buffer);

  ps_client.publish("esp32", buffer);
  
  // 一分に一回回る
  delay(DELAYTIME);
}


