# Emergency-Timer
## Emergency timer for an device in NTI Lab
## Instal Liquid cristall lib files to project first!
* LCD pins (AtMega 328P) A4 - SDA, A5 - SCL  
* LED pin 13
* Sensor pin A1
* Relay pins A2, A3, A6, A7
* Buttons INC/DEC pins 11, 12
* time to stop by default 5 min
* Maximum time 30 min
* Minimum time 1 min
* LCD address 0x3E (hex)
## Pins setup for minimal code 
* Sensor input A1
* Button stop 8
* Relay outputs (digital) A2, A3, 6, 7
* LED red (error) 9
* LED Green (normal work) 10
* Reset pin for reset button
* Time duration 5 min ( without changes )
