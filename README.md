# ESP-Home-Serta-Bed-Control
Control Serta adjustable bed from ESPHome and Home Assistant


This is a re-do of my previous project that allowed me to control my adjustable bed from SmartThings. The Fall 2020 Update of smartThings removed support for custom Device Types.

Pre Req's: Home Assistant. ESPHome.

Software Installation:

1: Create a new ESPHome device (esp_bed_controller). Save and upload to your ESP32 or other devlice. This will create the necessary folders.

2: Upload the CC2500_REG.h and CC2500_VAL.h files to your /config/esphome directory

3: Upload the ESPBedController.h to the /config/esphome directory

4: Edit the YAML and paste in the ESPBedController.yaml code. (Make sure to update for your board type, platform, and wifi credentials)

5: Upload!

Hardware:
You'll need a CC2500 chip. (CC2500 IC Wireless RF Transceiver 2.4G Module). These are available on ebay and amazon for a few dollars. They are small, so you'll need to be good at soldering. If there is no PCB antenna, you can attach a short (2inch) peice of wire to the antenna hole.
Attach to ESP32. SPI Pins are defined in the ESPBedController.h file:

SCK_PIN 12

MISO_PIN 14

MOSI_PIN 13

CS_PIN 27

3.3V

Ground



This project will setup 8 switches in Home Assistant. Turning on each switch will send the command. The switches turn themselves off when the command is complete. 


For a more complete explaination of how this works and what controls are supported check out my previous project:
https://github.com/jlboygenius/SmartThingsSertaBedControl

If you would prefer to use the controller over MQTT, check out: https://github.com/kyfalcon/SmartThingsSertaBedControl



 

