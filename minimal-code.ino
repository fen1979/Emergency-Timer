#define sensorIn A1
#define emergencyStopButton 8
#define relayOut1 A2
#define relayOut2 A3
#define relayOut3 6
#define relayOut4 7
#define ledError 9 // red LED
#define ledWork 10 // green LED

byte inPin[] = {A1, 8};
byte outPin[] = {A2, A3, 6, 7, 9, 10};
int timerResetDate = 1; // default time to stop minutes = 5
int timerCount = 0;
bool error = false;

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

  // turn motors switch to ON
  digitalWrite(relayOut1, LOW);
  if (SensorMesurment()) delay(2000);
  
  digitalWrite(relayOut2, LOW);
  if (SensorMesurment()) delay(2000);
  
  digitalWrite(relayOut3, LOW);
  if (SensorMesurment()) delay(2000);
  
  digitalWrite(relayOut4, LOW);

  // turn green LED ON
  digitalWrite(ledWork, HIGH);
}



void loop() {
  // Emergency Stop Button
  if (digitalRead(emergencyStopButton) == LOW ) {
    // set motors to OFF state
    digitalWrite(relayOut1, HIGH);
    digitalWrite(relayOut2, HIGH);
    digitalWrite(relayOut3, HIGH);
    digitalWrite(relayOut4, HIGH);
    // turn green LED OFF
    digitalWrite(ledWork, LOW);
    error = true;
  }

  // emergency case
  if (SensorMesurment() && !error) {
    if (timerCount < (timerResetDate * 60)) {
      // blink with green LED when timer work
      digitalWrite(ledWork, digitalRead(ledWork) ^ 1);
      timerCount++;
      delay(990);
    } else {
      // motors switch to OFF
      digitalWrite(relayOut1, HIGH);
      digitalWrite(relayOut2, HIGH);
      digitalWrite(relayOut3, HIGH);
      digitalWrite(relayOut4, HIGH);
      error = true;
    }
  }

  // back to normal work case
  if (!SensorMesurment() && !error) {
    digitalWrite(ledWork, HIGH);
    timerCount = 0;
  }


  // something went wrong in system
  if (error) {
    digitalWrite(ledError, digitalRead(ledError) ^ 1);
    delay(200);
  }

  delay(10);
}
