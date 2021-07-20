//------------------Библиотеки-------------------------------------------------------------------------------------------------
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Hash.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
//------------------Название-сети-и-пароль--------------------------------------------------------------------------------------
const char* ssid     = "Meteostation";
const char* password = "$|9U|X";
//------------------Переменные--------------------------------------------------------------------------------------------------
float temperature = 0.0;
float temperatureOut = 0.0;
float humidity = 0.0;
float humidityOut = 0.0;
float Speed = 0.0;
float uptime = millis();
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
    <meta http-equiv = "content-type" content = "text/html" charset = "windows-1251"/>
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
        <div>
            <p>
                <span class="dht-labels">Температура в доме: </span>
                <span id="temperature">%TEMPERATURE%</span>
                <sup class="units">&deg;C</sup>
            </p>
            <p>
                <span class="dht-labels">Температура на улице: </span>
                <span id="temperatureout">%TEMPERATUREOUT%</span>
                <sup class="units">&deg;C</sup>
            </p>
        </div>
        <div>
            <p>
                <span style="font-family: monospace">uptime: </span>
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
//--------------------------Home--------------------------------------------------------------------------------------------------
const char Home_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="ru">
<head>
    <meta http-equiv="content-type" content="text/html" charset="utf-8" />
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
        <div>
            <p>
                <span class="dht-labels">Температура: </span>
                <span id="temperature">%TEMPERATURE%</span>
                <sup class="units">&deg;C</sup>
            </p>
            <p>
                <span class="dht-labels">Влажность: </span>
                <span id="humidity">%HUMIDITY%</span>
                <span>%</span>
            </p>
            <p>
                <span class="dht-labels">Атмосферное давление: </span>
                <span id="pressure">%PRESSURE%</span>
                <span> мм рт.ст.</span>
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
    <meta http-equiv="content-type" content="text/html" charset="windows-1251" />
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
        <div>
            <p>
                <span class="dht-labels">Температура</span>
                <span id="temperatureout">%TEMPERATUREOUT%</span>
                <sup class="units">&deg;C</sup>
            </p>
            <p>
                <span class="dht-labels">Влажность</span>
                <span id="humidityout">%HUMIDITYOUT%</span>
                <span>%</span>
            </p>
            <p>
                <span class="dht-labels">Атмосферное давление</span>
                <span id="pressure">%PRESSURE%</span>
                <span> мм рт.ст.</span>
            </p>
            <p>
                <span class="dht-labels">Скорость ветра: </span>
                <span id="speed">%SPEED%</span>
                <span> м/с</span>
            </p>
        </div>
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
                document.getElementById("speed").innerHTML = this.responseText;
            }
        };
        xhttp.open("GET", "/speed", true);
        xhttp.send();
    }, 10000);
</script>
</html>)rawliteral";
//-----------------------------relay-------------------------------------------------------------------------------------------------
const char relay_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="ru">
<head>
    <meta http-equiv="content-type" content="text/html" charset="windows-1251" />
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
        .buttonON {
            display: inline-block; /* Строчно-блочный элемент */
            padding: 5px 20px; /* Добавляем поля */
            text-decoration: none; /* Убираем подчёркивание у ссылки */
            cursor: pointer; /* Курсор в виде руки */
            background: #4cff00; /* Фон  */
            border-radius: 5px; /* Скругляем уголки */
            border: 1px solid #000; /* Добавляем синюю рамку */
            font: 32px/1 Arial, sans-serif; /* Рубленый шрифт */
            color: #2c539e; /* Цвет текста и ссылки */
        }
        .buttonOFF {
            display: inline-block; /* Строчно-блочный элемент */
            padding: 5px 20px; /* Добавляем поля */
            text-decoration: none; /* Убираем подчёркивание у ссылки */
            cursor: pointer; /* Курсор в виде руки */
            background: #ff0000; /* Фон */
            border-radius: 5px; /* Скругляем уголки */
            border: 1px solid #000; /* Добавляем синюю рамку */
            font: 32px/1 Arial, sans-serif; /* Рубленый шрифт */
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
        <p>
            <span>Реле</span>
            <a href="#" class="buttonON">ON</a>
            <a href="#" class="buttonOFF">OFF</a>
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
    return String(temperatureOut);
  }
  else if (var == "HUMIDITYOUT") {
      return String(humidityOut);
  }
  else if (var == "SPEED") {
      return String(Speed);
  }
  return String();
}

void setup(){
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
    request->send_P(200, "text/plain", String(temperatureOut).c_str());
  });
  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(humidity).c_str());
  });
  server.on("/humidityout", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(humidityOut).c_str());
  });
  server.on("/speed", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send_P(200, "text/plain", String(Speed).c_str());
  });
  server.on("/uptime", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(uptime).c_str());
  });
  
  server.begin();
}
 
void loop(){  
  uptime = millis();
}
