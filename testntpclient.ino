// *** Libraries ***

// Libraries for Time
#include <NTPClient.h>

// Libraries for WiFi
#include <WiFiUdp.h>
#include <WiFi.h>

// *** Defines ***

// Sets offset from GMT (Greenwich Mean Time)
#define EST -18000 // - Eastern Standard Time
#define CST -21600 // - Central Standard Time
#define MST -25200 // - Mountain Standard Time
#define PST -28800 // - Pacific Standard Time

#define updateFrequency 1000; // sets how frequently time is checked in ms

// *** Variables ***

byte H; // For Hours
byte M; // For Minutes 

// Replace with your network credentials (found on the back of router)
const char* ssid = "TP-Link_51CA";
const char* password = "password";

const long utcOffsetInSeconds = EST; // Timezone set

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

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
}
void loop() {

  // Check the time
  timeClient.update();

  // Set bytes H and M to Hours and Minutes from timeClient.update();
  H = timeClient.getHours();
  M = timeClient.getMinutes();

  // Print the values of H and M to the Serial Monitor
  Serial.println("");
  Serial.println("Hour: ");
  Serial.print(H);
  Serial.println("Minute: ");
  Serial.print(M);

  // Delay until the loop is executed again
  delay(updateFrequency);
}
