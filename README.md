# New-Word-Clock
This repository contains the new code for the Word Clock which gets time through WiFi instead of an RTC, as well as has the capabilities to be controlled through a Web Server. 

Install this library for NTPClient: https://github.com/taranais/NTPClient

Install this library for ESPAsyncWebServer: https://github.com/me-no-dev/ESPAsyncWebServer
--> *If you are using an ESP32, like myself, you will need to install this additional library for AsyncTCP: https://github.com/me-no-dev/AsyncTCP

Some of these libraries only work on a specific older branch, if your IDE prompts you to update these libraries DO NOT, as they won't work properly if updated.

I used an Adafruit ESP32 HUZZAH Breakout but any ESP32 model should work fine.
