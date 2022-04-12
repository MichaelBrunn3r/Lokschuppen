#include "utils.h"
#include <Arduino.h>

typedef uint16_t PWMTicks;

struct ServoSpecs {
    // PWM frequency in Hz
    Herz frequency;
    // Time it takes the servo to turn 1˚
    Miliseconds msPerDegree;
};

ServoSpecs SPECS_MG_996R = {50, 170 / 60.0};
ServoSpecs SPECS_SG_90 = {50, 100 / 60.0};

class Servo {
  public:
    Range<PWMTicks> tickRange;
    Angle offset;
    Range<Angle> rotationRange;
    ServoSpecs* specs;

    Servo(PWMTicks minTicks, PWMTicks maxTicks, Angle offset, Angle maxAngle, ServoSpecs* specs) {
        this->tickRange = {minTicks, maxTicks};
        this->rotationRange = {-maxAngle / 2, maxAngle / 2};
        this->offset = offset;
        this->specs = specs;
    }

    PWMTicks angleToTicks(Angle angle) {
        return map(angle + this->offset, this->rotationRange.min, this->rotationRange.max,
                   this->tickRange.min, this->tickRange.max);
    }
};
