#define sensorIn A1
#define emergencyStopButton 5
#define relayOut1 6
#define relayOut2 7
#define relayOut3 8
#define relayOut4 9
#define ledError 10 // red LED
#define ledWork 11 // green LED
#define L LOW
#define H HIGH

byte inPin[] = {A1, 5};
byte outPin[] = {6, 7, 8, 9, 10, 11};
int timerResetDate = 5; // default time to stop minutes = 5
int timerCount = 0;
bool error = false;
bool started = false;

// MESURMENT CASE
bool SensorMesurment() {
  return (analogRead(sensorIn) < 50) ? true : false;
}

void setup() {
  for (byte i = 0; i < sizeof(inPin); i++) {
    pinMode(inPin[i], INPUT_PULLUP);
  }
  for (byte i = 0; i < sizeof(outPin); i++) {
    pinMode(outPin[i], OUTPUT);
  }

  // turn motors switch to ON IF LOW PRESSURE IS EXIST
  if (SensorMesurment()) {
    digitalWrite(relayOut1, H);
    delay(2000);
    digitalWrite(relayOut2, H);
    delay(2000);
    digitalWrite(relayOut3, H);
    delay(2000);
    digitalWrite(relayOut4, H);
  }
  // turn green LED ON
  digitalWrite(ledWork, H);
}

//
void RelayToOn() {
  if (SensorMesurment() && !started) {
    digitalWrite(relayOut1, H);
    delay(2000);
    digitalWrite(relayOut2, H);
    delay(2000);
    digitalWrite(relayOut3, H);
    delay(2000);
    digitalWrite(relayOut4, H);
    started = true;
    timerCount = timerCount + 6;
  }
}
//
void RelayToOff() {
  digitalWrite(relayOut1, L);
  digitalWrite(relayOut2, L);
  digitalWrite(relayOut3, L);
  digitalWrite(relayOut4, L);
}

// ----------------------------------------------------------------- //
void loop() {
  // Emergency Stop Button
  if (digitalRead(emergencyStopButton) == LOW ) {
    // set motors relay to OFF state
    digitalWrite(relayOut1, L);
    digitalWrite(relayOut2, L);
    digitalWrite(relayOut3, L);
    digitalWrite(relayOut4, L);
    // turn green LED OFF
    digitalWrite(ledWork, L);
    error = true;
  }

  // emergency case
  if (SensorMesurment() && !error) {
    RelayToOn();
    if (timerCount < (timerResetDate * 60)) {
      // blink with green LED when timer work
      digitalWrite(ledWork, digitalRead(ledWork) ^ 1);
      timerCount++;
      delay(990);
    } else {
      // motors relay switch to OFF
      digitalWrite(relayOut1, L);
      digitalWrite(relayOut2, L);
      digitalWrite(relayOut3, L);
      digitalWrite(relayOut4, L);
      error = true;
    }
  }

  // back to normal work case
  if (!SensorMesurment() && !error && started) {
    RelayToOff();
    digitalWrite(ledWork, H);
    timerCount = 0;
    started = false;
  }


  // something went wrong in system
  if (error) {
    digitalWrite(ledError, digitalRead(ledError) ^ 1);
    delay(200);
  }

  delay(10);
}
