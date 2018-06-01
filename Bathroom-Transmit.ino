/* 
 * Transmitter for bathroom. Uses magnetic door sensor to detect door open/closed and transmits
 * to receiver node for display.
 * 
 * Runs on Pro Mini 3.3V
 */

#include <nRF24L01.h>
#include <printf.h>
#include <RF24.h>
#include <RF24_config.h>
#include <SPI.h>

#include <Adafruit_NeoPixel.h>

#define debugMode TRUE // activate serial output and additional tests if set to true
#define SwitchPin 8 // door sensor is connected to this pin

int doorState = 0; // door zero means open, one means closed
bool hasChanged = false; // used to track changes that need transmitted
unsigned long resendTimer = 5*60*1000; // millis for resending the status, done as minutes * 60 seconds/min * 1000 millis/second to get milliseconds
unsigned long startMillis=millis(); // start time for checking against
unsigned long currentMillis=millis(); // current time


RF24 radio(9,10); // NRF24L01 used SPI pins + Pin 9 and 10
const uint64_t pipe = 0xE6E6E6E6E6E6; // Needs to be the same for communicating between 2 NRF24L01 

void setup(){
  if (debugMode) {
    Serial.begin(115200);
  }
  pinMode(SwitchPin, INPUT_PULLUP); // Define the sensor pin as an Input using Internal Pullups
  digitalWrite(SwitchPin,HIGH); // Set Pin to HIGH at beginning
  
  radio.begin(); // Start the NRF24L01
  radio.openWritingPipe(pipe); // Get NRF24L01 ready to transmit
}

void loop(){
  currentMillis=millis();
  int door=digitalRead(SwitchPin);
  if (door != doorState){ // the door has changed status
    doorState=door;
    hasChanged=true;
  }

  if (hasChanged) {
    radio.write(doorState,1);
    hasChanged=false;
    startMillis=millis(); // reset timer
  }
  
  if (currentMillis - startMillis >= resendTimer) {
    radio.write(doorState,1);
    startMillis=millis(); // reset timer
  }
  
  delay(1000);
}
