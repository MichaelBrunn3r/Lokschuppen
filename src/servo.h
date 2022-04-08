#include "utils.h"
#include <Arduino.h>

typedef uint16_t PWMTicks;

struct ServoSpecs {
    // PWM frequency in Hz
    Herz frequency;
    // Time it takes the servo to turn 1˚
    Miliseconds msPerDegree;
};

ServoSpecs MG_996R_SPECS = {50, 170 / 60.0};
ServoSpecs SG_90_SPECS = {50, 100 / 60.0};

class Servo {
  public:
    Range<PWMTicks> tickRange;
    Angle offset;
    Range<Angle> angleRange;

    Servo(PWMTicks minTicks, PWMTicks maxTicks, Angle offset, Angle maxAngle) {
        this->tickRange = {minTicks, maxTicks};
        this->angleRange = {-maxAngle / 2, maxAngle / 2};
        this->offset = offset;
    }

    PWMTicks angleToTicks(Angle angle) {
        return map(angle + this->offset, this->angleRange.min, this->angleRange.max,
                   this->tickRange.min, this->tickRange.max);
    }
};
