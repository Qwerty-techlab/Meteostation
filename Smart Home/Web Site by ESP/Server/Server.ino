//------------------Библиотеки-------------------------------------------------------------------------------------------------
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Hash.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <string.h>
//------------------Название-сети-и-пароль--------------------------------------------------------------------------------------
const char* ssid     = "Meteostation";
const char* password = "$|9U|X";
//------------------Переменные--------------------------------------------------------------------------------------------------
const char* PARAM_INPUT_1 = "state";
float temperature = 0.0;
float temperatureout = 0.0;
float humidity = 0.0;
float humidityout = 0.0;
float wind = 0.0;
float pressure = 0.0;
float uptime = millis();
float transmit_data[7];
int ledState = LOW;
const int output = 2;

#define channel 5
#define RF_SPEED RF24_1MBPS    // Указываем скорость передачи данных (RF24_250KBPS, RF24_1MBPS, RF24_2MBPS), RF24_1MBPS - 1Мбит/сек
#define RF_PA RF24_PA_MAX      // Указываем мощность передатчика (RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_HIGH=-6dBm, RF24_PA_MAX=0dBm)
RF24 radio(4, 15);

// Создаём AsyncWebServer object на 80 порту
AsyncWebServer server(80); 

// =================================HTML=страницы================================================================================

//EXAMPLE
/*const char #NAME#[] PROGMEM = R"rawliteral(
  <!DOCTYPE html>
   <html>
     <head></head>
     <bode></body>
   </html>)rawliteral";  
*/
//----------------------------------Index-----------------------------------------------------------------------------------------
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="ru">
<head>
    <meta http-equiv = "content-type" content = "text/html" charset = "UTF-8"/>
    <title>Главная страница</title>
    <style>
        .button {
            display: inline-block; /* Строчно-блочный элемент */
            padding: 5px 20px; /* Добавляем поля */
            text-decoration: none; /* Убираем подчёркивание у ссылки */
            cursor:pointer; /* Курсор в виде руки */
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
            color: #2c539e; /* Цвет текста и ссылки */`
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
            font-size: 32px;
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
            <h1>
                <span class="dht-labels">Температура в доме: </span>
                <span id="temperature">%TEMPERATURE%</span>
                <span class="units">&deg;C</span>
            </h1>
            <h1>
                <span class="dht-labels">Температура на улице: </span>
                <span id="temperatureout">%TEMPERATUREOUT%</span>
                <sup class="units">&deg;C</sup>
            </h1>
        </div>
        <div>
            <h1>
                <span style="font-family: monospace">uptime: </span>
                <span id="uptime">%UPTIME%</span>
            </h1>
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
//--------------------------Home--------------------------------------------------------------------------------------------------
const char Home_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="ru">
<head>
    <meta http-equiv="content-type" content="text/html" charset="UTF-8" />
    <title>Дом</title>
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
            font-size: 32px;
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
        <h1>
                <span>Температура: </span>
                <span id="temperature">%TEMPERATURE%</span>
                <span class="units">&deg;C</span>
        </h1>
        <h1>
                <span>Влажность: </span>
                <span id="humidity">%HUMIDITY%</span>
        </h1>
        <h1> 
                <span>Атмосферное давление: </span>
                <span id="pressure">%PRESSURE%</span>
                <span>мм рт.ст.</span>
        </h1>
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
      document.getElementById("humidity").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/humidity", true);
  xhttp.send();
}, 10000);

setInterval(function () {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("pressure").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/pressure", true);
  xhttp.send();
}, 10000 ) ;
</script>
</html>)rawliteral";
//-------------------------Outside--------------------------------------------------------------------------------------------------
const char outside_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="ru">
<head>
    <meta http-equiv="content-type" content="text/html" charset="UTF-8" />
    <title>Улица</title>
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
            font-size: 32px;
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
        <h1>
                <span>Температура: </span>
                <span id="temperatureout">%TEMPERATUREOUT%</span>
                <span class="units">&deg;C</span>
        </h1>
        <h1>
                <span>Влажность: </span>
                <span id="humidityout">%HUMIDITYOUT%</span>
        </h1>
        <h1> 
                <span>Атмосферное давление: </span>
                <span id="pressure">%PRESSURE%</span>
                <span>мм рт.ст.</span>
        </h1>
        <h1> 
                <span>Скорость ветра: </span>
                <span id="wind">%WIND%</span>
                <span>m/c</span>
        </h1>
    </article>
</body>
<script>
setInterval(function ( ) {
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
      document.getElementById("humidityout").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/humidityout", true);
  xhttp.send();
}, 10000);

setInterval(function () {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("pressure").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/pressure", true);
  xhttp.send();
}, 10000);
setInterval(function () {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function () {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("wind").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/wind", true);
  xhttp.send();
}, 10000);
</script>
</html>)rawliteral";
//-----------------------------relay-------------------------------------------------------------------------------------------------
const char relay_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="ru">
<head>
    <meta http-equiv="content-type" content="text/html" charset="UTF-8" />
    <title>Реле</title>
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
            font-size: 32px;
            display: inline-block;
            margin: 0px auto;
            text-align: left;
        }
        .switch {
            position: relative;
            display: inline-block;
            width: 120px;
            height: 68px
        }

            .switch input {
                display: none
            }

        .slider {
            position: absolute;
            top: 0;
            left: 0;
            right: 0;
            bottom: 0;
            background-color: #ccc;
            border-radius: 34px
        }

            .slider:before {
                position: absolute;
                content: "";
                height: 52px;
                width: 52px;
                left: 8px;
                bottom: 8px;
                background-color: #fff;
                -webkit-transition: .4s;
                transition: .4s;
                border-radius: 68px
            }

        input:checked + .slider {
            background-color: #2196F3
        }

            input:checked + .slider:before {
                -webkit-transform: translateX(52px);
                -ms-transform: translateX(52px);
                transform: translateX(52px)
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
        <p>
            %BUTTONPLACEHOLDER%
            <script>
function toggleCheckbox(element) {
  var xhr = new XMLHttpRequest();
  if(element.checked){ xhr.open("GET", "/update?state=1", true); }
  else { xhr.open("GET", "/update?state=0", true); }
  xhr.send();
}
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      var inputChecked;
      var outputStateM;
      if( this.responseText == 1){
        inputChecked = true;
        outputStateM = "On";
      }
      else {
        inputChecked = false;
        outputStateM = "Off";
      }
      document.getElementById("output").checked = inputChecked;
      document.getElementById("outputState").innerHTML = outputStateM;
    }
  };
  xhttp.open("GET", "/state", true);
  xhttp.send();
}, 1000 ) ;
            </script>
        </p>
    </article>
</body>
</html>)rawliteral";
//============================================================================================================================================
// Replaces placeholder with DHT values
String processor(const String& var){
  if(var == "TEMPERATURE"){
    return String(temperature);
  }
  else if(var == "HUMIDITY"){
    return String(humidity);
  }
  else if(var == "UPTIME"){
    return String(uptime);
  }
  else if(var == "TEMPERATUREOUT"){
    return String(temperatureout);
  }
  else if (var == "HUMIDITYOUT") {
      return String(humidityout);
  }
  else if (var == "WIND") {
      return String(wind);
  }
  else if (var == "PRESSURE") {
      return String(pressure);
  }
  else if (var == "BUTTONPLACEHOLDER") {
      String buttons = "";
      String outputStateValue = outputState();
      buttons += "<h4>Output - GPIO 2 - State <span id=\"outputState\"></span></h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"output\" " + outputStateValue + "><span class=\"slider\"></span></label>";
      return buttons;
  }
  return String();
}

String outputState() {
    if (digitalRead(output)) {
        return "checked";
    }
    else {
        return "";
    }
    return "";
}

void setup(){
//----------------------------Настройки-nRF-------------------------------------------------------------------------------------------------------
  SPI.setHwCs(true);
  SPI.begin();
  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(MSBFIRST);   
  radio.begin();                                        // Инициируем работу nRF24L01+
  radio.setChannel(channel);                            
  radio.setDataRate     (RF_SPEED);                     
  radio.setPALevel      (RF_PA);                        
  radio.openReadingPipe (1, 0x1234567890LL);            // Открываем 1 трубу с идентификатором 0x1234567890 для приема данных 
                                                        //(на ожном канале может быть открыто до 6 разных труб, которые должны
                                                        //отличаться только последним байтом идентификатора)
  radio.startListening();                               // Включаем приемник, начинаем прослушивать открытую трубу

  Serial.begin(115200);
 
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

  pinMode(output, OUTPUT);
  digitalWrite(output, LOW);
//----------------------------Переменные-для-пврсера--------------------------------------------------------------------------------------------
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
    request->send_P(200, "text/plain", String(temperature).c_str());
  });
  server.on("/temperatureout", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send_P(200, "text/plain", String(temperatureout).c_str());
  });
  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(humidity).c_str());
  });
  server.on("/humidityout", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(humidityout).c_str());
  });
  server.on("/pressure", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(pressure).c_str());
  });
  server.on("/uptime", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(uptime).c_str());
  });
  server.on("/wind", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(wind).c_str());
  });
  server.on("/update", HTTP_GET, [](AsyncWebServerRequest* request) {
      String inputMessage;
      String inputParam;
      // получаем значение input1 <ESP_IP>/update?state=<inputMessage>
      if (request->hasParam(PARAM_INPUT_1)) {
          inputMessage = request->getParam(PARAM_INPUT_1)->value();
          inputParam = PARAM_INPUT_1;
          digitalWrite(output, inputMessage.toInt());
          ledState = !ledState;
      }
      else {
          inputMessage = "No message sent";
          inputParam = "none";
      }
      Serial.println(inputMessage);
      request->send(200, "text/plain", "OK");
      });

  // Отправляем запрос GET на <ESP_IP>/state
  server.on("/state", HTTP_GET, [](AsyncWebServerRequest* request) {
      request->send(200, "text/plain", String(digitalRead(output)).c_str());
      });
  
  server.begin();
}
 
void loop() {
    uptime = millis();

    if (radio.available()) {
        radio.read(&transmit_data, sizeof(transmit_data));
        //---------------------------Принятие-данных-с-антенны-----------------------------------------------------------------------------------------------
        temperature = transmit_data[0];
        humidity = transmit_data[1];
        pressure = transmit_data[2];
        temperatureout = transmit_data[3];
        humidityout = transmit_data[4];
        wind = transmit_data[5];
    }
    digitalWrite(output, !ledState);

}
