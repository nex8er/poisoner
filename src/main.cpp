#include <Arduino.h>

#include <Wire.h> 
#include <LCD.h>
#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "GyverEncoder.h"

#include "config.h"
#include "utils.h"
#include "system.h"
#include "menu.h"

LiquidCrystal_I2C lcd(LCD_ADDR, LCD_EN, LCD_RW, LCD_RS, LCD_D4, LCD_D5, LCD_D6, LCD_D7, LCD_BL, POSITIVE);
OneWire oneWire(OW_PIN);
DallasTemperature ds(&oneWire);
Encoder enc(ENC_CLK, ENC_DT, ENC_SW);

System selfSystem;
Menu menu(&selfSystem, &lcd);


void setup() {
	Serial.begin(9600);

	// установка пинов
	pinMode(PUMP_PIN, OUTPUT);
	pinMode(BUZZER_PIN, OUTPUT);
	pinMode(HEATER_PIN, OUTPUT);
	pinMode(LLS_PIN, INPUT);

	AIR_OFF;
	HEATER_OFF;
	
	// инициализация дисплея
  lcd.begin(16,2);
	lcd.home();
  lcd.print("Initialization");

	// выбор типа энкодера
	enc.setType(TYPE1);
	// передаем объекту системы ссылку на датчик
	selfSystem.tempSensorInit(&ds);
	// загрузка параметров из EEPROM
	selfSystem.settingsLoad();

	// если все хорошо, подаем звуковой сигнал
	tone(BUZZER_PIN, 700, 200);
	
	// инициализация дисплея
	menu.init();
	menu.showMain();

}

void loop() {

	selfSystem.run();
	menu.run();
	enc.tick();
	
	// обработка энкодера
	if (enc.isRight()) {
		menu.turn(RIGHT);
	}
	if (enc.isLeft()) {
		menu.turn(LEFT);
	}
	if (enc.isClick()) {
		menu.click(true);

	}
	if (enc.isHolded()) {
		menu.hold();
	}
	if (enc.isDouble()) {
		int16_t defaultTimer = 1200;
		selfSystem.countdownSet(defaultTimer);
	}

}