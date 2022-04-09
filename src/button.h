#include <Arduino.h>
#include <FunctionalInterrupt.h>

class InterruptButton {

  public:
    InterruptButton(uint8_t pin, uint8_t mode = INPUT, bool highIsPressed = true) {
        this->_pin = pin;
        this->_mode = mode;
        this->_onState = highIsPressed ? HIGH : LOW;
        this->_isPressed = false;
    }
    ~InterruptButton() { detach(); }

    void attach() {
        pinMode(this->_pin, this->_mode);
        attachInterrupt(this->_pin, std::bind(&InterruptButton::handleStateChanged, this), CHANGE);
    }

    void detach() { detachInterrupt(this->_pin); }

    bool isPressed() { return this->_isPressed; }

  private:
    uint8_t _pin;
    volatile uint8_t _mode;
    volatile uint8_t _onState;
    volatile bool _isPressed;

    void IRAM_ATTR handleStateChanged() {
        this->_isPressed = digitalRead(this->_pin) == this->_onState;
    }
};
