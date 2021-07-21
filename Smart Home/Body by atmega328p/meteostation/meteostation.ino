//===============================НАСТРОЙКИ===============================================================
#define BTN1PIN 4  //кнопка вниз
#define BTN2PIN 3  //кнопка вверх
#define BTN3PIN 5                                        //кнопка ввод
#define DHTPIN 2
#define RESET_CLOCK 0                                    //сброс часов при компиляции
#define DHTTYPE DHT21
#define channel 5                                      // номер канала (должен совпадать с приёмником)

// УРОВЕНЬ МОЩНОСТИ ПЕРЕДАТЧИКА
// На выбор RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX
#define SIG_POWER RF24_PA_MAX
// СКОРОСТЬ ОБМЕНА
// На выбор RF24_2MBPS, RF24_1MBPS, RF24_250KBPS
// должна быть одинакова на приёмнике и передатчике!
// при самой низкой скорости имеем самую высокую чувствительность и дальность!!
// ВНИМАНИЕ!!! enableAckPayload НЕ РАБОТАЕТ НА СКОРОСТИ 250 kbps!
#define SIG_SPEED RF24_1MBPS
//-------------------------------Библтотеки-------------------------------------------------------------
#include "GyverButton.h"
GButton buttonUP(BTN1PIN);
GButton buttonDOWN(BTN2PIN);
GButton buttonENTER(BTN3PIN);

#include <microLiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 20, 4);

#include <microDS3231.h>
MicroDS3231 rtc;

#include <GyverBME280.h>
GyverBME280 bme;

#include "DHT.h"
DHT dht(DHTPIN, DHTTYPE);

#include "GyverTimer.h"
GTimer_ms hourT(3600000);                                  //таймер на час
GTimer_ms dayT(3600000 * 24);                              //таймер на сутки

#include <SPI.h>                                           // Подключаем библиотеку для работы с шиной SPI
#include <nRF24L01.h>                                      // Подключаем файл настроек из библиотеки RF24
#include "RF24.h" 
RF24 radio(9, 10);                                         // "создать" модуль на пинах 9 и 10 для НАНО/УНО
//--------------------------------Переменные------------------------------------------------------------
float dispTemp;
float dispHum;
float outTemp;
float outHum;
float wind = 0.0;
double dispPres;
int dispRain;
unsigned long time123, timer12;
float a, b;
unsigned long Pressure1, aver_pressure, pressure_array[6], time_array[6];
boolean wake_flag, move_arrow;
int hrs, mins, secs;
int mode = 0;
int delta;
uint32_t sumX, sumY, sumX2, sumXY;
int x = 0;            //переменные для очистки экрана
int y = 0;
int8_t arrowPos = 0;  // позиция стрелки
int plot_array[20];
float hourTempIn;
float hourTempOut;
float dayTempIn;
float dayTempOut;
byte transmit_data[3]; // массив, хранящий передаваемые данные
byte latest_data[3];   // массив, хранящий последние переданные данные
//------------------------------------Массивы------------------------------------------------------------
// цифры
uint8_t LT[8] = { 0b00111, 0b01111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111 };
uint8_t UB[8] = { 0b11111, 0b11111, 0b11111, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000 };
uint8_t RT[8] = { 0b11100, 0b11110, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111 };
uint8_t LL[8] = { 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b01111, 0b00111 };
uint8_t LB[8] = { 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b11111, 0b11111, 0b11111 };
uint8_t LR[8] = { 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11110, 0b11100 };
uint8_t UMB[8] = { 0b11111, 0b11111, 0b11111, 0b00000, 0b00000, 0b00000, 0b11111, 0b11111 };
uint8_t LMB[8] = { 0b11111, 0b00000, 0b00000, 0b00000, 0b00000, 0b11111, 0b11111, 0b11111 };
//график
byte row8[8] = { 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111 };
byte row7[8] = { 0b00000, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111 };
byte row6[8] = { 0b00000, 0b00000, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111 };
byte row5[8] = { 0b00000, 0b00000, 0b00000, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111 };
byte row4[8] = { 0b00000, 0b00000, 0b00000, 0b00000, 0b11111, 0b11111, 0b11111, 0b11111 };
byte row3[8] = { 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b11111, 0b11111, 0b11111 };
byte row2[8] = { 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b11111, 0b11111 };
byte row1[8] = { 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b11111 };
//=======================================================================================================
void setup() {
  Serial.begin(9600);
//-------------------------------------Инициализации-датчиков---------------------------------------------
  dht.begin();  //инициализация датчика температуры

  lcd.init();  // инициализация дисплея
  lcd.backlight();
  lcd.clear();

  bme.begin();  //инициализация датчика температуры

  rtc.setTime(COMPILE_TIME);  //настройки времени
  mins = rtc.getMinutes();
  hrs = rtc.getHours();

  radioSetup();
//-----------------------------------Переменные-для-графиков-----------------------------------------------
  hourTempIn = dispTemp;
  hourTempOut = outTemp;
  dayTempIn = dispTemp;
  dayTempOut = outTemp;
}
void loop() { 
  Radio();

  if (buttonENTER.isClick()) lcd.clear();
  if (millis() - time123 >= 60516) {
    time123 = millis();
    lcd.clear();
  }
//---------------------------------------Переменные-времени-------------------------------------------------
  mins = rtc.getMinutes();
  hrs = rtc.getHours();
//----------------------------------------Кнопки------------------------------------------------------------
  buttonUP.tick();
  buttonDOWN.tick();
  buttonENTER.tick();
  if (buttonENTER.isClick()) {
    mode = 1;
    y = 0;
  }
  if (buttonENTER.isDouble()) {
    mode = 0;
    x = 0;
  }
//-----------------------------------------Главный-экран---------------------------------------------------
  if (mode == 0) {
    loadClock();
    if (millis() - time123 >= 59000) {
      time123 = millis();
      lcd.clear();
    }
    if (y < 1) {
      y++;
      lcd.clear();
    }
    readSensors();
    drawSensors();
    drawData();
    drawClock(hrs, mins, 0, 0, 1);
    lcd.setCursor(7, 1);
    lcd.write(165);
    lcd.setCursor(7, 0);
    lcd.write(165);
  }
//------------------------------------------Меню-----------------------------------------------------------
  if (mode == 1) {
    menu();
  }
//------------------------------------------Обновление-переменных-для-графиков-----------------------------
  if (hourT.isReady()) hourTempIn = dispTemp;
  if (hourT.isReady()) hourTempOut = outTemp;
  if (dayT.isReady()) dayTempIn = dispTemp;
  if (dayT.isReady()) dayTempOut = outTemp;
}
