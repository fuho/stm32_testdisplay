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

#ifndef AdvancedRotaryEncoder_h
#define AdvancedRotaryEncoder_h

#include <Arduino.h>
#include <vector>

enum EventType {
    MoveCW, MoveCCW, ButtonDown, ButtonUp, None
};

struct EncoderEvent {
    EventType eventType;
    unsigned long millis;
    unsigned long micros;
};

typedef void (*callback_function)(EncoderEvent e);

class AdvancedRotaryEncoder {
public:
    AdvancedRotaryEncoder(int pinA, int pinB, int pinButton);

    void start();

    void stop();

    void setListener(callback_function callbackFunction);

    void removeListener();

    int16_t getPosition();

    double_t getAngle();

    void setStepsPerRotation(int16_t numSteps);

    void update();

    void setPosition(int16_t position);

    void setAngle(double_t angle);


private:
    uint8_t pinA;
    uint8_t pinB;
    uint8_t pinButton;
    uint16_t stepsPerRotation;
    static std::vector<AdvancedRotaryEncoder*> instances;


    volatile bool isStarted = false;
    volatile int16_t position = 0;
    volatile double_t angle = 0;
    volatile int16_t maxPosition = 0x7fff;  // +32767
    volatile int16_t minPosition = 0xffff;  // -32768
    volatile callback_function listener;

    volatile uint8_t previousAB = 0;    //previous state of "A"+"B"
    volatile uint8_t currentAB = 0;    //current   state of "A"+"B"
    volatile bool isButtonPressed = false; //encoder button status, idle value is "true" because internal pull-up resistor is enabled

    static void staticInterruptHandler();
};

#endif
