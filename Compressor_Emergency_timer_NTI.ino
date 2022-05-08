//#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define sensorIn A1
#define relayOut1 A2
#define relayOut2 A3
#define relayOut3 A5
#define relayOut4 A7
#define indication 13
#define incBtn 12
#define decBtn 11

#define CLEAR 1
#define NOT_CLEAR 0

// time for stop the compressor
int timerResetDate = 10; // minutes
int timerCount = 0;
bool writeToLcd = false;

LiquidCrystal_I2C lcd(0x3E, 16, 2);

void setup() {
  pinMode(sensorIn, INPUT_PULLUP);
  pinMode(incBtn, INPUT_PULLUP);
  pinMode(decBtn, INPUT_PULLUP);

  pinMode(relayOut1, OUTPUT);
  pinMode(relayOut2, OUTPUT);
  pinMode(relayOut3, OUTPUT);
  pinMode(relayOut4, OUTPUT);
  pinMode(indication, OUTPUT);

  analogWrite(relayOut1, 1024);
  analogWrite(relayOut2, 1024);
  analogWrite(relayOut3, 1024);
  analogWrite(relayOut4, 1024);
  analogWrite(indication, 1024);

  lcd.begin();
  lcd.clear();
  lcd.setCursor(0, 0);
  String outTxt = "Current Emergency stop time is " + String(timerResetDate) + " minute";
  scrollMessage(0, outTxt, 350, 16);
}



void loop() {

  if (SensorMesurment()) {
    StartEmergencyTimer();

    if (writeToLcd) {
      WriteStaticTextToLcd(0, "Emergency", 1);
      WriteStaticTextToLcd(1, "Timer started!!!", 0);
      writeToLcd = false;
    }
  } else {
    analogWrite(indication, 1024);
    timerCount = 0;
    if (!writeToLcd) {
      WriteStaticTextToLcd(0, "Normal Work.", 1);
      WriteStaticTextToLcd(1, "Stop time " + String(timerResetDate) + " min", 0);
      writeToLcd = true;
    }
  }

  if (digitalRead(incBtn) == LOW && timerResetDate != 30) {
    timerResetDate++;
    WriteStaticTextToLcd(0, "Stop Time", 1);
    WriteStaticTextToLcd(1, String(timerResetDate) + " min", 0);
    writeToLcd = false;
    delay(800);
  }
  if (digitalRead(decBtn) == LOW && timerResetDate != 1) {
    timerResetDate--;
    WriteStaticTextToLcd(0, "Stop Time", 1);
    WriteStaticTextToLcd(1, String(timerResetDate) + " min", 0);
    writeToLcd = false;
    delay(800);
  }
  delay(10);
}

bool SensorMesurment() {
  return (analogRead(sensorIn) < 50) ? true : false;
}

void StartEmergencyTimer() {

  if (timerCount < (timerResetDate * 60)) {
    timerCount++;
    if ((timerCount % 2) == 0) {
      analogWrite(indication, 0);
    } else {
      analogWrite(indication, 1024);
    }

    uint32_t t = (timerResetDate * 60) - timerCount;
    uint32_t s = t % 60;
    t = (t - s) / 60;
    uint32_t m = t % 60;
    t = (t - m) / 60;
    uint32_t h = t;

    String timeToStop = String(h) + ":" + String(m) + ":" + String(s);
    WriteStaticTextToLcd(0, "Emergency Timer", 1);
    WriteStaticTextToLcd(1, "Stop - " + timeToStop, 0);

    delay(990);
  } else {
    analogWrite(relayOut1, 0);
    analogWrite(relayOut2, 0);
    analogWrite(relayOut3, 0);
    analogWrite(relayOut4, 0);
    analogWrite(indication, 0);
    lcd.clear();
    lcd.setCursor(0, 0);
    scrollMessage(0, "For restart need power off !!!", 350, 16);
  }
}

// screen static text
void WriteStaticTextToLcd(int row, String message, byte clearOrNot) {
  if (clearOrNot == 1) {
    lcd.clear();
  }
  lcd.setCursor(0, row);
  lcd.print(message);
}

// screen scrolling message function
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
