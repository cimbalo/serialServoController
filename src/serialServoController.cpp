/*
 serialServoController.cpp - Serial servo controller based on https://github.com/adafruit/Adafruit-PWM-Servo-Driver-Library/blob/master/examples/servo/servo.pde

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <Wire.h>
#include <Arduino.h>
#include <Adafruit_PWMServoDriver.h>

// called this way, it uses the default address 0x40
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
// you can also call it with a different address you want
//Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x41);

// Depending on your servo make, the pulse width min and max may vary, you
// want these to be as small/large as possible without hitting the hard stop
// for max range. You'll have to tweak them as necessary to match the servos you
// have!
#define SERVOMIN  150 // this is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  600 // this is the 'maximum' pulse length count (out of 4096)

// our servo # counter
uint8_t servonum = 0;

boolean newData = false;

const byte numChars = 32;
char receivedChars[numChars];	// an array to store the received data
String receivedString;

void readLine() {
  static byte ndx = 0;
  char endMarker = '\n';
  char rc;

  while (Serial.available() > 0 && newData == false) {
    rc = Serial.read();

    if (rc != endMarker) {
      receivedChars[ndx] = rc;
      ndx++;
      if (ndx >= numChars) {
        ndx = numChars - 1;
      }
    }
    else {
      receivedChars[ndx] = '\0'; // terminate the string
      ndx = 0;
      newData = true;
    }
  }
}

String getValue(String data, char separator, int index)
{
  int maxIndex = data.length()-1;
  int j=0;
  String chunkVal = "";

  for(int i=0; i<=maxIndex && j<=index; i++)
  {
    chunkVal.concat(data[i]);

    if(data[i]==separator)
    {
      j++;

      if(j>index)
      {
        chunkVal.trim();
        return chunkVal;
      }

      chunkVal = "";
    }
  }
}


int countOccurrences( char* s, char c )
{
  return *s == '\0' ? 0 : countOccurrences( s + 1, c ) + (*s == c);
}

void setup() {
   Serial.begin(9600);
   Serial.println("Servos controller initialized");

   pwm.begin();

   pwm.setPWMFreq(60);  // Analog servos run at ~60 Hz updates

   yield();
}


void loop() {
  readLine();
  if (newData == true) {
    newData = false;
    int argc= countOccurrences(receivedChars, ' ');
    receivedString = receivedChars;
    Serial.print("Command received:");
    Serial.println(receivedString);
    Serial.print("Command args length:");
    Serial.println(argc);

    String command = getValue(receivedString, ' ', 0);
    if (command == "set"){
      switch (argc){
      case 2:
        pwm.setPWM(servonum, getValue(receivedString, ' ', 1).toInt(), getValue(receivedString, ' ', 2).toInt());
        Serial.println("Servo on pin " + getValue(receivedString, ' ', 1) + " setted to: " + getValue(receivedString, ' ', 2));
        break;
      default:
        Serial.print("Exception: Wrong arg number in set! Expecting 2, ");
        Serial.print(argc);
        Serial.println(" received");
        break;
      }
    }
    else{
      Serial.println("Exception: Unknown command '" + command + "'");
    }

  }
}
