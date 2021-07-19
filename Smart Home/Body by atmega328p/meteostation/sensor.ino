void readSensors() {
  float pressure1 = bme.readPressure(); 
  dispTemp = bme.readTemperature();
  dispHum = bme.readHumidity();
  dispPres = pressureToMmHg(pressure1);
  outTemp = dht.readTemperature();
  outHum = dht.readHumidity();

    
   // тут делаем линейную аппроксимацию для предсказания погоды
    long averPress = 0;
    for (byte i = 0; i < 10; i++) {
      bme.oneMeasurement();
      averPress += bme.readPressure();
      delay(1);
    }
    averPress /= 10;

    for (byte i = 0; i < 5; i++) {                   // счётчик от 0 до 5 (да, до 5. Так как 4 меньше 5)
      pressure_array[i] = pressure_array[i + 1];     // сдвинуть массив давлений КРОМЕ ПОСЛЕДНЕЙ ЯЧЕЙКИ на шаг назад
    }
    pressure_array[5] = averPress;                    // последний элемент массива теперь - новое давление
    sumX = 0;
    sumY = 0;
    sumX2 = 0;
    sumXY = 0;
    for (int i = 0; i < 6; i++) {                    // для всех элементов массива
      sumX += time_array[i];
      sumY += (long)pressure_array[i];
      sumX2 += time_array[i] * time_array[i];
      sumXY += (long)time_array[i] * pressure_array[i];
    }
    a = 0;
    a = (long)6 * sumXY;             // расчёт коэффициента наклона приямой
    a = a - (long)sumX * sumY;
    a = (float)a / (6 * sumX2 - sumX * sumX);
    delta = a * 6;      // расчёт изменения давления
    dispRain = map(delta, -250, 250, 100, -100);  // пересчитать в проценты
}
void drawSensors() {
  lcd.setCursor(0, 2);
  lcd.print(String(dispTemp, 1));
  lcd.write(223);
  lcd.setCursor(5, 2);
  lcd.print("/" + String(outTemp, 1));
  lcd.write(223);
  lcd.setCursor(12, 2);
  lcd.print(String(dispHum, 0) + "%  ");
  lcd.setCursor(15, 2);
  lcd.print("/" + String(outHum, 0) + "%");
  lcd.setCursor(0, 3);
  lcd.print(int(dispPres));
  lcd.setCursor(4, 3);
  lcd.print("mm");
  lcd.setCursor(12, 3);
  lcd.print("rn/sw " + String(dispRain) + "%");
}
