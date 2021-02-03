#include <Arduino.h>
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>

const int debounceTimeout = 100;
const int enableActiveState = LOW;
const int relayActiveState = LOW;
const int track = 1;

const byte enablePin = 2;
const byte tvPin = 3;

SoftwareSerial mySoftwareSerial(10, 11); // RX, TX
DFRobotDFPlayerMini myDFPlayer;

void setup() {
  Serial.begin(9600);
  mySoftwareSerial.begin(9600);
  
  delay(1000);

  if (!myDFPlayer.begin(mySoftwareSerial)) {
    Serial.println("Unable to begin:");
    Serial.println("1.Please recheck the connection!");
    Serial.println("2.Please insert the SD card!");
    while(true);
  }
  Serial.println("DFPlayer online");

  myDFPlayer.setTimeOut(500); //Set serial communictaion time out 500ms

  myDFPlayer.EQ(DFPLAYER_EQ_NORMAL);
  myDFPlayer.outputDevice(DFPLAYER_DEVICE_SD);
  myDFPlayer.enableLoop();
  myDFPlayer.volume(20);

  pinMode(enablePin, INPUT_PULLUP);
  pinMode(tvPin, OUTPUT);
  
  Serial.println("INIT");
}

int lastValue = enableActiveState == HIGH ? LOW : HIGH;
unsigned long prevValueMillis = millis();
void loop() {
  unsigned long currentMillis = millis();

  int value = currentMillis - prevValueMillis > debounceTimeout ? digitalRead(enablePin) : lastValue;
  
  boolean state = value == enableActiveState;
    
  if (value != lastValue) {
    prevValueMillis = currentMillis;
    lastValue = value;

    if (state) {
      myDFPlayer.loop(track);
      digitalWrite(tvPin, relayActiveState);
      Serial.println("ENABLED");
    } else {
      myDFPlayer.disableLoopAll();
      digitalWrite(tvPin, relayActiveState == HIGH ? LOW : HIGH);
      Serial.println("DISABLED");
    }
  }
}
