
#include <EEPROM.h>
#include <LiquidCrystal.h>
#include <max6675.h>

#include "defines.h"
#include "toggle_button.h"

using namespace thermst;

enum Mode { MODE_AUTO = 0, MODE_MANUAL, all_modes_num };

uint16_t current_temperature;
uint16_t target_temperature;
uint16_t current_power_percents{0};
bool heater_on{false};
int mode_code{MODE_AUTO};

MAX6675 thermocouple(pin::TC_SCK, pin::TC_CS, pin::TC_SO);
LiquidCrystal lcd(pin::LCD_RS, pin::LCD_EN, pin::LCD_DB4, pin::LCD_DB5,
                  pin::LCD_DB6, pin::LCD_DB7);
ToggleButton<pin::MODE_BUTTON> mode_button;

void setupDisplay() {
  lcd.begin(LCD_COLS, LCD_ROWS);
  pinMode(pin::LCD_LED, OUTPUT);
  digitalWrite(pin::LCD_LED, HIGH);
}

void updateDisplay() {
  char line_buffer[LCD_COLS];
  lcd.setCursor(0, 0);
  lcd.print("t["
            "\x99"
            "C]: ");

  if (mode_code == MODE_AUTO)
    sprintf(line_buffer, "%3.d %1s %3.d", target_temperature,
            heater_on ? "\x13" : "", current_temperature);
  else
    sprintf(line_buffer, "%3.d      ", current_temperature);

  lcd.setCursor(7, 0);
  lcd.print(line_buffer);

  sprintf(line_buffer, "P[ %%]: %3.d%s", current_power_percents,
          mode_code == MODE_AUTO ? "  AUTO" : "      ");
  lcd.setCursor(0, 1);
  lcd.print(line_buffer);
}

void store() {
  int idx{0};
  EEPROM.write(idx++, mode_code);
}

void restore() {
  int idx{0};
  mode_code = EEPROM.read(idx++);
}

void updateEntities() {
  current_temperature = thermocouple.readCelsius();

  const auto temperature_range_width = MAX_TEMPERATURE - MIN_TEMPERATURE;
  target_temperature =
      MIN_TEMPERATURE + float(analogRead(pin::THERM_PRESET_POTENTIOMETER)) /
                            MAX_ADC_VALUE * temperature_range_width;

  current_power_percents =
      min(analogRead(pin::POWER_PERCENTS_POTENTIOMETER) * 1.f / MAX_ADC_VALUE,
          1.f) *
      100;

  mode_button.update(mode_code, all_modes_num);

  if (mode_code == MODE_AUTO) {
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

  const int pwm_factor = MAX_PWM_VALUE * (1.f - current_power_percents * heater_on / 100.f);
  analogWrite(pin::POWER_CTRL, pwm_factor);
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(pin::RELAY, OUTPUT);
  pinMode(pin::POWER_CTRL, OUTPUT);

  restore();

  setupDisplay();

  delay(500); // wait for the thermocouple to stabilize
}

void loop() {
  const auto prev_mode_code = mode_code;
  updateEntities();
  if (prev_mode_code != mode_code) {
    store();
  }
  applyRegulation();
  updateDisplay();
  delay(500);
}
