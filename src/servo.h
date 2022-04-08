#include "utils.h"
#include <Arduino.h>

typedef float Angle;
typedef uint16_t PWMTicks;

struct ServoSpecs {
    // PWM frequency in Hz
    float frequency;
    Range<Angle> angleRange;
};

ServoSpecs MG_996R_SPECS = {50, {-100, 100}};

PWMTicks angleToTicks(unsigned int angle, Range<Angle> angleRange, Range<PWMTicks> tickRange) {
    return map(angle, angleRange.min, angleRange.max, tickRange.min, tickRange.max);
}
