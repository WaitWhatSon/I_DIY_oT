#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>
#include "time.h"

const char* ssid       = "*****";
const char* password   = "***";

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;

String my_Api_Key = "6e0aa98c8b546199e3bf1cf8********";

String my_city = "Bialystok"; //specify your city
String my_country_code = "PL"; //specify your country code

String json_array;

String GET_Request(const char* server) {
  HTTPClient http;    
  http.begin(server);
  int httpResponseCode = http.GET();
  
  String payload = "{}"; 
  
  if (httpResponseCode>0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  http.end();

  return payload;
}

void printLocalTime()
{
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
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
  
  //init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  delay(1000);
}

void loop()
{
  printLocalTime();

  if(WiFi.status()== WL_CONNECTED)
  {
    String server = "http://api.openweathermap.org/data/2.5/weather?q=" + my_city + "," + my_country_code + "&APPID=" + my_Api_Key;
    json_array = GET_Request(server.c_str());
//    Serial.println(json_array);
    JSONVar my_obj = JSON.parse(json_array);

    if (JSON.typeof(my_obj) == "undefined") 
    {
      Serial.println("Parsing input failed!");
    }

    Serial.print("Temperature: ");
    Serial.println(my_obj["main"]["temp"]);
    Serial.print("Pressure: ");
    Serial.println(my_obj["main"]["pressure"]);
    Serial.print("Humidity: ");
    Serial.println(my_obj["main"]["humidity"]);
    Serial.print("Wind Speed: ");
    Serial.println(my_obj["wind"]["speed"]);
  }

  else
  {
    Serial.println("WiFi Disconnected");
  }
  
  delay(5000);
}
