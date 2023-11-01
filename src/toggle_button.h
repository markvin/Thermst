#pragma once

#include <Arduino.h>

template <uint8_t Pin> struct ToggleButton {
  ToggleButton() { pinMode(Pin, INPUT_PULLUP); }
  int update(int &val, int max_val) {
    int current_state = digitalRead(Pin);
    if (last_state == HIGH and current_state == LOW) {
      val = (val + 1) % max_val;
    }
    last_state = current_state;
    return val;
  }

private:
  int last_state;
};
