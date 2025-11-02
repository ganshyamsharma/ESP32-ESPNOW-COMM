# ESP32-ESPNOW-COMM
ESP32 based IoT solution for water tank full indication and alarm annunciation using wireless ESP-NOW protocol. The system consists of two units; 
unit 1 connected to level switch of water tank at the roof and unit 2 at a convinient location in ground floor. The microcontrollers are programmed using Arduino IDE.
## Unit 1
- Consists of a push button and level switch connection connected to GPIO4 and GPIO21 respectively. 
- The push button can be used to simulate tank full condition for testing purpose.
- Uses ESP-NOW communication protocol to transmit the tank level status to unit 2.
## Unit 2
- Consists of a 4 digit LED, MQ-6 flammable gas sensor, two push buttons (Accept & Test) and two piezo buzzers.
- Any alarm condition will show a custom text on LED display and hooters will start buzzing. Pressing Accept PB will silent the hooter, 
the display will be cleared only when the alarm condition no longer prevails.
- Three alarm conditions are possible which are listed according to their priority.
-  Gas Leak: If gas concentration exceeds set limit. Display shows "LPG_" and hooter buzzes continuously. 
-  Communication Error: If connection with unit 1 not established. Display shows "Conn" and hooter buzzes continuously.
-  Tank Full: Water tank is full. Display shows "FULL" and hooter buzzes continuously.
- Pressing Test PB when no alarm condition is present will make all segments on LED display glow and hooters will start buzzing to check component functionality.