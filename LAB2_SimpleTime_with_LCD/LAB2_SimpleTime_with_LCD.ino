#include <WiFi.h>
#include "time.h"
#include <Wire.h>
#include "rgb_lcd.h"

// LCD
rgb_lcd lcd;

const int colorR = 255;
const int colorG = 0;
const int colorB = 0;

// WIFI
const char* ssid       = "*****";
const char* password   = "***";

// NTP
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;

void printLocalTime()
{
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");

  // set the cursor to column 0, line 1
    lcd.setCursor(0, 0);
    lcd.print(&timeinfo, "%B %d %Y");

    lcd.setCursor(0, 1);
    lcd.print(&timeinfo, "%H:%M:%S");
}

void setup()
{
  Serial.begin(115200);
  
  //connect to WiFi
  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
  Serial.println(" CONNECTED");

  // LCD init
  lcd.begin(16, 2);
  lcd.setRGB(colorR, colorG, colorB);
  delay(1000);
  
  //init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();

  //disconnect WiFi as it's no longer needed
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
}

void loop()
{
  delay(1000);
  printLocalTime();
}
