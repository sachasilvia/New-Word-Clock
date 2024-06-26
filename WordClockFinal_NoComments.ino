#include <NTPClient.h>
#include <WiFiUdp.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <Adafruit_NeoPixel.h>
#define EST -18000
#define CST -21600
#define MST -25200
#define PST -28800
#define updateFrequency 1000
#define PIN GPIO13
#define NUMPIXELS 132
#define sunday 0
#define march 3
#define november 11
byte H;
byte M;
String redValue = "0";
String greenValue = "0";
String blueValue = "0";
const char* PARAM_INPUT = "value";
int redVal = 0;
int greenVal = 0;
int blueVal = 0;
const int GPIO13 = 13;
const char* ssid = "TP-Link_51CA";
const char* password = "password";
const long utcOffsetInSeconds = EST;
String stringmonth;
String stringday;
int wday;
int month;
int day;
int previousSunday;
int firstSunday;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
AsyncWebServer server(80);
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>Word Clock RGB</title>
  <style>
    html {font-family: Arial; display: inline-block; text-align: center;}
    h2 {font-size: 2.3rem;}
    p {font-size: 1.9rem;}
    body {max-width: 400px; margin:0px auto; padding-bottom: 25px;}
    .slider { -webkit-appearance: none; margin: 14px; width: 360px; height: 25px; background: #D4D4D4;
      outline: none; -webkit-transition: .2s; transition: opacity .2s;}
    .slider::-webkit-slider-thumb {-webkit-appearance: none; appearance: none; width: 35px; height: 35px; background: #656C70; cursor: pointer;}
    .slider::-moz-range-thumb { width: 35px; height: 35px; background: #656C70; cursor: pointer; } 
    .red { color: red; }
    .green { color: green; }
    .blue { color: blue; }
  </style>
</head>
<body>
  <h2>Word Clock RGB Controller</h2>
  <p><span style="color:red">Red</span></p>
  <p><input type="range" onchange="updateSliderPWM(this, 'red')" id="redSlider" min="0" max="255" value="%REDVALUE%" step="1" class="slider"></p>
  <p><span style="color:green">Green</span></p>
  <p><input type="range" onchange="updateSliderPWM(this, 'green')" id="greenSlider" min="0" max="255" value="%GREENVALUE%" step="1" class="slider"></p>
  <p><span style="color:blue">Blue</span></p>
  <p><input type="range" onchange="updateSliderPWM(this, 'blue')" id="blueSlider" min="0" max="255" value="%BLUEVALUE%" step="1" class="slider"></p>

<script>
function updateSliderPWM(element, color) {
  var xhr = new XMLHttpRequest();
  xhr.open("GET", "/slider?color=" + color + "&value=" + element.value, true);
  xhr.send();
  document.getElementById("text" + color + "Value").innerHTML = element.value;
  console.log(element.value);
}
</script>
</body>
</html>
)rawliteral";
void setled(int, int, int);
void setled(int start, int end, int on) {
  for (int pixel = start; pixel < end + 1; pixel++) {
    if (on == true) {
      pixels.setPixelColor(pixel, pixels.Color(redVal, greenVal, blueVal));
    } else {
      pixels.setPixelColor(pixel, pixels.Color(0, 0, 0));
    }
    pixels.show();
  }
}
void setup() {
  Serial.begin(115200);
  Serial.print("attempting connection to: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi successfully connected!");
  Serial.print("local ip: ");
  Serial.println(WiFi.localIP());
  timeClient.begin();
  pixels.begin();
  pixels.clear();
  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    String html = String(index_html);
    html.replace("%REDVALUE%", redValue);
    html.replace("%GREENVALUE%", greenValue);
    html.replace("%BLUEVALUE%", blueValue);
    request->send(200, "text/html", html);
  });
  server.on("/slider", HTTP_GET, [](AsyncWebServerRequest* request) {
    String inputMessage;
    String colorParam;
    if (request->hasParam(PARAM_INPUT)) {
      inputMessage = request->getParam(PARAM_INPUT)->value();
      colorParam = request->getParam("color")->value();
      if (colorParam == "red") {
        redValue = inputMessage;
        redVal = redValue.toInt();
      }
      if (colorParam == "green") {
        greenValue = inputMessage;
        greenVal = greenValue.toInt();
      }
      if (colorParam == "blue") {
        blueValue = inputMessage;
        blueVal = blueValue.toInt();
      }
    }
  });
  server.begin();
}
void loop() {
  String fdate = timeClient.getFormattedDate();
  stringmonth = fdate.substring(5, 7);
  stringday = fdate.substring(8, 10);
  month = stringmonth.toInt();
  day = stringday.toInt();
  wday = timeClient.getDay();
  previousSunday = day - wday;
  firstSunday = wday - day;
  timeClient.update();
  if (((month > march) && (month < november)) || ((month == march) && (previousSunday >= 8)) || ((month == march) && (day > 14)) || ((month == november) && (firstSunday >= 0) && (day <= 7))) {
    H = timeClient.getHours() + 1;
    Serial.println("Is DST");
  } else {
    H = timeClient.getHours();
    Serial.println("Not DST");
  }
  M = timeClient.getMinutes();
  Serial.println("");
  Serial.print("Hour: ");
  Serial.println(H);
  Serial.print("Minute: ");
  Serial.println(M);
  Serial.print("Red:  ");
  Serial.println(redVal);
  Serial.print("Green:  ");
  Serial.println(greenVal);
  Serial.print("Blue:  ");
  Serial.println(blueVal);
  Serial.println("");
  setled(0, 1, true);
  setled(3, 4, true);
  if (M >= 5) {
    setled(68, 68, true);
  } else {
    setled(68, 68, false);
  }
  if (M >= 5) {
    setled(48, 54, true);
  } else {
    setled(48, 54, false);
  }
  if ((M >= 5) && (M <= 34)) {
    setled(69, 71, true);
  } else {
    setled(69, 71, false);
  }
  if (M >= 35) {
    setled(67, 67, true);
  } else {
    setled(67, 67, false);
  }
  setled(126, 131, true);
  if (((M >= 5) && (M < 10)) || ((M >= 25) && (M < 30)) || ((M >= 35) && (M < 40)) || (M >= 55)) {
    setled(30, 33, true);
  } else {
    setled(30, 33, false);
  }
  if (((M >= 10) && (M < 15)) || ((M >= 50) && (M < 55))) {
    setled(38, 40, true);
  } else {
    setled(38, 40, false);
  }
  if (((M >= 15) && (M < 20)) || ((M >= 45) && (M < 50))) {
    setled(15, 21, true);
  } else {
    setled(15, 21, false);
  }
  if (((M >= 20) && (M < 30)) || ((M >= 35) && (M < 45))) {
    setled(24, 29, true);
  } else {
    setled(24, 29, false);
  }
  if ((M >= 30) && (M < 35)) {
    setled(42, 47, true);
  } else {
    setled(42, 47, false);
  }
  if (((H == 1) && (M < 35)) || ((H == 13) && (M < 35)) || ((H == 12) && (M >= 35)) || ((H == 0) && (M >= 35))) {
    setled(109, 111, true);
  } else {
    setled(109, 111, false);
  }
  if (((H == 2) && (M < 35)) || ((H == 14) && (M < 35)) || ((H == 13) && (M >= 35)) || ((H == 1) && (M >= 35))) {
    setled(84, 86, true);
  } else {
    setled(84, 86, false);
  }
  if (((H == 3) && (M < 35)) || ((H == 15) && (M < 35)) || ((H == 14) && (M >= 35)) || ((H == 2) && (M >= 35))) {
    setled(112, 116, true);
  } else {
    setled(112, 116, false);
  }
  if (((H == 4) && (M < 35)) || ((H == 16) && (M < 35)) || ((H == 15) && (M >= 35)) || ((H == 3) && (M >= 35))) {
    setled(61, 64, true);
  } else {
    setled(61, 64, false);
  }
  if (((H == 5) && (M < 35)) || ((H == 17) && (M < 35)) || ((H == 16) && (M >= 35)) || ((H == 4) && (M >= 35))) {
    setled(88, 91, true);
  } else {
    setled(88, 91, false);
  }
  if (((H == 6) && (M < 35)) || ((H == 18) && (M < 35)) || ((H == 17) && (M >= 35)) || ((H == 5) && (M >= 35))) {
    setled(117, 119, true);
  } else {
    setled(117, 119, false);
  }
  if (((H == 7) && (M < 35)) || ((H == 19) && (M < 35)) || ((H == 18) && (M >= 35)) || ((H == 6) && (M >= 35))) {
    setled(72, 76, true);
  } else {
    setled(72, 76, false);
  }
  if (((H == 8) && (M < 35)) || ((H == 20) && (M < 35)) || ((H == 19) && (M >= 35)) || ((H == 7) && (M >= 35))) {
    setled(96, 100, true);
  } else {
    setled(96, 100, false);
  }
  if (((H == 9) && (M < 35)) || ((H == 21) && (M < 35)) || ((H == 20) && (M >= 35)) || ((H == 8) && (M >= 35))) {
    setled(92, 95, true);
  } else {
    setled(92, 95, false);
  }
  if (((H == 10) && (M < 35)) || ((H == 22) && (M < 35)) || ((H == 21) && (M >= 35)) || ((H == 9) && (M >= 35))) {
    setled(120, 122, true);
  } else {
    setled(120, 122, false);
  }
  if (((H == 11) && (M < 35)) || ((H == 23) && (M < 35)) || ((H == 22) && (M >= 35)) || ((H == 10) && (M >= 35))) {
    setled(102, 107, true);
  } else {
    setled(102, 107, false);
  }
  if (((H == 12) && (M < 35)) || ((H == 0) && (M < 35)) || ((H == 23) && (M >= 35)) || ((H == 11) && (M >= 35))) {
    setled(78, 83, true);
  } else {
    setled(78, 83, false);
  }
  delay(updateFrequency);
}