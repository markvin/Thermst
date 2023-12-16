#pragma once

#include <stdint.h>

namespace thermst {
namespace pin {
inline constexpr uint8_t LCD_RS = 6;
inline constexpr uint8_t LCD_EN = 7;
inline constexpr uint8_t LCD_DB4 = 8;
inline constexpr uint8_t LCD_DB5 = 10;
inline constexpr uint8_t LCD_DB6 = 11;
inline constexpr uint8_t LCD_DB7 = 12;
inline constexpr uint8_t LCD_LED = 5;

inline constexpr uint8_t RELAY = 0;

inline constexpr uint8_t TC_SCK = 2;
inline constexpr uint8_t TC_CS = 3;
inline constexpr uint8_t TC_SO = 4;

inline constexpr uint8_t THERM_PRESET_POTENTIOMETER = A0;
inline constexpr uint8_t POWER_PERCENTS_POTENTIOMETER = A1;

inline constexpr uint8_t POWER_CTRL = 9;

inline constexpr uint8_t MODE_BUTTON = A5;
} // namespace pin

inline constexpr uint8_t LCD_ROWS = 2;
inline constexpr uint8_t LCD_COLS = 16;

inline constexpr int MIN_TEMPERATURE = 150; // in CELSIUS
inline constexpr int MAX_TEMPERATURE = 450;
inline constexpr int MAX_ADC_VALUE = 1023;
inline constexpr int MAX_PWM_VALUE = 255;
inline constexpr int TEMPERATURE_HYSTERESIS = 2;

} // namespace thermst