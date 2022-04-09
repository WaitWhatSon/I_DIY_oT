/*
 WiFi Web Server LED Blink

 A simple web server that lets you blink an LED via the web.
 This sketch will print the IP address of your WiFi Shield (once connected)
 to the Serial monitor. From there, you can open that address in a web browser
 to turn on and off the LED on pin 5.

 If the IP address of your shield is yourAddress:
 http://yourAddress/H turns the LED on
 http://yourAddress/L turns it off

 This example is written for a network using WPA encryption. For
 WEP or WPA, change the Wifi.begin() call accordingly.

 Circuit:
 * WiFi shield attached
 * LED attached to pin 5

 created for arduino 25 Nov 2012
 by Tom Igoe

ported for sparkfun esp32 
31.01.2017 by Jan Hendrik Berlin
 
 */

/***
  Read Our Complete Guide: https://RandomNerdTutorials.com/esp32-bme680-sensor-arduino/
  Designed specifically to work with the Adafruit BME680 Breakout ----> http://www.adafruit.com/products/3660 These sensors use I2C or SPI to communicate, 2 or 4 pins are required to interface. Adafruit invests time and resources providing this open source code, please support Adafruit and open-source hardware by purchasing products from Adafruit! Written by Limor Fried & Kevin Townsend for Adafruit Industries. BSD license, all text above must be included in any redistribution
***/

#include <WiFi.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BME680.h"
#include "time.h"

// WiFi
const char* ssid     = "*****";
const char* password = "***";
// BME
#define SEALEVELPRESSURE_HPA (1013.25)
Adafruit_BME680 bme; // I2C
//
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;

WiFiServer server(80);

void setup()
{
  // SERIAL
    Serial.begin(115200);
    while(!Serial);
    Serial.println();
    // We start by connecting to a WiFi network
    Serial.print("Connecting to ");
    Serial.println(ssid);

  // BME
    if (!bme.begin()) 
    {
      Serial.println(F("Could not find a valid BME680 sensor, check wiring!"));
      while (1);
    }
    // Set up oversampling and filter initialization
    bme.setTemperatureOversampling(BME680_OS_8X);
    bme.setHumidityOversampling(BME680_OS_2X);
    bme.setPressureOversampling(BME680_OS_4X);

  // WIFI
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  
  // NTP
    //init and get the time
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  // SERVER
    server.begin();
}

int value = 0;

int makeMeasurement()
{
  unsigned long endTime = bme.beginReading();
  if (endTime == 0) {
    Serial.println(F("Failed to begin reading :("));
    return 1;
  }
  delay(50);
  if (!bme.endReading()) {
    Serial.println(F("Failed to complete reading :("));
    return 1;
  }
  return 0;
}

void loop(){
 WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
    Serial.println("New Client.");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // the content of the HTTP response follows the header:
            client.print("Click <a href=\"/T\">here</a> to get temperature.<br>");
            client.print("Click <a href=\"/H\">here</a> to get humidity.<br>");
            client.print("Click <a href=\"/P\">here</a> to get pressure.<br>");

            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          } else {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
        
        if (currentLine.endsWith("GET /weather")) {
          Serial.println("GET weather");
          if(makeMeasurement()>0) {
            client.println("HTTP/1.1 503 Service Unavailable");
            client.println("Content-type:text/html");
            client.println("Failed to make measurement<br>");
          } else {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.print("temperature: ");
            client.println(bme.temperature);
            client.print("pressure: ");
            client.println(bme.pressure);
            client.print("humidity: ");
            client.println(bme.humidity);
          }
          break; // break out of the while loop:
        }
        if (currentLine.endsWith("GET /time")) {
          Serial.println("GET time");
          struct tm timeinfo;
          if(!getLocalTime(&timeinfo)) {
            client.println("HTTP/1.1 503 Service Unavailable");
            client.println("Content-type:text/html");
            client.println("Failed to obtain time<br>");
          } else {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.print("time: ");
            client.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
          }
          break; // break out of the while loop:
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }
}
