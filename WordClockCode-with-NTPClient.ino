// *** Libraries ***

// Libraries for Time
#include <NTPClient.h>

// Libraries for WiFi
#include <WiFiUdp.h>
#include <WiFi.h>

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
#define PIN GPIO13
#define NUMPIXELS 129

// *** Variables ***

byte H; // For Hours
byte M; // For Minutes 

const int GPIO13 = 13;

// Replace with your network credentials (found on the back of router)
const char* ssid = "TP-Link_51CA";
const char* password = "password";

const long utcOffsetInSeconds = EST; // Timezone set

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

// Define NeoPixel
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

void setled(int, int, int);

// Function for setting specific LEDs
void setled(int start, int end, int on) {
  for (int pixel = start; pixel < end + 1; pixel++) {
    if (on == true) {
      pixels.setPixelColor(pixel, pixels.Color(255, 0, 0));

    } else {
      pixels.setPixelColor(pixel, pixels.Color(0, 0, 0));
    }
    pixels.show();
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

  // Initialize a NTPClient to get time
  timeClient.begin();

  // Initialize and Clear all Pixels
  pixels.begin();
  pixels.clear();

}
void loop() {

  // Check the time
  timeClient.update();

  // Set bytes H and M to Hours and Minutes from timeClient.update();
  H = timeClient.getHours();
  M = timeClient.getMinutes();

  // Print the values of H and M to the Serial Monitor
  Serial.println("");
  Serial.print("Hour: ");
  Serial.println(H);
  Serial.print("Minute: ");
  Serial.println(M);

  // *** Setting words based on time ***

  setled(0, 0, true);  // IT
  setled(2, 3, true);  // IS

  if (M >= 5) {
    setled(67, 67, true);  // T for PAS(T) and (T)O
  } else {
    setled(67, 67, false);
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
