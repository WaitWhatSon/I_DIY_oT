#include <WiFi.h>
#include <SPI.h>

// protobuffers
#include "src/messages.pb.h"
#include "pb_common.h"
#include "pb.h"
#include "pb_encode.h"
#include "pb_decode.h"

// WiFi
const char* ssid     = "*****";
const char* password = "***";

WiFiServer server(80);

// buffer for serialized message
uint8_t buffer[128];
size_t message_length;

void setup() {
  Serial.begin(115200);
  while(!Serial);
  Serial.println();
  // Connecting to network
  Serial.print("Connecting to "); Serial.println(ssid);

  // WiFi server  
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  
  Serial.println(""); Serial.println("WiFi Connected.");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());

  server.begin();
}

void loop() {
  WiFiClient client = server.available();

  if (client) {
    Serial.println("New Client.");
    String currentLine = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);

        // READ REQUEST
        if (c == '\n')
        {
          if (currentLine.length() == 0)  { break; } 
          else                            { currentLine = ""; }
        }
        else if (c != '\r')               { currentLine += c; }

        // GET MESSAGE1
        if (currentLine.endsWith("GET /Message1")) {

          Message1 message = Message1_init_zero;
          pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));
          message.has_timestamp = true;
          message.timestamp = 1111;
          message.has_content = true;
          strcpy(message.content, "Message1 content");
          message.has_station = true;
          strcpy(message.station.name, "esp");
          message.station.id = 1;
          bool status = pb_encode(&stream, Message1_fields, &message);
          if(!status) { Serial.println("Failed to encode"); break; }
          Serial.print("\nMessage: ");
          for(int i = 0; i < stream.bytes_written; i++) { Serial.printf("%02x", buffer[i]); }
          message_length = stream.bytes_written;
          
          client.println("HTTP/1.1 200 OK");
          client.println("Content-type:application/octet-stream");
          client.println();
          client.write(&buffer[0], message_length);
          client.flush();
          break;
        }

        // GET MESSAGE2
        if (currentLine.endsWith("GET /Message2")) {

          Message2 message = Message2_init_zero;
          pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));
          message.has_timestamp = true;
          message.timestamp = 2222;
          // message.anotherContent = 2.2;
          message.has_station = true;
          strcpy(message.station.name, "esp");
          message.station.id = 2;
          bool status = pb_encode(&stream, Message2_fields, &message);
          if(!status) { Serial.println("Failed to encode"); break; }
          Serial.print("\nMessage: ");
          for(int i = 0; i < stream.bytes_written; i++) { Serial.printf("%02x", buffer[i]); }
          message_length = stream.bytes_written;

          client.println("HTTP/1.1 200 OK");
          client.println("Content-type:application/octet-stream");
          client.println();
          client.write(&buffer[0], message_length);
          client.flush();
          break;
        }

        // GET MESSAGE3
        if (currentLine.endsWith("GET /Message3")) {
          
          Message3 message = Message3_init_zero;
          pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));
          message.has_timestamp = true;
          message.timestamp = 3333;
          message.has_stationType = true;
          message.stationType = Message3_StationType_ESP;
          message.has_station = true;
          strcpy(message.station.name, "esp");
          message.station.id = 3;
          bool status = pb_encode(&stream, Message3_fields, &message);
          if(!status) { Serial.println("Failed to encode"); break; }
          Serial.print("\nMessage: ");
          for(int i = 0; i < stream.bytes_written; i++) { Serial.printf("%02x", buffer[i]); }
          message_length = stream.bytes_written;
          
          client.println("HTTP/1.1 200 OK");
          client.println("Content-type:application/octet-stream");
          client.println();
          client.write(&buffer[0], message_length);
          client.flush();
          break;
        }

        // GET MESSAGE4
        if (currentLine.endsWith("GET /Message4")) {
          
          Message4 message = Message4_init_zero;
          pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));
          message.has_timestamp = true;
          message.timestamp = 4444;
          message.isOK = true;
          message.isOK = true;
          message.has_station = true;
          strcpy(message.station.name, "esp");
          message.station.id = 4;
          bool status = pb_encode(&stream, Message4_fields, &message);
          if(!status) { Serial.println("Failed to encode"); break; }
          Serial.print("\nMessage: ");
          for(int i = 0; i < stream.bytes_written; i++) { Serial.printf("%02x", buffer[i]); }
          message_length = stream.bytes_written;
          
          client.println("HTTP/1.1 200 OK");
          client.println("Content-type:application/octet-stream");
          client.println();
          client.write(&buffer[0], message_length);
          client.flush();
          break;
        }

        // POST MESSAGE1
        if (currentLine.endsWith("POST /Message1")) {
            client.println("HTTP/1.1 200 OK");
            String header;
            do { header = client.readStringUntil('\n'); } while(header.length() != 1);
            String messagestr = client.readStringUntil('\r');
            Serial.println("Received body: " + messagestr);
            messagestr.getBytes(buffer, messagestr.length()+1);
            Message1 message = Message1_init_zero;
            pb_istream_t stream = pb_istream_from_buffer(buffer, messagestr.length());            
            bool status = pb_decode(&stream, Message1_fields, &message);
            if (!status) { Serial.printf( "Decoding failed: %s\n ", PB_GET_ERROR(&stream)); }
            else
            {/* Print the data contained in the message. */
              Serial.printf( "Message1:\ntimestamp: %d,\ncontent: %s,\nstation: name: %s, id: %d\n" , 
                message.timestamp, message.content, message.station.name, message.station.id);
            }
            client.flush(); // discard all bytes that have been read
            break;
        }

        // POST MESSAGE2
        if (currentLine.endsWith("POST /Message2")) {
            client.println("HTTP/1.1 200 OK");
            String header;
            do { header = client.readStringUntil('\n'); } while(header.length() != 1);
            String messagestr = client.readStringUntil('\r');
            Serial.println("Received body: " + messagestr);
            messagestr.getBytes(buffer, messagestr.length()+1);
            Message2 message = Message2_init_zero;
            pb_istream_t stream = pb_istream_from_buffer(buffer, messagestr.length());            
            bool status = pb_decode(&stream, Message2_fields, &message);
            if (!status) { Serial.printf( "Decoding failed: %s\n ", PB_GET_ERROR(&stream)); }
            else
            {/* Print the data contained in the message. */
              Serial.printf( "Message2:\ntimestamp: %d,\nanotherContent: %d,\nstation: name: %s, id: %d\n" , 
                message.timestamp, sizeof(message.anotherContent), message.station.name, message.station.id);
            }
            client.flush(); // discard all bytes that have been read
            break;
        }
      
        // POST MESSAGE3
        if (currentLine.endsWith("POST /Message3")) {
            client.println("HTTP/1.1 200 OK");
            String header;
            do { header = client.readStringUntil('\n'); } while(header.length() != 1);
            String messagestr = client.readStringUntil('\r');
            Serial.println("Received body: " + messagestr);
            messagestr.getBytes(buffer, messagestr.length()+1);
            Message3 message = Message3_init_zero;
            pb_istream_t stream = pb_istream_from_buffer(buffer, messagestr.length());            
            bool status = pb_decode(&stream, Message3_fields, &message);
            if (!status) { Serial.printf( "Decoding failed: %s\n ", PB_GET_ERROR(&stream)); }
            else
            {/* Print the data contained in the message. */
              Serial.printf( "Message3:\ntimestamp: %d,\nstationType: %d,\nstation: name: %s, id: %d\n" , 
                message.timestamp, message.stationType, message.station.name, message.station.id);
            }
            client.flush(); // discard all bytes that have been read
            break;
        }
      
        // POST MESSAGE4
        if (currentLine.endsWith("POST /Message4")) {
            client.println("HTTP/1.1 200 OK");
            String header;
            do { header = client.readStringUntil('\n'); } while(header.length() != 1);
            String messagestr = client.readStringUntil('\r');
            Serial.println("Received body: " + messagestr);
            messagestr.getBytes(buffer, messagestr.length()+1);
            Message4 message = Message4_init_zero;
            pb_istream_t stream = pb_istream_from_buffer(buffer, messagestr.length());            
            bool status = pb_decode(&stream, Message4_fields, &message);
            if (!status) { Serial.printf( "Decoding failed: %s\n ", PB_GET_ERROR(&stream)); }
            else
            {/* Print the data contained in the message. */
              Serial.printf( "Message4:\ntimestamp: %d,\nisOK: %d,\nstation: name: %s, id: %d\n" , 
                message.timestamp, message.isOK, message.station.name, message.station.id);
            }
            client.flush(); // discard all bytes that have been read
            break;
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("\nClient Disconnected.");
  }
}
