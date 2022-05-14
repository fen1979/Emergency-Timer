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
int timerResetDate = 10; // default time to stop minutes
int timerCount = 0;
byte menuNow = 1;
bool writeToLcd = true;
// for PNP Line
byte motorStatus = 1;
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
  analogWrite(relayOut1, 0);
  analogWrite(relayOut2, 0);
  analogWrite(relayOut3, 0);
  analogWrite(relayOut4, 0);
  digitalWrite(indication, HIGH);
  lcd.clear();
  lcd.setCursor(0, 0);
  scrollMessage(0, "Emergency stop time is " + String(timerResetDate) + " min", 350, 16);
}
// MESURMENT CASE
bool SensorMesurment() {
  return (analogRead(sensorIn) < 50) ? true : false;
}
// EMERGENCY CASE
void EmergencyCase() {
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
    PrintTextToLCD("EMERGENCY STOP", "Stop after " +
                   (((((t - (t % 60)) / 60) % 60) < 10) ? "0" + String(((t - (t % 60)) / 60) % 60) : String(((t - (t % 60)) / 60) % 60))
                   + ":" +
                   (((t % 60) < 10) ? "0" + String(t % 60) : String(t % 60)));
    timerCount++;
    delay(990);
  } else {
    analogWrite(relayOut1, 1024);
    analogWrite(relayOut2, 1024);
    analogWrite(relayOut3, 1024);
    analogWrite(relayOut4, 1024);
    digitalWrite(indication, LOW);
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
    motorPower[0] = motorPower[2] = 1024;
  }
  if (!SensorMesurment() && motorStatus == 1 && power) {
    PrintTextToLCD("PNP Line ON", "Motor 1,3 OFF");
    power = false;
    motorStatus = 2;
    motorPower[0] = motorPower[2] = 0;
  }
  if (SensorMesurment() && motorStatus == 2 && !power) {
    PrintTextToLCD("PNP Line ON", "Motor 2,4 ON");
    power = true;
    motorPower[1] = motorPower[3] = 1024;
  }
  if (!SensorMesurment() && motorStatus == 2 && power) {
    PrintTextToLCD("PNP Line ON", "Motor 2,4 OFF");
    power = false;
    motorStatus = 1;
    motorPower[1] = motorPower[3] = 0;
  }
  // changing pin state
  analogWrite(relayOut1, motorPower[0]);
  analogWrite(relayOut2, motorPower[1]);
  analogWrite(relayOut3, motorPower[2]);
  analogWrite(relayOut4, motorPower[3]);
}
// POWER OFF CASE
void PowerOffCase() {
  if (writeToLcd) {
    PrintTextToLCD("MOTOR POWER OFF", "Press menu to ON");
    writeToLcd = false;
  }
  analogWrite(relayOut1, 1024);
  analogWrite(relayOut2, 1024);
  analogWrite(relayOut3, 1024);
  analogWrite(relayOut4, 1024);
  digitalWrite(indication, LOW);
}
// EMERGENCY POWER STOP CASE
void EmergencyPowerOffCase() {
  analogWrite(relayOut1, 1024);
  analogWrite(relayOut2, 1024);
  analogWrite(relayOut3, 1024);
  analogWrite(relayOut4, 1024);
  digitalWrite(indication, LOW);
  lcd.clear();
  lcd.setCursor(0, 0);

  lcd.print("EMERGENCY STOP");
  lcd.setCursor(0, 1);
  scrollMessage(1, "Unpush STOP Button and reset the system!", 350, 16);
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
  if (digitalRead(emergencyStopButton) == LOW) {
    menuNow = 4;
    writeToLcd = true;
  } else {
    /* TODO: button stop like a fire btn and add menu code to else state */
  }

  // round cicle menu
  if (digitalRead(menuBtn) == LOW) {
    if (menuNow != 3)menuNow++;
    else menuNow = 1;
    writeToLcd = true;
    delay(350);
  }

  // work code calls
  if (menuNow == 1) EmergencyCase();
  if (menuNow == 2) MotorSwitching();
  if (menuNow == 3) PowerOffCase();
  if (menuNow == 4) EmergencyPowerOffCase();

  delay(10);
}
