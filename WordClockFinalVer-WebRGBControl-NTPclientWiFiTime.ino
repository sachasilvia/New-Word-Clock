// Credits to Rui Santos for the original code for getting Date and Time from NTP Server: https://randomnerdtutorials.com/esp32-ntp-client-date-time-arduino-ide/
// Credits to Rui Santos for the original code for creating an interactive slider on a Web Server: https://randomnerdtutorials.com/esp32-web-server-slider-pwm/ 
// Credits to Ben Dash for his modifications of the slider code to control RGB values: https://github.com/bdash9/ESP32-LED-NeoPixel-webserver-with-slider-controls/tree/main 

// *Note* I recommend reading the comments for the code because they will help explain what everything does and why it's neccessary :)

// *** ALL OF THE WORD CLOCK REFERENCES ***

// Helpers
// 0 -> 1 = IT
// 3 -> 4 = IS
// Minutes
// 15 -> 21 = FIFTEEN
// 24 -> 33 = TWENTYFIVE (twenty and five are combined, 20 on its own is: 24 -> 29 and five on its own is: 30 -> 33)
// 47 -> 42 = THIRTY
// 40 -> 38 = TEN
// Helpers
// 48 -> 54 = MINUTES
// 68 -> 71 = PAST
// 67 -> 68 = TO
// Hours
// 64 -> 61 = FOUR
// 72 -> 76 = SEVEN
// 77 -> 83 = TWELVE
// 92 -> 95 = NINE
// 88 -> 91 = FIVE
// 84 -> 86 = TWO
// 96 -> 100 = EIGHT
// 102 -> 107 = ELEVEN
// 117 -> 119 = SIX
// 112 -> 116 = THREE
// 109 -> 111 = ONE
// 120 -> 122 = TEN
// Helper
// 126 -> 131 = O'CLOCK

//                      :LED MATRIX:
//                    11x12 - 132 pixels
// -> 000 001 002 003 004 005 006 007 008 009 010 011
//    023 022 021 020 019 018 017 016 015 014 013 012 <-
// -> 024 025 026 027 028 029 030 031 032 033 034 035
//    047 046 045 044 043 042 041 040 039 038 037 036 <-
// -> 048 049 050 051 052 053 054 055 056 057 058 059
//    071 070 069 068 067 066 065 064 063 062 061 060 <-
// -> 072 073 074 075 076 077 078 079 080 081 082 083
//    095 094 093 092 091 090 089 088 087 086 085 084 <-
// -> 096 097 098 099 100 101 102 103 104 105 106 107
//    119 118 117 116 115 114 113 112 111 110 109 108 <-
// -> 120 121 122 123 124 125 126 127 128 129 130 131


//                    :WORD MATRIX:
//               I T L I S A S T H P M A
//               A C F I F T E E N D C O
//               T W E N T Y F I V E X W
//               T H I R T Y F T E N O S
//               M I N U T E S E T O U R
//               P A S T O R U F O U R T
//               S E V E N X T W E L V E
//               N I N E F I V E C T W O
//               E I G H T F E L E V E N
//               S I X T H R E E O N E G
//               T E N S E Z O'C L O C K

// *** Libraries ***

// Libraries for Time
#include <NTPClient.h>

// Libraries for WiFi
#include <WiFiUdp.h>
#include <WiFi.h>

// Libraries for Web Server
#include <ESPAsyncWebServer.h>

// Libraries for LEDs
#include <Adafruit_NeoPixel.h>

// *** Defines ***

// Sets offset from GMT (Greenwich Mean Time)
#define EST -18000 // - Eastern Standard Time
#define CST -21600 // - Central Standard Time
#define MST -25200 // - Mountain Standard Time
#define PST -28800 // - Pacific Standard Time

#define updateFrequency 1000 // Sets how frequently time is checked in ms

// For defining NeoPixel object
#define PIN GPIO13 // Tells what digital pin the NeoPixel is connected to
#define NUMPIXELS 129 // Tells how many LED pixels exist on the NeoPixel

// Defines used for daylight savings conditonal
#define sunday 0
#define march 3
#define november 11

// *** Variables ***

// Time Variables
byte H; // For Hours
byte M; // For Minutes 

// Auxiliary Variables for Sliders and to Store RBG Values
String redValue = "0"; // Red Slider Value 
String greenValue = "0"; // Green Silder Value
String blueValue = "0"; // Blue Slider Value

const char* PARAM_INPUT = "value"; //for web server control 

// Variables to Set RGB Values (value sourced from Strings above ^^)
int redVal = 0; // Sets Red
int greenVal = 0; // Sets Green
int blueVal = 0; // Sets Blue

const int GPIO13 = 13; // Define GPIO13 as having an unchanging value of 13

// vvv Replace with your network credentials vvv (found on the back of router)
const char* ssid = "TP-Link_51CA"; // <-- Type your SSID here in between the ""
const char* password = "password"; // <-- Type your Password here in between the ""

const long utcOffsetInSeconds = EST; // <-- Set your Timezone (I've only included mainland US abreviations, you can add your own if needed)

// Variables for daylight savings conditional
String stringmonth;
String stringday;
int wday;
int month;
int day;  
int previousSunday;

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

// Define NeoPixel
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

// Set the server port
AsyncWebServer server(80); // Port 80 is the standard for HTTP communication 

// *** Web Page ***
// Here is where the actual web page is created, unfortunately because of how HTML is translated I can't add in comments, I recommend following Rui Santos' guide from Random Nerd Tutorials, it is linked at the top of this document ^^

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

// *** Setting LEDs Function ***

void setled(int, int, int); // Specify the name of the function and say what can be entered, in this case 3 integers

// Function for setting specific LEDs
void setled(int start, int end, int on) { // Create the three ints the function runs on
  for (int pixel = start; pixel < end + 1; pixel++) { // Sets what pixels are turned on by taking a start number(entered later), and end number(entered later) and filling the spaces between them
    if (on == true) { // If the true statement is present during the use of the function
      pixels.setPixelColor(pixel, pixels.Color(redVal, greenVal, blueVal)); // Set the pixels specified to the color values taken from the web server and stored in ints

    } else { // Else if the true statement is not present during the use of the function
      pixels.setPixelColor(pixel, pixels.Color(0, 0, 0)); // Turn the specified pixels off
    }
    pixels.show(); // Display the changes
  }
}

void setup() {

  // Initialize Serial Monitor
  Serial.begin(115200);

  // Print a start message
  Serial.print("attempting connection to: ");
  Serial.println(ssid);

  // Loop until WiFi connects
  WiFi.begin(ssid, password);
  while (WiFi.status()!= WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Print a success message
  Serial.println("");
  Serial.println("WiFi successfully connected!");
  Serial.print("local ip: ");
  Serial.println(WiFi.localIP()); // Print the IP adress that can be searched in a web browser to access the web page

  // Initialize a NTPClient to get time
  timeClient.begin();

  // Initialize and Clear all Pixels
  pixels.begin();
  pixels.clear();

// *** Handling the Web Server ***
server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) { // Set the Route for the Root HTML / the web page 
    String html = String(index_html); // Create a varibale that stores the values of index_html (this is for making a request on the Root HTML / Web Server)
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

  // Check the time
  timeClient.update();

  // Set bytes H and M to Hours and Minutes retrieved from timeClient.update(); and apply the daylight savings conditional for H
    if (((month > march) && (month < november)) || 
     ((month == march) && (previousSunday >= 8)) || 
     ((month == march) && (day > 14)) || 
     ((month == november) && (previousSunday <= 1))) {
    H = timeClient.getHours() + 1; //add the hour for daylight savings
  }  else {
    H = timeClient.getHours(); //justs leaves hours without daylight savings
  }
    M = timeClient.getMinutes(); 

  // Set month and day strings to get the needed values from .getFormattedDate() string
  stringmonth = fdate.substring(5,7);
  stringday = fdate.substring(8,10);

  // Set our ints by converting strings to ints
  month = stringmonth.toInt();
  day = stringday.toInt();
  
  wday = timeClient.getDay(); // Set weekday with .getDay()

  // Print the values of H and M to the Serial Monitor
  Serial.println("");
  Serial.print("Hour: ");
  Serial.println(H);
  Serial.print("Minute: ");
  Serial.println(M);

  // Print DST Test
  if(H == timeClient.getHours()+1){
    Serial.println("Is DST");
  }
  else if(H == timeClient.getHours()){
    Serial.println("Not DST");
  }

  // Print the RGB values of the NeoPixel to the Serial Monitor
  Serial.print("Red:  ");
  Serial.println(redVal);
  Serial.print("Green:  ");
  Serial.println(greenVal);
  Serial.print("Blue:  ");
  Serial.println(blueVal);

  // *** Setting LEDS based on time ***

  setled(0, 0, true);  // IT
  setled(2, 3, true);  // IS

  if (M >= 5) { //Sets the parameters that must be true for the statement below to be executed, in this case the variable M (which stores the number of the Minutes that have passed since the start of the Hour) must have a value equal to or greater than 5
    setled(67, 67, true);  // T for PAS(T) and (T)O         //The "true" statement allows the "if" portion of the setled function to run
  } else {
    setled(67, 67, false);                                  //The "false" statement (or anything besides true) forces the else portion of the setled function to run
  }
  
  if (M  >= 5) {
    setled(47, 53, true);  // MINUTES
  } else {
    setled(47, 53, false);
  }

  if ((M >= 5) && (M <= 34)) {
    setled(68, 70, true);  // PAST (Techincally turns on PAS)
  } else {
    setled(68, 70, false);
  }

  if (M >= 35) {
    setled(66, 66, true);  // TO (Technically turns on O)
  } else {
    setled(66, 66, false);
  }

  setled(124, 129, true);  // O'CLOCK

  if (((M >= 5) && (M < 10)) || ((M >= 25) && (M < 30)) || ((M >= 35) && (M < 40)) || (M >= 55)) {
    setled(29, 32, true);
  } else {
    setled(29, 32, false);  // FIVE (Minutes)
  }

  if (((M >= 10) && (M < 15)) || ((M >= 50) && (M < 55))) {
    setled(37, 39, true);  // TEN (Minutes)
  } else {
    setled(37, 39, false);
  }

  if (((M >= 15) && (M < 20)) || ((M >= 45) && (M < 50))) {
    setled(14, 20, true);  // FIFTEEN (Minutes)
  } else {
    setled(14, 20, false);
  }

  if (((M >= 20) && (M < 30)) || ((M >= 35) && (M < 45))) {
    setled(23, 28, true);  // TWENTY (Minutes)
  } else {
    setled(23, 28, false);
  }

  if ((M >= 30) && (M < 35)) {
    setled(41, 46, true);  // THIRTY (Minutes)
  } else {
    setled(41, 46, false);
  }

  if (((H == 1) && (M < 35)) || ((H == 13) && (M < 35)) || ((H == 12) && (M >= 35)) || ((H == 0) && (M >= 35))) {
    setled(107, 109, true);  // ONE (Hours)
  } else {
    setled(107, 109, false);
  }

  if (((H == 2) && (M < 35)) || ((H == 14) && (M < 35)) || ((H == 13) && (M >= 35)) || ((H == 1) && (M >= 35))) {
    setled(82, 84, true);  // TWO (Hours)
  } else {
    setled(82, 84, false);
  }

  if (((H == 3) && (M < 35)) || ((H == 15) && (M < 35)) || ((H == 14) && (M >= 35)) || ((H == 2) && (M >= 35))) {
    setled(110, 114, true);  // THREE (Hours)
  } else {
    setled(110, 114, false);
  }

  if (((H == 4) && (M < 35)) || ((H == 16) && (M < 35)) || ((H == 15) && (M >= 35)) || ((H == 3) && (M >= 35))) {
    setled(60, 63, true);  // FOUR (Hours)
  } else {
    setled(60, 63, false);
  }

  if (((H == 5) && (M < 35)) || ((H == 17) && (M < 35)) || ((H == 16) && (M >= 35)) || ((H == 4) && (M >= 35))) {
    setled(86, 89, true);  // FIVE (Hours)
  } else {
    setled(86, 89, false);
  }

  if (((H == 6) && (M < 35)) || ((H == 18) && (M < 35)) || ((H == 17) && (M >= 35)) || ((H == 5) && (M >= 35))) {
    setled(115, 117, true);  // SIX (Hours)
  } else {
    setled(115, 117, false);
  }

  if (((H == 7) && (M < 35)) || ((H == 19) && (M < 35)) || ((H == 18) && (M >= 35)) || ((H == 6) && (M >= 35))) {
    setled(71, 75, true);  // SEVEN (Hours)
  } else {
    setled(71, 75, false);
  }

  if (((H == 8) && (M < 35)) || ((H == 20) && (M < 35)) || ((H == 19) && (M>= 35)) || ((H == 7) && (M >= 35))) {
    setled(94, 98, true);  // EIGHT (Hours)
  } else {
    setled(94, 98, false);
  }

  if (((H == 9) && (M < 35)) || ((H == 21) && (M < 35)) || ((H == 20) && (M >= 35)) || ((H == 8) && (M >= 35))) {
    setled(90, 93, true);  // NINE (Hours)
  } else {
    setled(90, 93, false);
  }
  if (((H == 10) && (M < 35)) || ((H == 22) && (M < 35)) || ((H == 21) && (M >= 35)) || ((H == 9) && (M >= 35))) {
    setled(118, 120, true);  // TEN (Hours)
  } else {
    setled(118, 120, false);
  }

  if (((H == 11) && (M < 35)) || ((H == 23) && (M < 35)) || ((H == 22) && (M >= 35)) || ((H == 10) && (M >= 35))) {
    setled(100, 105, true);  // ELEVEN (Hours)
  } else {
    setled(100, 105, false);
  }

  if (((H == 12) && (M < 35)) || ((H == 0) && (M < 35)) || ((H == 23) && (M >= 35)) || ((H == 11) && (M >= 35))) {
    setled(77, 81, true);  // TWELVE (Hours)
  } else {
    setled(77, 81, false);
  }

  // Delay until the loop is executed again
  delay(updateFrequency);
}
