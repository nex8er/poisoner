#pragma once

//настройки
#define clk 1000
#define DELAY 1000
#define ALARM_CONTINUE 59
#define RESET -1

//display pins on PCF
#define LCD_ADDR 0x38
#define LCD_EN 2
#define LCD_RW 1
#define LCD_RS 0
#define LCD_D4 4
#define LCD_D5 5
#define LCD_D6 6
#define LCD_D7 7
#define LCD_BL 3

//encoder pins
#define ENC_CLK 3
#define ENC_DT 4
#define ENC_SW 5

//outfit
#define LLS_PIN 6
#define PUMP_PIN 9
#define OW_PIN 12
#define BUZZER_PIN 10
#define HEATER_PIN 11

//сокращения
#define HEATER_OFF digitalWrite(HEATER_PIN, LOW)
#define HEATER_ON digitalWrite(HEATER_PIN, HIGH)
#define AIR_OFF digitalWrite(PUMP_PIN, HIGH);

#define RIGHT true
#define LEFT false