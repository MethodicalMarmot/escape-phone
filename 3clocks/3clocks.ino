#include <AccelStepper.h>
#include <MultiStepper.h>

const int STEPS = 4320;                     // steps per revolution 4320? 1/12 deg
const int MINUTE_STEPS = (STEPS / 60);      // steps per one minute mark on clock face
const int INCREMENT_STEPS = MINUTE_STEPS / 2;


const byte secondsPerTurn = 5;              // how many seconds should be spent on a full turn

const int clock1HourTarget = MINUTE_STEPS * 5 * 9;        // 9 hour
const int clock1MinTarget = MINUTE_STEPS * 30;            // 30 min

const int clock2HourTarget = MINUTE_STEPS * 5 * 9;        // 9 hour
const int clock2MinTarget = MINUTE_STEPS * 30;            // 30 min

const int clock3HourTarget = MINUTE_STEPS * 5 * 9;        // 9 hour
const int clock4MinTarget = MINUTE_STEPS * 30;            // 30 min

const byte calibrationLevel = HIGH;
const byte movementButtonLevel = HIGH;
const byte lockRelayLockedLevel = LOW;
const unsigned int correctPositionHoldTimeout = 2000;     // 2 sec to clock arms should be in correct position

// lock relay
const byte lockRelay = 21;

// clock1
const byte clock1HourCalibrate = 0;
const byte clock1MinCalibrate = 1;
const byte clock1HourForward = 2;
const byte clock1HourBackward = 3;
const byte clock1MinForward = 4;
const byte clock1MinBackward = 5;
const AccelStepper clock1Hour(AccelStepper::FULL4WIRE, 6, 7, 8, 9);
const AccelStepper clock1Min(AccelStepper::FULL4WIRE, 10, 11, 12, 13);

// clock2
const byte clock2HourCalibrate = 52;
const byte clock2MinCalibrate = 50;
const byte clock2HourForward = 48;
const byte clock2HourBackward = 46;
const byte clock2MinForward = 44;
const byte clock2MinBackward = 42;
const AccelStepper clock2Hour(AccelStepper::FULL4WIRE, 40, 38, 36, 34);
const AccelStepper clock2Min(AccelStepper::FULL4WIRE, 32, 30, 28, 26);

// clock3
const byte clock3HourCalibrate = 53;
const byte clock3MinCalibrate = 51;
const byte clock3HourForward = 49;
const byte clock3HourBackward = 47;
const byte clock3MinForward = 45;
const byte clock3MinBackward = 43;
const AccelStepper clock3Hour(AccelStepper::FULL4WIRE, 41, 39, 37, 35);
const AccelStepper clock3Min(AccelStepper::FULL4WIRE, 33, 31, 29, 27);

void setup() {
    pinMode(lockRelay, OUTPUT);

    pinMode(clock1HourCalibrate, INPUT_PULLUP);
    pinMode(clock1MinCalibrate, INPUT_PULLUP);
    pinMode(clock1HourForward, INPUT_PULLUP);
    pinMode(clock1HourBackward, INPUT_PULLUP);
    pinMode(clock1MinForward, INPUT_PULLUP);
    pinMode(clock1MinBackward, INPUT_PULLUP);
    clock1Hour.setMaxSpeed(STEPS / secondsPerTurn);
    clock1Min.setMaxSpeed(STEPS / secondsPerTurn);

    pinMode(clock2HourCalibrate, INPUT_PULLUP);
    pinMode(clock2MinCalibrate, INPUT_PULLUP);
    pinMode(clock2HourForward, INPUT_PULLUP);
    pinMode(clock2HourBackward, INPUT_PULLUP);
    pinMode(clock2MinForward, INPUT_PULLUP);
    pinMode(clock2MinBackward, INPUT_PULLUP);
    clock2Hour.setMaxSpeed(STEPS / secondsPerTurn);
    clock2Min.setMaxSpeed(STEPS / secondsPerTurn);

    pinMode(clock3HourCalibrate, INPUT_PULLUP);
    pinMode(clock3MinCalibrate, INPUT_PULLUP);
    pinMode(clock3HourForward, INPUT_PULLUP);
    pinMode(clock3HourBackward, INPUT_PULLUP);
    pinMode(clock3MinForward, INPUT_PULLUP);
    pinMode(clock3MinBackward, INPUT_PULLUP);
    clock3Hour.setMaxSpeed(STEPS / secondsPerTurn);
    clock3Min.setMaxSpeed(STEPS / secondsPerTurn);
}

unsigned int lastIncorrectPositionMillis = millis();
bool isCalibrated = false;
void loop() {
  if (!isCalibrated) {
    digitalWrite(lockRelay, lockRelayLockedLevel);
    
    calibrate(clock1Hour, clock1HourCalibrate);
    calibrate(clock1Min, clock1MinCalibrate);

    calibrate(clock2Hour, clock2HourCalibrate);
    calibrate(clock2Min, clock2MinCalibrate);

    calibrate(clock3Hour, clock3HourCalibrate);
    calibrate(clock3Min, clock3MinCalibrate);

    isCalibrated = true;
  }

  boolean isCorrectPosition = isCalibrated &&
    atPosition(clock1Hour, clock1HourTarget) && atPosition(clock1Min, clock1MinTarget) &&
    atPosition(clock2Hour, clock2HourTarget) && atPosition(clock2Min, clock2MinTarget) &&
    atPosition(clock3Hour, clock3HourTarget) && atPosition(clock3Min, clock3MinTarget);

  unsigned int currentMillis = millis();
  if (isCorrectPosition) {
    if (currentMillis - lastIncorrectPositionMillis > correctPositionHoldTimeout) {

      digitalWrite(lockRelay, lockRelayLockedLevel == HIGH ? LOW : HIGH);

      while (isCalibrated) {
        isCalibrated =
          !isPushed(clock1HourForward) &&
          !isPushed(clock1HourBackward) &&
          !isPushed(clock1MinForward) &&
          !isPushed(clock1MinBackward) &&
          
          !isPushed(clock2HourForward) &&
          !isPushed(clock2HourBackward) &&
          !isPushed(clock2MinForward) &&
          !isPushed(clock2MinBackward) &&
          
          !isPushed(clock3HourForward) &&
          !isPushed(clock3HourBackward) &&
          !isPushed(clock3MinForward) &&
          !isPushed(clock3MinBackward);
      }
    }
  }
  else {
    lastIncorrectPositionMillis = currentMillis;
  }

  moveOnPushed(clock1Hour, clock1HourForward, INCREMENT_STEPS);
  moveOnPushed(clock1Hour, clock1HourBackward, -INCREMENT_STEPS);
  moveOnPushed(clock1Min, clock1MinForward, INCREMENT_STEPS);
  moveOnPushed(clock1Min, clock1MinBackward, -INCREMENT_STEPS);

  moveOnPushed(clock2Hour, clock2HourForward, INCREMENT_STEPS);
  moveOnPushed(clock2Hour, clock2HourBackward, -INCREMENT_STEPS);
  moveOnPushed(clock2Min, clock2MinForward, INCREMENT_STEPS);
  moveOnPushed(clock2Min, clock2MinBackward, -INCREMENT_STEPS);

  moveOnPushed(clock3Hour, clock3HourForward, INCREMENT_STEPS);
  moveOnPushed(clock3Hour, clock3HourBackward, -INCREMENT_STEPS);
  moveOnPushed(clock3Min, clock3MinForward, INCREMENT_STEPS);
  moveOnPushed(clock3Min, clock3MinBackward, -INCREMENT_STEPS);

}

void calibrate(AccelStepper stepper, byte calibrationPin) {
  while (digitalRead(calibrationPin) != calibrationLevel) {
    move(stepper, INCREMENT_STEPS);
  }

  stepper.setCurrentPosition(0);
}

void moveOnPushed(AccelStepper stepper, byte pin, int steps) {
  if (isPushed(pin)) {
    move(stepper, steps);
  }
}

void move(AccelStepper stepper, int steps) {
  stepper.move(steps);
  stepper.runToPosition();
}

boolean isPushed(byte pin) {
  return digitalRead(pin) == movementButtonLevel;
}

boolean atPosition(AccelStepper stepper, int target) {
  return (int(stepper.currentPosition() / 60) % target) == 0;
}
