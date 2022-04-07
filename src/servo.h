#include "utils.h"
#include <Arduino.h>

typedef int Angle;
typedef uint16_t PWMTick;

struct ServoSpecs {
    // PWM frequency in Hz
    float frequency;
    Range<Angle> angleRange;
};

ServoSpecs MG_996R_SPECS = {50, {-100, 100}};

PWMTick angleToTicks(unsigned int angle, Range<Angle> angleRange, Range<PWMTick> tickRange) {
    return map(angle, angleRange.min, angleRange.max, tickRange.min, tickRange.max);
}
