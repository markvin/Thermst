// this example is public domain. enjoy!
// https://learn.adafruit.com/thermocouple/

#include <LiquidCrystal.h>
#include <arduino-timer.h>
#include <max6675.h>

#include "defines.h"

using namespace thermst;

enum Mode { AUTO, MANUAL };

uint16_t current_temperature;
uint16_t target_temperature;
uint16_t current_power_percents{0};
bool heater_on{false};
Mode mode{AUTO};

namespace display {
LiquidCrystal lcd(pin::LCD_RS, pin::LCD_EN, pin::LCD_DB4, pin::LCD_DB5,
                  pin::LCD_DB6, pin::LCD_DB7);

void setup() {
  lcd.begin(LCD_COLS, LCD_ROWS);
  pinMode(pin::LCD_LED, OUTPUT);
  digitalWrite(pin::LCD_LED, HIGH);
}

void update() {
  char line_buffer[LCD_COLS];
  lcd.setCursor(0, 0);
  lcd.print("t["
            "\x99"
            "C]: ");

  if (mode == AUTO)
    sprintf(line_buffer, "%3.d %1s %3.d", target_temperature,
            heater_on ? "\x13" : "", current_temperature);
  else
    sprintf(line_buffer, "%3.d      ", current_temperature);

  lcd.setCursor(7, 0);
  lcd.print(line_buffer);

  sprintf(line_buffer, "P[ %%]: %3.d%s", current_power_percents,
          mode == AUTO ? "  AUTO" : "      ");
  lcd.setCursor(0, 1);
  lcd.print(line_buffer);
}
} // namespace display

MAX6675 thermocouple(pin::TC_SCK, pin::TC_CS, pin::TC_SO);

struct ToggleButton {
  bool update() {
    int current_state = digitalRead(pin::MODE_BUTTON);
    if (last_state == HIGH and current_state == LOW) {
      on = !on;
    }
    last_state = current_state;
    return on;
  }

private:
  int last_state;
  bool on;

} mode_button;

void updateEntities() {
  current_temperature = thermocouple.readCelsius();

  const auto temperature_range_width = MAX_TEMPERATURE - MIN_TEMPERATURE;
  target_temperature =
      MIN_TEMPERATURE + float(analogRead(pin::THERM_PRESET_POTENTIOMETER)) /
                            MAX_ADC_VALUE * temperature_range_width;

  current_power_percents =
      float(analogRead(pin::POWER_PERCENTS_POTENTIOMETER)) / MAX_ADC_VALUE *
      100;

  mode = mode_button.update() ? AUTO : MANUAL;

  if (mode == AUTO) {
    if (heater_on and
        current_temperature > (target_temperature + TEMPERATURE_HYSTERESIS))
      heater_on = false;
    else if (not heater_on and current_temperature < (target_temperature -
                                                      TEMPERATURE_HYSTERESIS))
      heater_on = true;
  } else
    heater_on = true;
}

void applyRegulation() {
  digitalWrite(pin::RELAY, not heater_on);
  digitalWrite(LED_BUILTIN, heater_on);
  analogWrite(pin::POWER_CTRL,
              MAX_PWM_VALUE * current_power_percents * heater_on / 100.f);
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(pin::RELAY, OUTPUT);
  pinMode(pin::POWER_CTRL, OUTPUT);
  pinMode(pin::MODE_BUTTON, INPUT_PULLUP);

  display::setup();
}

void loop() {
  updateEntities();
  applyRegulation();
  display::update();
}
