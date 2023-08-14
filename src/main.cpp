#include <Arduino.h>

/*********
Written by spoonie (Rick Spooner) for RobotsGo 

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.

A Huge thankyou to Rui Santos https://randomnerdtutorials.com for his
esp32 tutorials which this code is heavly based on.

Thanks Rui :)
    
*********/

// Load needed library's 
#include <WiFi.h>
#include <Servo.h>
#include <Arduino.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "SPIFFS.h"
#include <Servo.h>

// Replace with your network credentials
const char* ssid     = "THE_CLAW";
const char* password = "123456789";

// Set web server port number to 80
AsyncWebServer server(80);

AsyncWebSocket ws("/ws");

// create servo object to control a servo
Servo clawServo;  
Servo clawRotateServo;
Servo stickServo;
Servo boomServo;
Servo slewServo;

// GPIO the servo is attached to
static const int clawPin = 12;
static const int clawRotatePin = 27;
static const int stickPin = 25;
static const int boomPin = 32;
static const int slewPin = 23;

// Servo vars Defaults
unsigned int clawOpen = 180;
unsigned int clawRotate = 90;
unsigned int stick = 136;
unsigned int boom = 0;
unsigned int slew = 91;  

// Initialize SPIFFS
void initFS() 
{
  if (!SPIFFS.begin()) {
    Serial.println("An error has occurred while mounting SPIFFS");
  }
  else{
   Serial.println("SPIFFS mounted successfully");
  }
}

// Initialize WiFi
void initWiFi() 
{
Serial.println("Setting soft access point mode");
WiFi.mode(WIFI_AP);
delay(250);

Serial.println("SSID:" + String(ssid));
Serial.println("Password:" + String(password));
WiFi.softAP(ssid, password);
delay(250);

IPAddress IP = WiFi.softAPIP();
Serial.print("AP IP address:");
Serial.println(IP);
delay(250);
}


void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) 
  {
    data[len] = 0;
    String message = (char*)data;
    Serial.println("GUI Message: " + message);
    
    //Claw open commands
    if (message == "cOpen")
    {
      if(clawOpen <= 180 && clawOpen > 0)
      {
        clawOpen--;
        Serial.println("Claw open recived, current valve of claw:" + String(clawOpen));
      }
      else
      {
        Serial.println("Claw open recived, CLAW FULLY OPEN, current valve of claw:" + String(clawOpen));
      }
    }
    else if (message == "cClose")
    {
      if(clawOpen >= 0 && clawOpen < 180)
      {
        clawOpen++;
        Serial.println("Claw open recived, current valve of claw:" + String(clawOpen));
      }
      else
      {
        Serial.println("Claw open recived, CLAW FULLY CLOSED, current valve of claw:" + String(clawOpen));
      }
    }

    //Claw Rotate commands
    if (message == "rRight")
    {
      if (clawRotate <= 180 && clawRotate > 0)
      {
        clawRotate--;
        Serial.println("Claw right recived, current valve of claw rotation:" + String(clawRotate));
      }
      else
      {
        Serial.println("Claw right recived, CLAW ROTATED FULLY RIGHT, current valve of claw rotation:" + String(clawRotate));
      }
    }
    else if (message == "rLeft")
    {
      if(clawRotate >= 0 && clawRotate < 180)
      {
        clawRotate++;
        Serial.println("Claw left recived, current valve of claw rotation:" + String(clawRotate));
      }
      else
      {
        Serial.println("Claw left recived, CLAW ROTATED FULLY LEFT, current valve of claw rotation:" + String(clawRotate));
      }
    }

    //Stick commands
    if (message == "sDown")
    {
      if(stick <= 180 && stick >0)
      {
        stick--;
        Serial.println("Slew dowm recived, current valve of slew:" + String(stick));
      }
      else
      {
        Serial.println("Slew dowm recived, SLEW FULLY DOWN, current valve of slew:" + String(stick));
      }
    }
    else if (message == "sUp")
    {
      if(stick >= 0 && stick < 180)
      {
        stick++;
        Serial.println("Slew up recived, current valve of slew:" + String(stick));
      }
      else
      {
        Serial.println("Slew up recived, SLEW FULLY UP, current valve of slew:" + String(stick));
      }
    }

    //Boom commands
    if (message == "bBackwards")
    {
      
      if (boom <= 95 && boom > 0)
      {
        boom--;
        Serial.println("Boom backwards recived, current valve of boom:" + String(boom));
      }
      else
      {
        Serial.println("Boom backwards recived, BOOM FULLY BACKWARDS, current valve of boom:" + String(boom));
      }
    }
    else if (message == "bForwards")
    {
      if(boom >= 0 && boom < 95)
      {
        boom++;
        Serial.println("Boom forwards recived, current valve of boom:" + String(boom));
      }
      else
      {
        Serial.println("Boom forwards recived, BOOM FULLY FORWARDS, current valve of boom:" + String(boom));
      }
    }

    //Slew commands 
    if (message == "sLeft")
    {
      Serial.println("Slew left recived");
      slew = 81;
    }
    else if (message == "sRight")
    {
      Serial.println("Slew right recived");
      slew = 101;
    }
    
    //Stop command
    if (message == "sStop")
    {
      Serial.println("STOP recived");
      slew= 91;
    }

    //Home commands
    if (message == "home")
    {
      clawOpen = 180;
      clawRotate = 90;
      stick = 136;
      boom = 0;
      slew = 91;

      Serial.println("Servos Homed");
    }
  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) 
{
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

void initWebSocket() 
{
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

void setup() 
{
  Serial.begin(115200);
  delay(250);

  Serial.println("*****************************************");
  Serial.println("*                                       *");
  Serial.println("* THE CLAW ESP32 SERVO CONTROLLER 0.1V  *");
  Serial.println("*     By Rick spooner (spoonie)         *");
  Serial.println("*     https://github.com/RobotsGo       *");
  Serial.println("*     https://robotsgo.net/             *");
  Serial.println("*                                       *");
  Serial.println("*    Thanks Rui Santos                  *");
  Serial.println("*    https://randomnerdtutorials.com    *");
  Serial.println("*                                       *");
  Serial.println("*****************************************");
  
  clawServo.attach(clawPin);
  clawRotateServo.attach(clawRotatePin);
  stickServo.attach(stickPin);
  boomServo.attach(boomPin);
  slewServo.attach(slewPin);

  initFS();
  initWiFi();
  initWebSocket();
  
  // Web Server Root URL
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", "text/html");
  });
  
  server.serveStatic("/", SPIFFS, "/");

  // Start server
  server.begin();
}

void loop() 
{
  clawServo.write(clawOpen);
  clawRotateServo.write(clawRotate);
  stickServo.write(stick);
  boomServo.write(boom); 
  slewServo.write(slew);
  ws.cleanupClients();
}