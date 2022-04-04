#if DEBUG
    #define log(x) Serial.print(x)
    #define logln(x) Serial.println(x)
    #define logf(x, ...) Serial.printf(x, __VA_ARGS__)
#else
    #define log(x)
    #define logln(x)
    #define logf(x)
#endif
