//------------------Библиотеки-------------------------------------------------------------------------------------------------
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Hash.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
//------------------Название-сети-и-пароль--------------------------------------------------------------------------------------
const char* ssid     = "Meteostation";
const char* password = "$|9U|X";
//------------------Переменные--------------------------------------------------------------------------------------------------
#define DHTPIN 4             // Подключение Датчика dht11 к ESP
#define DHTTYPE    DHT21     // Выбор датчика: DHT 21 (AM2301), DHT 22 (AM2302)

DHT dht(DHTPIN, DHTTYPE);

float t = 0.0;
float h = 0.0;
float u = millis();
// Создаём AsyncWebServer object на 80 порту
AsyncWebServer server(80);
unsigned long previousMillis = 0;    // Время последнего обновления датчика DHT
const long interval = 10000;        // Обновлять показания датчика DHT каждые 10 секунд  

// =================================HTML=страницы==============================================================================
//EXAMPLE
/*const char #NAME#[] PROGMEM = R"rawliteral(
  <!DOCTYPE html>
   <html>
     <head></head>
     <bode></body>
   </html>)rawliteral";  
*/
//----------------------------------Index---------------------------------------------------------------------------------------
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="ru">
<head>
    <meta http-equiv = "content-type" content = "text/html"; charset = "UTF-8"/>
    <title>Главная страница</title>
    <style>
        .button {
            display: inline-block; /* Строчно-блочный элемент */
            padding: 5px 20px; /* Добавляем поля */
            text-decoration: none; /* Убираем подчёркивание у ссылки */
            cursor: pointer; /* Курсор в виде руки */
            background: #deefff; /* Фон для браузеров, не поддерживающих градиент */
            /* Градиент */
            background: -moz-linear-gradient(top, #deefff 0%, #98bede 100%);
            background: -webkit-gradient(linear, left top, left bottom, color-stop(0%,#deefff), color-stop(100%,#98bede));
            background: -webkit-linear-gradient(top, #deefff 0%,#98bede 100%);
            background: -o-linear-gradient(top, #deefff 0%,#98bede 100%);
            background: -ms-linear-gradient(top, #deefff 0%,#98bede 100%);
            background: linear-gradient(top, #deefff 0%,#98bede 100%);
            border-radius: 5px; /* Скругляем уголки */
            border: 1px solid #008; /* Добавляем синюю рамку */
            font: 60px/1 Arial, sans-serif; /* Рубленый шрифт */
            color: #2c539e; /* Цвет текста и ссылки */
        }
        header {
            margin: 0 auto;
            text-align: center;
        }
        nav {
            display: flex;
            padding: 0 5px;
            flex-direction: row;
            display: flex;
            justify-content: center;
            align-content: space-between;
            align-items: center;
        }
        .item {
            margin: 0 5px;
        }
        article {
            font-size: 42px;
            display: inline-block;
            margin: 0px auto;
            text-align: left;
        }
    </style>
</head>
<body>
    <header>
        <p><a href="/" class="button">Главная</a>
    </header>
    <nav>
        <p class="item"><a href="/home" class="button">Дом</a>
        <p class="item"><a href="/outside" class="button">Улица</a>
        <p class="item"><a href="/relay" class="button">Реле</a>
    </nav>
    <article>
        <div>
            <p>
                <span class="dht-labels">Температура в доме</span>
                <span id="temperature">%TEMPERATURE%</span>
                <sup class="units">&deg;C</sup>
            </p>
            <p>
                <span class="dht-labels">Температура на улице</span>
                <span id="temperatureout">%TEMPERATUREOUT%</span>
                <sup class="units">&deg;C</sup>
            </p>
        </div>
        <div>
            <p>
                <span>uptime</span>
                <span id="uptime">%UPTIME%</span>
            </p>
        </div>
    </article>
</body>
<script>
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("temperature").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/temperature", true);
  xhttp.send();
}, 10000);

setInterval(function () {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("temperatureout").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/temperatureout", true);
  xhttp.send();
}, 10000);

setInterval(function () {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("uptime").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/uptime", true);
  xhttp.send();
}, 10000 ) ;
</script>
</html>)rawliteral";
//--------------------------Home------------------------------------------------------------------
const char Home_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="ru">
<head>
    <meta http-equiv = "content-type" content = "text/html; charset = UTF-8" />
    <title>Погода в доме</title>
    <style>
        .button {
            display: inline-block; /* Строчно-блочный элемент */
            padding: 5px 20px; /* Добавляем поля */
            text-decoration: none; /* Убираем подчёркивание у ссылки */
            cursor: pointer; /* Курсор в виде руки */
            background: #deefff; /* Фон для браузеров, не поддерживающих градиент */
            /* Градиент */
            background: -moz-linear-gradient(top, #deefff 0%, #98bede 100%);
            background: -webkit-gradient(linear, left top, left bottom, color-stop(0%,#deefff), color-stop(100%,#98bede));
            background: -webkit-linear-gradient(top, #deefff 0%,#98bede 100%);
            background: -o-linear-gradient(top, #deefff 0%,#98bede 100%);
            background: -ms-linear-gradient(top, #deefff 0%,#98bede 100%);
            background: linear-gradient(top, #deefff 0%,#98bede 100%);
            border-radius: 5px; /* Скругляем уголки */
            border: 1px solid #008; /* Добавляем синюю рамку */
            font: 60px/1 Arial, sans-serif; /* Рубленый шрифт */
            color: #2c539e; /* Цвет текста и ссылки */
        }
        header {
            margin: 0 auto;
            text-align: center;
        }
        nav {
            display: flex;
            padding: 0 5px;
            flex-direction: row;
            display: flex;
            justify-content: center;
            align-content: space-between;
            align-items: center;
        }
        .item {
            margin: 0 5px;
        }
        article {
            font-size: 42px;
            display: inline-block;
            margin: 0px auto;
            text-align: left;
        }
    </style>
</head>
<body>
    <header>
        <p><a href="/" class="button">Главная</a>
    </header>
    <nav>
        <p class="item"><a href="/home" class="button">Дом</a>
        <p class="item"><a href="/outside" class="button">Улица</a>
        <p class="item"><a href="/relay" class="button">Реле</a>
    </nav>
    <article>
    </article>
</body>
</html>)rawliteral";
//-------------------------Outside----------------------------------------------------------------
const char outside_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="ru">
<head>
    <meta http-equiv = "content-type" content = "text/html; charset = UTF-8" />
    <title>Погода на улице</title>
    <style>
        .button {
            display: inline-block; /* Строчно-блочный элемент */
            padding: 5px 20px; /* Добавляем поля */
            text-decoration: none; /* Убираем подчёркивание у ссылки */
            cursor: pointer; /* Курсор в виде руки */
            background: #deefff; /* Фон для браузеров, не поддерживающих градиент */
            /* Градиент */
            background: -moz-linear-gradient(top, #deefff 0%, #98bede 100%);
            background: -webkit-gradient(linear, left top, left bottom, color-stop(0%,#deefff), color-stop(100%,#98bede));
            background: -webkit-linear-gradient(top, #deefff 0%,#98bede 100%);
            background: -o-linear-gradient(top, #deefff 0%,#98bede 100%);
            background: -ms-linear-gradient(top, #deefff 0%,#98bede 100%);
            background: linear-gradient(top, #deefff 0%,#98bede 100%);
            border-radius: 5px; /* Скругляем уголки */
            border: 1px solid #008; /* Добавляем синюю рамку */
            font: 60px/1 Arial, sans-serif; /* Рубленый шрифт */
            color: #2c539e; /* Цвет текста и ссылки */
        }
        header {
            margin: 0 auto;
            text-align: center;
        }
        nav {
            display: flex;
            padding: 0 5px;
            flex-direction: row;
            display: flex;
            justify-content: center;
            align-content: space-between;
            align-items: center;
        }
        .item {
            margin: 0 5px;
        }
        article {
            font-size: 42px;
            display: inline-block;
            margin: 0px auto;
            text-align: left;
        }
    </style>
</head>
<body>
    <header>
        <p><a href="/" class="button">Главная</a>
    </header>
    <nav>
        <p class="item"><a href="/home" class="button">Дом</a>
        <p class="item"><a href="/outside" class="button">Улица</a>
        <p class="item"><a href="/relay" class="button">Реле</a>
    </nav>
    <article>
    </article>
</body>
</html>)rawliteral";
//-----------------------------relay--------------------------------------------------------------
const char relay_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="ru">
<head>
    <meta http-equiv = "content-type" content = "text/html; charset = UTF-8" />
    <title>Управление реле</title>
    <style>
        .button {
            display: inline-block; /* Строчно-блочный элемент */
            padding: 5px 20px; /* Добавляем поля */
            text-decoration: none; /* Убираем подчёркивание у ссылки */
            cursor: pointer; /* Курсор в виде руки */
            background: #deefff; /* Фон для браузеров, не поддерживающих градиент */
            /* Градиент */
            background: -moz-linear-gradient(top, #deefff 0%, #98bede 100%);
            background: -webkit-gradient(linear, left top, left bottom, color-stop(0%,#deefff), color-stop(100%,#98bede));
            background: -webkit-linear-gradient(top, #deefff 0%,#98bede 100%);
            background: -o-linear-gradient(top, #deefff 0%,#98bede 100%);
            background: -ms-linear-gradient(top, #deefff 0%,#98bede 100%);
            background: linear-gradient(top, #deefff 0%,#98bede 100%);
            border-radius: 5px; /* Скругляем уголки */
            border: 1px solid #008; /* Добавляем синюю рамку */
            font: 60px/1 Arial, sans-serif; /* Рубленый шрифт */
            color: #2c539e; /* Цвет текста и ссылки */
        }
        header {
            margin: 0 auto;
            text-align: center;
        }
        nav {
            display: flex;
            padding: 0 5px;
            flex-direction: row;
            display: flex;
            justify-content: center;
            align-content: space-between;
            align-items: center;
        }
        .item {
            margin: 0 5px;
        }
        article {
            font-size: 42px;
            display: inline-block;
            margin: 0px auto;
            text-align: left;
        }
    </style>
</head>
<body>
    <header>
        <p><a href="/" class="button">Главная</a>
    </header>
    <nav>
        <p class="item"><a href="/home" class="button">Дом</a>
        <p class="item"><a href="/outside" class="button">Улица</a>
        <p class="item"><a href="/relay" class="button">Реле</a>
    </nav>
    <article>
    </article>
</body>
</html>)rawliteral";
//================================================================================================
// Replaces placeholder with DHT values
String processor(const String& var){
  if(var == "TEMPERATURE"){
    return String(t);
  }
  else if(var == "HUMIDITY"){
    return String(h);
  }
  else if(var == "UPTIME"){
    return String(u);
  }
  else if(var == "TEMPERATUREOUT"){
    return String(t);
  }
  return String();
}

void setup(){
  Serial.begin(115200);
  dht.begin();
 
  //Добавьте(сотрите) параметр password, если хотите, чтобы точка доступа была закрытой(открытой)
  WiFi.softAP(ssid, password);
  
  //IP addres
  Serial.print("AP IP адрес: ");
  Serial.println(WiFi.softAPIP());
  
  //MAC addres
  Serial.println();
  Serial.print("MAC: ");
  Serial.println(WiFi.macAddress());

  // Выводим локальный IP
  Serial.println(WiFi.localIP());
//----------------------------Переменные-для-пврсера----------------------------------------------------------
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });
  server.on("/home", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", Home_html, processor);
  });
  server.on("/outside", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", outside_html, processor);
  });
  server.on("/relay", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", relay_html, processor);
  });
  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(t).c_str());
  });
  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(h).c_str());
  });
  server.on("/uptime", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(u).c_str());
  });
   server.on("/temperatureout", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(t).c_str());
  });

  server.begin();
}
 
void loop(){  
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    // сохраняем время последнего обновления значения DHT
    previousMillis = currentMillis;
    // Считываем температуру
    float newT = dht.readTemperature();
    if (isnan(newT)) {
      Serial.println("Ошибка при считывании температуры с датчика DHT");
    }
    else {
      t = newT;
      Serial.println(t);
    }
    // Считываем влажность
    float newH = dht.readHumidity();
    if (isnan(newH)) {
      Serial.println("Ошибка при считывании влажности с датчика DHT");
    }
    else {
      h = newH;
      Serial.println(h);
    }
  }
  u = millis();
}
