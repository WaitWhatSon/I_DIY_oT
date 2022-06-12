#include <WiFi.h>
#include <PubSubClient.h>

#include <Keypad.h>
#include <Adafruit_NeoPixel.h>


// RPi Access Point:
const char* ssid = "*****";
const char* password = "***";

// CHANGE FOR CLIENT 1 NAD CLIENT 2
const char* esp_name = "ESP32_Client1"; // to change
const char* mqtt_user = "client1";
const char* mqtt_password = "password1";

// client1: password1
// client2: password2


// KEYBOARD:
const byte ROW_NUM = 4; //four rows
const byte COL_NUM = 4; //four columns
char keys[ROW_NUM][COL_NUM] = {
  {'2', '5', '8', 'Q'},
  {'2', '5', '8', 'E'},
  {'1', '4', '7', 'F'},
  {'0', '3', '6', 'G'}
};
byte pin_rows[ROW_NUM]      = {23, 22, 21, 19}; // GIOP23, GIOP22, GIOP21, GIOP19 connect to the row pins
byte pin_column[COL_NUM] = {18, 5, 4, 15};   // GIOP18, GIOP5, GIOP4, GIOP15 connect to the column pins
Keypad keypad = Keypad( makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COL_NUM );

// LEDS:
#define BRIGHTNESS 10
#define PIN 2 // DATA PIN
#define LEDSNUMBER 9
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(LEDSNUMBER, PIN, NEO_GRB + NEO_KHZ800);

// MQTT:
const char* mqtt_server = "192.168.43.36"; // $hostname -I // ifconfig (wlan0)
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

// GAME:
int myNum = 0;
int turn = 0; // 0 -> oczekiwanie; 1 -> player 1; 2 -> player 2
char positionNum[2] = {'x', '\0'}; // ruch do wysłania
char positionTopic[8] = {'c','l','i','e','n','t','0','\0'};

// declarations:
void setDiodes(int);
void sendPosition(char);


void setup(){
  Serial.begin(115200);

  // leds setup:
  pixels.begin();
  int diodes[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
  
  // wifi connecting
  pixels.setPixelColor(0, BRIGHTNESS, BRIGHTNESS, 0);
  pixels.show();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected.");
  setDiodes(diodes);

  // pub sub client
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop(){
  char key = keypad.getKey();

  if (key != NO_KEY){
    Serial.println(key);
    
    if(key <= '8' && key >= '0' && turn == myNum)
    {
      Serial.println("MOŻNA WYSYŁAĆ");
      Serial.println(turn);
      Serial.println(myNum);
      int diodes[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
      diodes[key-'0'] = 1;
      sendPosition(key);
      setDiodes(diodes);
    }
    else if(key == 'Q' && myNum == 0)
    {
      Serial.println("SEND CONNECTION REQUEST");
      sendConnectionRequest();
    }
    else
    {
      Serial.println("CANNOT SEND ANYTHING");
    }
  }

  if (!client.connected()) {
      reconnect();
    }
    client.loop();
}



// functions:
// - game:
void setDiodes(int diodes[9]){
  for(int i = 0; i < 9; i++)
  {
    if(diodes[i] == -1) // empty
    {
      pixels.setPixelColor(i, 0, 0, 0  );
    }
    else if(diodes[i] == 1) // O
    {
      pixels.setPixelColor(i, BRIGHTNESS, 0, 0);
    }
    else if(diodes[i] == 2) // X
    {
      pixels.setPixelColor(i, 0, BRIGHTNESS, 0);
    }
    else if(diodes[i] == 3) // X
    {
      pixels.setPixelColor(i, BRIGHTNESS, BRIGHTNESS, BRIGHTNESS);
    }
    else
    {
      pixels.setPixelColor(i, 0, 0, BRIGHTNESS);
    }
  }
  pixels.show();
}

void sendConnectionRequest()
{
  client.subscribe("nextclientid");
  client.publish("gameclients", "subscribed");
  client.subscribe("turn");
  client.subscribe("board");
  client.subscribe("won");
}

void sendPosition(char pos)
{
  positionNum[0] = pos;
  client.publish(positionTopic, positionNum);
  Serial.println(positionTopic);
  Serial.println(positionNum);
  delay(100);
}


// - mqtt:
void callback(char* topic, byte* message, unsigned int length) 
{
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  if(strcmp(topic, "nextclientid") == 0) // CLIENT ID -> nadany numer klienta
  {
    if(myNum == 0)
    {
      myNum = ((char)message[0])-'0';
      positionTopic[6] = (char)message[0];
    }
  }
  else if(strcmp(topic, "turn") == 0) // TURN -> czyj ruch / 0 jesli gra jeszcze nie rozpoczeta
  {
    turn = ((char)message[0])-'0';
    Serial.print("TURN: ");
    Serial.println(turn);
  }
  else if(strcmp(topic, "board") == 0) // BOARD -> przysłana plansza do przeparsowania
  {
    Serial.println("BOARD");
    int diodes[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
    for (int i = 0; i < 9; i++) {
      diodes[i] = (char)message[i]-'0';
    }
    setDiodes(diodes);
  }
  else if(strcmp(topic, "won") == 0) // BOARD -> przysłana plansza do przeparsowania
  {
    Serial.println("WON");
    int diodes[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
    for (int i = 0; i < 9; i++) {
      diodes[i] = ((char)message[0])-'0';
    }
    setDiodes(diodes);
    delay(1000);
    int diodes2[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
    setDiodes(diodes2);
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(esp_name, mqtt_user, mqtt_password)) {
      Serial.println("connected");
      // Subscribe
      //client.subscribe("");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
