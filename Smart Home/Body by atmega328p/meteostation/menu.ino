void menu(){
  while (x<1){
    x++;
    lcd.clear();
             }
   if (mode==1)  printGUI1(); 
   if (mode==1 && buttonDOWN.isClick()){
   lcd.clear();
   arrowPos++;
   if (arrowPos <= 3){
   printGUI1(); 
    }
   if (arrowPos >=4){
    lcd.clear();
    printGUI2();
    if(arrowPos >= 7) arrowPos=7;
    }
   }
   
   if(mode==1 && buttonUP.isClick()){
     lcd.clear();
     arrowPos--;
     if (arrowPos < 0 ) arrowPos = 0;
       printGUI1(); 
     }

   if(buttonENTER.isHold() && mode==1){
       initPlot();  
       mode=2;
       lcd.clear();
       
       switch (arrowPos) {
    case 0:  
         drawPlot(0, 3, 12, 4, 10, 40, int(hourTempIn));
      break;
    case 1:  
          drawPlot(0, 3, 12, 4, 10, 40, int(dayTempIn));
      break;
    case 2: 
          drawPlot(0, 3, 12, 4, -43, 40, int(hourTempOut));
      break;
    case 3: 
          drawPlot(0, 3, 12, 4, -43, 40, int(dayTempOut));
      break;
     } 
  }
}
void printGUI1(){
  lcd.setCursor(0, 0); lcd.print("temp_graf_h_in"); 
  lcd.setCursor(0, 1); lcd.print("temp_graf_d_in"); 
  lcd.setCursor(0, 2); lcd.print("temp_graf_h_out"); 
  lcd.setCursor(0, 3); lcd.print("temp_graf_d_out"); 

 switch (arrowPos) {
    case 0: lcd.setCursor(16, 0);
      break;
    case 1: lcd.setCursor(16, 1);
      break;
    case 2: lcd.setCursor(16, 2);
      break;
    case 3: lcd.setCursor(16, 3);
      break;
  }
  lcd.write(127);   // вывести стрелку
}
void printGUI2(){
  lcd.setCursor(0, 0); lcd.print("temp_graf_h_in"); 
  lcd.setCursor(0, 1); lcd.print("temp_graf_d_in"); 
  lcd.setCursor(0, 2); lcd.print("temp_graf_h_out"); 
  lcd.setCursor(0, 3); lcd.print("temp_graf_d_out"); 

 switch (arrowPos) {
    case 4: lcd.setCursor(16, 0);
      break;
    case 5: lcd.setCursor(16, 1);
      break;
    case 6: lcd.setCursor(16, 2);
      break;
    case 7: lcd.setCursor(16, 3);
      break;
  }
  lcd.write(127);   // вывести стрелку
}
