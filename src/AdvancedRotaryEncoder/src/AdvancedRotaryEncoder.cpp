/*
                               ________         _______
                  PinA _______|       |_______|       |______ PinA
          CW  -->          _______         _______         __
                  PinB ___|       |_______|       |_______|   PinB

                           _______         _______         __
                  PinA ___|       |_______|       |_______|   PinA
          CCW <--              _______         _______
                  PinB _______|       |_______|       |______ PinB



           oldAB     newAB      oldABnewAB      Encoder State
          -------   ---------   --------------  -------------
            00         00            0000       nothing
            00         01            0001       CW
            00         10            0010       CCW
            00         11            0011       invalid
            01         00            0100       CCW
            01         01            0101       nothing
            01         10            0110       invalid
            01         11            0111       CW
            10         00            1000       CW
            10         01            1001       invalid
            10         10            1010       nothing
            10         11            1011       CCW
            11         00            1100       invalid
            11         01            1101       CCW
            11         10            1110       CW
            11         11            1111       nothing

          - CW:     0b0001, 0b0111, 0b1000, 0b1110
          - CCW:    0b0010, 0b0100, 0b1011, 0b1101
*/
/***************************************************************************************************/

#include "AdvancedRotaryEncoder.h"

std::vector<AdvancedRotaryEncoder *> AdvancedRotaryEncoder::instances;

AdvancedRotaryEncoder::AdvancedRotaryEncoder(int pinA, int pinB, int pinButton) {
    this->pinA = pinA;
    this->pinB = pinB;
    this->pinButton = pinButton;
    this->isStarted = false;

    AdvancedRotaryEncoder::instances.push_back(this);
}

void AdvancedRotaryEncoder::staticInterruptHandler() {
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);


    // If this commented out section enabled the LED stays lit
    /*
    if (AdvancedRotaryEncoder::instances.empty()) {
        return;
    }

    for (auto &element : AdvancedRotaryEncoder::instances) {
        element->update();
    }
    */
}


void AdvancedRotaryEncoder::start() {
    pinMode(this->pinA, INPUT_PULLUP); //enable internal pull-up resistors
    pinMode(this->pinB, INPUT_PULLUP);
    pinMode(this->pinButton, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(this->pinA), staticInterruptHandler, CHANGE);
    attachInterrupt(digitalPinToInterrupt(this->pinB), staticInterruptHandler, CHANGE);
    attachInterrupt(digitalPinToInterrupt(this->pinButton), staticInterruptHandler, CHANGE);
    this->isStarted = true;

}

void AdvancedRotaryEncoder::stop() {
    isStarted = false;
    detachInterrupt(digitalPinToInterrupt(this->pinA));
    detachInterrupt(digitalPinToInterrupt(this->pinB));
    detachInterrupt(digitalPinToInterrupt(this->pinButton));
}

double_t AdvancedRotaryEncoder::getAngle() {
    return this->angle;
}

void AdvancedRotaryEncoder::setAngle(double_t newAngle) {
    this->angle = newAngle;
}

void AdvancedRotaryEncoder::setStepsPerRotation(int16_t numSteps) {
    this->stepsPerRotation = numSteps;
}

void AdvancedRotaryEncoder::setListener(callback_function callbackFunction) {
    this->listener = callbackFunction;
}

void AdvancedRotaryEncoder::removeListener() {
    this->listener = nullptr;
}


void AdvancedRotaryEncoder::update() {
    noInterrupts();

    digitalWrite(LED_BUILTIN, LOW);
    delay(10);
    digitalWrite(LED_BUILTIN, HIGH);

    EncoderEvent event = {None, millis(), micros()};

    bool isButtonPressedNow = !digitalRead(this->pinButton);
    // Button processing takes precedence
    if (isButtonPressedNow && !this->isButtonPressed) {
        isButtonPressed = isButtonPressedNow;
        event.eventType = ButtonDown;
    } else if (!isButtonPressedNow && this->isButtonPressed) {
        event.eventType = ButtonUp;
    } else { // Now since button hasn't changed check for rotation
        this->currentAB = digitalRead(this->pinA) << 1;
        this->currentAB |= digitalRead(this->pinB);

        switch ((previousAB | currentAB)) {
            //CW
            case 0b0001:
            case 0b1110:
                this->position++;
                this->angle += 360.0 / this->stepsPerRotation;
                event.eventType = MoveCW;
                break;
                //CCW
            case 0b0100:
            case 0b1011:
                this->position--;
                this->angle -= 360.0 / this->stepsPerRotation;
                event.eventType = MoveCCW;
                break;
        }
        this->previousAB = this->currentAB << 2;
    }

    this->listener(event);
    interrupts();
}

int16_t AdvancedRotaryEncoder::getPosition() {
    return position;
}

void AdvancedRotaryEncoder::setPosition(int16_t position) {
    this->position = position;
}
