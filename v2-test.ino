#include <LiquidCrystal_I2C.h>
#define sensorIn A1
#define relayOut1 A2
#define relayOut2 A3
#define relayOut3 6
#define relayOut4 7
#define emergencyStopButton 8
#define decBtn 9
#define menuBtn 10
#define incBtn 11
#define indication 13

byte inPin[] = {A1, 8, 9, 10, 11};
byte outPin[] = {13, A2, A3, 6, 7};
int timerResetDate = 5; // default time to stop minutes
int timerCount = 0;
byte menuNow = 1;
bool writeToLcd = true;
// for PNP Line
byte motorStatus = 1;
bool motorOnOffState = false;
int motorPower[4] = {0, 0, 0, 0};
bool power = false;

LiquidCrystal_I2C lcd(0x3E, 16, 2);

void setup() {
  lcd.begin();

  for (byte i = 0; i < sizeof(inPin); i++) {
    pinMode(inPin[i], INPUT_PULLUP);
  }
  for (byte i = 0; i < sizeof(outPin); i++) {
    pinMode(outPin[i], OUTPUT);
  }
  // switch motors to ON state
  motorOnOffState = MotorOnOff(true);

  lcd.clear();
  lcd.setCursor(0, 0);
  // scrollMessage(0, "Emergency stop time is " + String(timerResetDate) + " min", 350, 16);
}
// MESURMENT CASE
bool SensorMesurment() {
  return (analogRead(sensorIn) < 50) ? true : false;
}
// EMERGENCY CASE
void EmergencyCase() {
  // if motor power is OFF turn it to ON
  if (!motorOnOffState) motorOnOffState = MotorOnOff(true);

  if (SensorMesurment()) {
    StartEmergencyTimer();
    writeToLcd = true;
  } else {
    digitalWrite(indication, HIGH);
    timerCount = 0;
    if (writeToLcd) {
      PrintTextToLCD("NORMAL WORK", "Stop time " + String(timerResetDate) + " min");
      writeToLcd = false;
    }
  }
  // increase/decrease time to stop actions
  if (digitalRead(incBtn) == LOW || digitalRead(decBtn) == LOW) {
    PrintTextToLCD("Stop Time", String(timerResetDate) + " min");
    if (digitalRead(incBtn) == LOW && timerResetDate != 30) timerResetDate++;
    if (digitalRead(decBtn) == LOW && timerResetDate != 1) timerResetDate--;
    writeToLcd = true;
    delay(350);
  }
}
// EMERGENCY TIMER CASE
void StartEmergencyTimer() {
  if (timerCount < (timerResetDate * 60)) {
    digitalWrite(indication, digitalRead(indication) ^ 1);
    int t = (timerResetDate * 60) - timerCount;

    if (!writeToLcd) {
      PrintTextToLCD("EMERGENCY STOP", "Stop after ");
      writeToLcd = true;
    }
    lcd.setCursor(11, 1);
    lcd.print((((((t - (t % 60)) / 60) % 60) < 10) ? "0" + String(((t - (t % 60)) / 60) % 60) : String(((t - (t % 60)) / 60) % 60))
              + ":" +
              (((t % 60) < 10) ? "0" + String(t % 60) : String(t % 60)));
    timerCount++;
    delay(990);
  } else {
    // switch motors to OFF state
    motorOnOffState = MotorOnOff(false);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("EMERGENCY STOP");
    lcd.setCursor(0, 1);
    scrollMessage(1, "For restart need power off !!!", 350, 16);
  }
}
// PNP LINE CASE
void MotorSwitching() {
  if (writeToLcd) {
    PrintTextToLCD("PNP Line ON", "");
    writeToLcd = false;
  }
  if (SensorMesurment() && motorStatus == 1 && !power) {
    PrintTextToLCD("PNP Line ON", "Motor 1,3 ON");
    power = true;
    motorPower[0] = motorPower[2] = HIGH;
  }
  if (!SensorMesurment() && motorStatus == 1 && power) {
    PrintTextToLCD("PNP Line ON", "Motor 1,3 OFF");
    power = false;
    motorStatus = 2;
    motorPower[0] = motorPower[2] = LOW;
  }
  if (SensorMesurment() && motorStatus == 2 && !power) {
    PrintTextToLCD("PNP Line ON", "Motor 2,4 ON");
    power = true;
    motorPower[1] = motorPower[3] = HIGH;
  }
  if (!SensorMesurment() && motorStatus == 2 && power) {
    PrintTextToLCD("PNP Line ON", "Motor 2,4 OFF");
    power = false;
    motorStatus = 1;
    motorPower[1] = motorPower[3] = LOW;
  }
  // changing pin state
  digitalWrite(relayOut1, motorPower[0]);
  digitalWrite(relayOut2, motorPower[1]);
  digitalWrite(relayOut3, motorPower[2]);
  digitalWrite(relayOut4, motorPower[3]);
}
// POWER OFF CASE
void PowerOffCase() {
  if (writeToLcd) {
    PrintTextToLCD("MOTOR POWER OFF", "Press menu to ON");
    writeToLcd = false;
  }
  // switch motors to OFF state
  motorOnOffState = MotorOnOff(false);
}
// POWER ON WHEN MOTORS IS SHOOTDOWNED
bool MotorOnOff(bool motorState) {
  if (motorState) {
    // motors switch to ON
    digitalWrite(relayOut1, HIGH);
    digitalWrite(relayOut2, HIGH);
    digitalWrite(relayOut3, HIGH);
    digitalWrite(relayOut4, HIGH);
    digitalWrite(indication, LOW);
  }
  if (!motorState) {
    // motors switch to OFF
    digitalWrite(relayOut1, LOW);
    digitalWrite(relayOut2, LOW);
    digitalWrite(relayOut3, LOW);
    digitalWrite(relayOut4, LOW);
    digitalWrite(indication, HIGH);
  }
  return motorState;
}
// EMERGENCY POWER STOP CASE
void EmergencyPowerOffCase() {
  // switch motors to OFF state
  motorOnOffState = MotorOnOff(false);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("EMERGENCY STOP");
  lcd.setCursor(0, 1);
  scrollMessage(1, "Unpush STOP Button and press MENU button to start!", 350, 16);
}
// LINE PRINT TEXT CASE
void PrintTextToLCD(String topRow, String bottomRow) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(topRow);
  lcd.setCursor(0, 1);
  lcd.print(bottomRow);
}
// SCROLLING PRINT TEXT CASE
void scrollMessage(int row, String message, int delayTime, int totalColumns) {
  for (int i = 0; i < totalColumns; i++) {
    message = " " + message;
  }
  message = message + " ";
  for (int position = 0; position < message.length(); position++) {
    lcd.setCursor(0, row);
    lcd.print(message.substring(position, position + totalColumns));
    delay(delayTime);
  }
}

void loop() {
  // Emergency Stop Button
  if (digitalRead(emergencyStopButton) == LOW ) {
    menuNow = 4;
    writeToLcd = true;
  } else {
    // round cicle menu
    if (digitalRead(menuBtn) == LOW) {
      if (menuNow != 3)menuNow++;
      else menuNow = 1;
      writeToLcd = true;
      delay(350);
    }
  }

  // work code calls
  if (menuNow == 1) EmergencyCase();
  if (menuNow == 2) MotorSwitching();
  if (menuNow == 3) PowerOffCase();
  if (menuNow == 4) EmergencyPowerOffCase();

  delay(10);
}
