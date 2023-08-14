# THE_CLAW
An ESP32 Web Wifi-AP controller for the EEZYbotARM MK2 by Carlo Franciscone fitted with the Mantis Gripper claw by Sergei Golikov.  
   
## Arm Build Instructions
EEZYbotARM MK2   
https://www.thingiverse.com/thing:1454048  
Mantis Gripper    
https://www.thingiverse.com/thing:4148341   
   
## Requirements
Platform.io      
ESP-32 Dev Board (ESP32-Wroom-32)     
6V power source    
Your servos values will possibly need adjustment to function correctly         

## Libraries
AsyncTCP   
ESP Async WebServer   
ESPAsyncTCP   
ServoESP32   

## PINOUT
clawPin = 12   
clawRotatePin = 27  
stickPin = 25   
boomPin = 32   
slewPin = 23   

## Usage
The ESP32 is configured to be its own Wifi_Ap, can be connected to with any device with wifi and a webbrowser.        
Once powered on connect to the wifi network SSID:THE_CLAW PASSWORD:123456789, your device will get an IP.   
Point your webbrowser to 192.168.4.1 and you will be greated with the web based control interface.   



