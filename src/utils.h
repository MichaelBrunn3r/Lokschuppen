#pragma once

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
