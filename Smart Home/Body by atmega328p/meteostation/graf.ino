void initPlot() {
  lcd.createChar(0, row8);
  lcd.createChar(1, row1);
  lcd.createChar(2, row2);
  lcd.createChar(3, row3);
  lcd.createChar(4, row4);
  lcd.createChar(5, row5);
  lcd.createChar(6, row6);
  lcd.createChar(7, row7);
}

void drawPlot(byte pos, byte row, byte width, byte height, int min_val, int max_val, int fill_val) { 
  for (byte i = 0; i < width; i++) {
    plot_array[i] = plot_array[i + 1];
  }
  fill_val = constrain(fill_val, min_val, max_val);
  plot_array[width] = fill_val;
  for (byte i = 0; i < width; i++) {              // каждый столбец параметров
    byte infill, fract;
    // найти количество целых блоков с учётом минимума и максимума для отображения на графике
    infill = floor((float)(plot_array[i] - min_val) / (max_val - min_val) * height * 10);
    fract = (infill % 10) * 8 / 10;               // найти количество оставшихся полосок
    infill = infill / 10;
    for (byte n = 0; n < height; n++) {           // для всех строк графика
      if (n < infill && infill > 0) {             // пока мы ниже уровня
        lcd.setCursor(pos + i, (row - n));        // заполняем полными ячейками
        lcd.write(0);
      }
      if (n >= infill) {                          // если достигли уровня
        lcd.setCursor(pos + i, (row - n));
        if (fract > 0) lcd.write(fract);          // заполняем дробные ячейки
        else lcd.write(16);                       // если дробные == 0, заливаем пустой
        for (byte k = n + 1; k < height; k++) {   // всё что сверху заливаем пустыми
          lcd.setCursor(pos + i, (row - k));
          lcd.write(16);
        }
        break;
      }
    }
  }
  int max_value = -32000;
  int min_value = 32000;
  for (byte i = 0; i < 12; i++) { if (plot_array[i] > max_value) max_value = plot_array[i];
    if (plot_array[i] < min_value) min_value = plot_array[i];
  }
  lcd.setCursor(17, 1);
  lcd.print(fill_val); lcd.print(" ");
  lcd.setCursor(13, 0);
  lcd.print("max "); lcd.print(max_value); lcd.print(" ");
  lcd.setCursor(13, 3);
  lcd.print("min "); lcd.print(min_value); lcd.print(" ");
}
