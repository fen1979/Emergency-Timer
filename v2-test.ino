#include <LiquidCrystal_I2C.h>
#define sensorIn A1
#define relayOut1 A2
#define relayOut2 A3
#define relayOut3 6
#define relayOut4 7
#define saveBtn 8
#define decBtn 9
#define menuBtn 10
#define incBtn 11
#define indication 13

#define CLEAR_LCD 1
#define NO_CLEAR_LCD 0

byte inPin[] = {A1, 8, 9, 10, 11};
byte outPin[] = {13, A2, A3, 6, 7};
int timerResetDate = 10; // minutes
int timerCount = 0;
bool writeToLcd = false;
byte menuNow = 1;
// for PNP Line
byte motorStatus = 1;

LiquidCrystal_I2C lcd(0x3E, 16, 2);

void setup() {
  for (byte i = 0; i < sizeof(inPin); i++) {
    pinMode(inPin[i], INPUT);
  }

  for (byte i = 0; i < sizeof(outPin); i++) {
    pinMode(outPin[i], OUTPUT);
  }

  analogWrite(relayOut1, 0);
  analogWrite(relayOut2, 0);
  analogWrite(relayOut3, 0);
  analogWrite(relayOut4, 0);
  analogWrite(indication, 1024);

  lcd.begin();
  lcd.clear();
  lcd.setCursor(0, 0);
  String outTxt = "Emergency stop time is " + String(timerResetDate) + " min";
  // scrollMessage(0, outTxt, 350, 16);
}

void loop() {
  // menu function
  Menu();
  // function for emergency stop
  if (menuNow == 1) {
    EmergencyCase();
  }
  // function for PNP Line
  if (menuNow == 2) {
    MotorSwitching();
  }
  delay(10);
}

bool SensorMesurment() {
  return (analogRead(sensorIn) < 50) ? true : false;
}

void StartEmergencyTimer() {
  if (timerCount < (timerResetDate * 60)) {
    timerCount++;
    analogWrite(indication, ((timerCount % 2) == 0) ? 1024 : 0);

    int t = (timerResetDate * 60) - timerCount;
    int s = t % 60;
    t = (t - s) / 60;
    int m = t % 60;
    t = (t - m) / 60;
    int h = t;

    String timeToStop = String(h) + ":" + String(m) + ":" + String(s);
    WriteStaticTextToLcd(0, "Emergency Timer", CLEAR_LCD);
    WriteStaticTextToLcd(1, "Stop - " + timeToStop, NO_CLEAR_LCD);

    delay(990);
  } else {
    analogWrite(relayOut1, 1024);
    analogWrite(relayOut2, 1024);
    analogWrite(relayOut3, 1024);
    analogWrite(relayOut4, 1024);
    analogWrite(indication, 0);
    lcd.clear();
    lcd.setCursor(0, 0);
    scrollMessage(0, "For restart need power off !!!", 350, 16);
  }
}
void EmergencyCase() {
  if (SensorMesurment()) {
    StartEmergencyTimer();

    if (writeToLcd) {
      WriteStaticTextToLcd(0, "Emergency", CLEAR_LCD);
      WriteStaticTextToLcd(1, "Timer started!!!", NO_CLEAR_LCD);
      writeToLcd = false;
    }
  } else {
    analogWrite(indication, 1024);
    timerCount = 0;
    if (!writeToLcd) {
      WriteStaticTextToLcd(0, "Normal Work.", CLEAR_LCD);
      WriteStaticTextToLcd(1, "Stop time " + String(timerResetDate) + " min", NO_CLEAR_LCD);
      writeToLcd = true;
    }
  }
}

void MotorSwitching() {
  analogWrite(indication, 0);
  if (SensorMesurment()) {
    motorStatus = (motorStatus == 1) ? 2 : 1;
  }

  switch (motorStatus) {
    case 1: {
        analogWrite(relayOut1, 1024);
        analogWrite(relayOut2, 0);
        analogWrite(relayOut3, 1024);
        analogWrite(relayOut4, 0);
      } break;
    case 2: {
        analogWrite(relayOut1, 0);
        analogWrite(relayOut2, 1024);
        analogWrite(relayOut3, 0);
        analogWrite(relayOut4, 1024);
      } break;
  }
}

// menu buttons and options
void Menu() {
  if (digitalRead(menuBtn) == LOW) {
    menuNow = (menuNow == 1) ? 2 : 1;
    delay(500);
  }

  if (digitalRead(incBtn) == LOW || digitalRead(decBtn) == LOW) {
    // increase/decrease time to stop
    WriteStaticTextToLcd(0, "Stop Time", CLEAR_LCD);
    WriteStaticTextToLcd(1, String(timerResetDate) + " min", NO_CLEAR_LCD);

    if (digitalRead(incBtn) == LOW && timerResetDate != 30) {
      timerResetDate++;
    }
    if (digitalRead(decBtn) == LOW && timerResetDate != 1) {
      timerResetDate--;
    }

    writeToLcd = false;
    delay(250);
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
