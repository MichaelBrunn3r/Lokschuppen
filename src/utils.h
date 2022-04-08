#pragma once

typedef float Miliseconds;
typedef float Angle;
typedef float Herz;

template <typename T> struct Range {
    union {
        T min;
        T lower;
    };
    union {
        T max;
        T upper;
    };
};
