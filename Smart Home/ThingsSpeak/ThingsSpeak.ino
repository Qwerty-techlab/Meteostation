#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Hash.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <string.h>

String api_key = "4OC07NL1CWY163Z2"; //  Введите свой ключ API записи из ThingSpeak
const char *ssid = "test";           // замените на ваш Wi-Fi ssid и ключ wpa2
const char *pass = "test1111";       // замените на ваш пароль Wi-Fi
const char *server = "api.thingspeak.com";

const char *PARAM_INPUT_1 = "state";
float temperature = 0.0;
float temperatureout = 0.0;
float humidity = 0.0;
float humidityout = 0.0;
float wind = 0.0;
float pressure = 0.0;
float transmit_data[7];
int ledState = LOW;
int timer = 0;
const int output = 2;

#define channel 5
#define RF_SPEED RF24_1MBPS // Указываем скорость передачи данных (RF24_250KBPS, RF24_1MBPS, RF24_2MBPS), RF24_1MBPS - 1Мбит/сек
#define RF_PA RF24_PA_MAX   // Указываем мощность передатчика (RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_HIGH=-6dBm, RF24_PA_MAX=0dBm)
RF24 radio(4, 15);

WiFiClient client;

void setup()
{
  SPI.setHwCs(true);
  SPI.begin();
  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(MSBFIRST);
  radio.begin(); // Инициируем работу nRF24L01+
  radio.setChannel(channel);
  radio.setDataRate(RF_SPEED);
  radio.setPALevel(RF_PA);
  radio.openReadingPipe(1, 0x1234567890LL); // Открываем 1 трубу с идентификатором 0x1234567890 для приема данных
                                            //(на ожном канале может быть открыто до 6 разных труб, которые должны
                                            //отличаться только последним байтом идентификатора)
  radio.startListening();                   // Включаем приемник, начинаем прослушивать открытую трубу

  Serial.begin(115200);
  delay(10);
  Serial.println("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
}

void loop()
{

  if (radio.available())
  {
    radio.read(&transmit_data, sizeof(transmit_data));
    //---------------------------Принятие-данных-с-антенны-----------------------------------------------------------------------------------------------
    temperature = transmit_data[0];
    humidity = transmit_data[1];
    pressure = transmit_data[2];
    temperatureout = transmit_data[3];
    humidityout = transmit_data[4];
    wind = transmit_data[5];
  }

  if (millis() - timer <= 10000)
  {
    timer = millis();
    if (client.connect(server, 80)) //   "184.106.153.149" или api.thingspeak.com
    {
      String data_to_send = api_key;
      data_to_send += "&field1=";
      data_to_send += temperature;
      data_to_send += "&field2=";
      data_to_send += temperatureout;
      data_to_send += "&field3=";
      data_to_send += pressure;
      data_to_send += "&field4=";
      data_to_send += humidity;
      data_to_send += "&field5=";
      data_to_send += humidityout;
      data_to_send += "\r\n\r\n";

      client.print("POST /update HTTP/1.1\n");
      client.print("Host: api.thingspeak.com\n");
      client.print("Connection: close\n");
      client.print("X-THINGSPEAKAPIKEY: " + api_key + "\n");
      client.print("Content-Type: application/x-www-form-urlencoded\n");
      client.print("Content-Length: ");
      client.print(data_to_send.length());
      client.print("\n\n");
      client.print(data_to_send);

      Serial.println("%. Send to Thingspeak.");
    }
    client.stop();
    delay(1000);
    Serial.println("Waiting...");
  }
}