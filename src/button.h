#include <Arduino.h>
#include <FunctionalInterrupt.h>

class InterruptButton {

  public:
    InterruptButton(uint8_t pin, uint8_t mode = INPUT, bool highIsPressed = true) {
        this->pin = pin;
        this->mode = mode;
        this->onState = highIsPressed ? HIGH : LOW;
        this->attach();
    }
    ~InterruptButton() { detach(); }

    void attach() {
        pinMode(this->pin, this->mode);
        attachInterrupt(this->pin, std::bind(&InterruptButton::handleStateChanged, this), CHANGE);
    }

    void detach() { detachInterrupt(this->pin); }

    bool isPressed() { return this->pressed; }

  private:
    uint8_t pin;
    volatile uint8_t mode;
    volatile uint8_t onState;
    volatile bool pressed;

    void IRAM_ATTR handleStateChanged() { pressed = digitalRead(pin) == onState; }
};
