#include "utils.h"
#include <Arduino.h>

typedef float Angle;
typedef uint16_t PWMTicks;

struct ServoSpecs {
    // PWM frequency in Hz
    float frequency;
};

ServoSpecs MG_996R_SPECS = {50};
ServoSpecs SG_90_SPECS = {50};

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
