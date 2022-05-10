#include <LiquidCrystal_I2C.h>

#define sensorIn A1
#define relayOut1 A2
#define relayOut2 A3
#define relayOut3 6
#define relayOut4 7
#define indication 13

#define saveBtn 8
#define decBtn 9
#define menuBtn 10
#define incBtn 11

#define CLEAR_LCD 1
#define NO_CLEAR_LCD 0


// time for stop the compressor
int timerResetDate = 10; // minutes
int timerCount = 0;
bool writeToLcd = false;
byte menuPositioner = 0;
String menu[] = {
"1) Stop Time",
"2) ",
"3) ",
"4) "
  };

LiquidCrystal_I2C lcd(0x3E, 16, 2);

void setup() {
  pinMode(sensorIn, INPUT);
  pinMode(incBtn, INPUT);
  pinMode(decBtn, INPUT);
  pinMode(menuBtn, INPUT);
  pinMode(saveBtn, INPUT);

  pinMode(relayOut1, OUTPUT);
  pinMode(relayOut2, OUTPUT);
  pinMode(relayOut3, OUTPUT);
  pinMode(relayOut4, OUTPUT);
  pinMode(indication, OUTPUT);

  analogWrite(relayOut1, 0);
  analogWrite(relayOut2, 0);
  analogWrite(relayOut3, 0);
  analogWrite(relayOut4, 0);
  analogWrite(indication, 1024);

  lcd.begin();
  lcd.clear();
  lcd.setCursor(0, 0);
  String outTxt = "Current Emergency stop time is " + String(timerResetDate) + " minute";
  // scrollMessage(0, outTxt, 350, 16);
}



void loop() {
  // menu function
  isMenuCalled();

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

// menu buttons and options
void isMenuCalled() {

  if (digitalRead(menuBtn) == LOW) {
    delay(250);
    if ( menuPositioner != 4) {
      menuPositioner++;
    } else {
      menuPositioner = 0;
    }
  }

  switch (menuPositioner) {
    // timer inc/dec
    case 1: {
        if (digitalRead(incBtn) == LOW || digitalRead(decBtn) == LOW) {
          WriteStaticTextToLcd(0, String(menuPositioner) + ") Stop Time", CLEAR_LCD);
          WriteStaticTextToLcd(1, String(timerResetDate) + " min", NO_CLEAR_LCD);
          if (digitalRead(incBtn) == LOW && timerResetDate != 30) {
            timerResetDate++;
            writeToLcd = false;
            delay(250);
          }

          if (digitalRead(decBtn) == LOW && timerResetDate != 1) {
            timerResetDate--;
            writeToLcd = false;
            delay(250);
          }
        }
      } break;
    // stop all works
    case 2: {
        WriteStaticTextToLcd(0, String(menuPositioner) + ") Stop Time", CLEAR_LCD);
        WriteStaticTextToLcd(1, String(timerResetDate) + " min", NO_CLEAR_LCD);
      } break;
    //
    case 3: {
        WriteStaticTextToLcd(0, String(menuPositioner) + ") Stop Time", CLEAR_LCD);
        WriteStaticTextToLcd(1, String(timerResetDate) + " min", NO_CLEAR_LCD);
      } break;
    //
    case 4: {
        WriteStaticTextToLcd(0, String(menuPositioner) + ") Stop Time", CLEAR_LCD);
        WriteStaticTextToLcd(1, String(timerResetDate) + " min", NO_CLEAR_LCD);
      } break;

    default : break;
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
