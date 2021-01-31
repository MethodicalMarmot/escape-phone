#include <SD.h>
#include <TMRpcm.h>

const int debounceTimeout = 100;
const int enableActiveState = LOW;
char* filename = "000.wav";

const byte enablePin = 2;
const byte tvPin = 3;

const byte phonePin = 9;
const byte chipSelect = 10;
TMRpcm tmrpcm;

// set up variables using the SD utility library functions:
Sd2Card card;
SdVolume volume;
SdFile root;

void setup() {
  Serial.begin(9600);

  pinMode(chipSelect, OUTPUT); // change this to 53 on a mega  // don't follow this!!
  digitalWrite(chipSelect, HIGH); // Add this line

  if (!card.init(SPI_HALF_SPEED, chipSelect)) {
    Serial.println("initialization failed");
    return;
  } else {
    Serial.println("Wiring is correct and a card is present.");
  }

  // print the type of card
  Serial.print("\nCard type: ");
  switch (card.type()) {
    case SD_CARD_TYPE_SD1:
      Serial.println("SD1");
      break;
    case SD_CARD_TYPE_SD2:
      Serial.println("SD2");
      break;
    case SD_CARD_TYPE_SDHC:
      Serial.println("SDHC");
      break;
    default:
      Serial.println("Unknown");
  }

  // Now we will try to open the 'volume'/'partition' - it should be FAT16 or FAT32
  if (!volume.init(card)) {
    Serial.println("Could not find FAT16/FAT32 partition.\nMake sure you've formatted the card");
    return;
  }

  if (!SD.begin(chipSelect)) {
    return;
  }

  tmrpcm.speakerPin = phonePin;
  tmrpcm.setVolume(5);
  tmrpcm.loop(0);
  tmrpcm.quality(1);

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
      tmrpcm.play(filename);
      digitalWrite(tvPin, HIGH);
      Serial.println("ENABLED");
    } else {
      tmrpcm.stopPlayback();
      digitalWrite(tvPin, LOW);
      Serial.println("DISABLED");
    }
  }

  if (state && !tmrpcm.isPlaying()) {
    tmrpcm.play(filename);
    Serial.println("REPEAT");
  }
}
