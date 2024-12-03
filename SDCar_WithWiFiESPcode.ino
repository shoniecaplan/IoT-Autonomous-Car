// ESP8266 Code
// Libraries
//=================================
// Ultrasonc Sensor 2 Pins
const int trig2 = 4;
const int echo2 = 5;
//=================================
// Array to store Pro Micro data
const byte numChars = 32;
char receivedChars[numChars];
//=================================
boolean newData = false;
//=================================

//=================================

//=================================

//=================================

//=================================





//=================================
void recvWithStartendids() {
  static boolean recvInProgress = false;
  static byte ndx = 0;
  char startid = '>';
  char endid = '<';
  char rc;

  while (Serial.available() > 0 && newData == false) {
    rc = Serial.read();

    if (recvInProgress == true) {
      if (rc != endid) {
        if (rc != 'T') {
          receivedChars[ndx] = rc;
          ndx++;

          if (ndx >= numChars) {
            ndx = numChars - 1;
          }
        } else {
          break; // Break in test 'T' cases
        }
      } else {
        receivedChars[ndx] = '\0'; // terminate the string
        recvInProgress = false;
        ndx = 0;
        newData = true;
      }
    } else if (rc == startid) {
      recvInProgress = true;
    }
  }
}
//=================================
void showNewData() {
  if (newData == true) {
    Serial.println(receivedChars);
    newData = false;
  }
}
//=================================

void setup() {
  Serial.begin(115200);
  delayMicroseconds(100);
}

void loop() {
  recvWithStartendids();
  showNewData();

}
