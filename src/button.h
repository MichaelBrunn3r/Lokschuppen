#include <Arduino.h>
#include <FunctionalInterrupt.h>

class InterruptButton {

  public:
    InterruptButton(uint8_t pin, uint8_t mode = INPUT_PULLDOWN, uint8_t defaultState = LOW) {
        this->_pin = pin;
        this->_mode = mode;
        this->_defaultState = defaultState;
    }
    ~InterruptButton() { detach(); }

    void attach() {
        pinMode(this->_pin, this->_mode);
        int interruptMode = this->_defaultState == LOW ? RISING : FALLING;
        attachInterrupt(this->_pin, std::bind(&InterruptButton::handleStateChanged, this),
                        interruptMode);
    }

    void detach() { detachInterrupt(this->_pin); }

    bool wasPressed() {
        bool tmp = this->_wasPressed;
        this->_wasPressed = false;
        return tmp;
    }

  private:
    uint8_t _pin;
    volatile uint8_t _mode;
    volatile uint8_t _defaultState;
    volatile bool _wasPressed = false;
    volatile unsigned long _lastPressMillis = 0;

    void IRAM_ATTR handleStateChanged() {
        unsigned long millisSinceLastPress = millis() - this->_lastPressMillis;

        if (millisSinceLastPress >= 100) {
            this->_wasPressed = true;
            this->_lastPressMillis = millis();
        }
    }
};
