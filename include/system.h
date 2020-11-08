#pragma once
#include <Arduino.h>
#include <EEPROM.h>
#include <DallasTemperature.h>
#include "config.h"

enum ErrorList {
	OK,
	SENSOR_ERROR,
	OUT_OF_RANGE
};

class System {
public:
	// основной исполнительный метод
	void run();

	// инициализация и первичная проверка датчика температуры
	void tempSensorInit(DallasTemperature*);
	// возвращает текущую температуру
	int8_t tempGet(void);
	// возвразает установление значение требуемой температуры
	int8_t tempNeededGet(void) const;
	// устанавлевает требуемую температуру
	void tempNeededSet(int8_t);
	// устанавливает таймер обратного отчета на Х секунд 
	void countdownSet(int16_t&);
	// возвращает текущее значение таймера обратного отчета в секундах
	int16_t countdownGet(void) const;
	// возвращает текущее сост. датчика уровня жидкости
	bool liquidLevelCheck(void) const;
	// устанавливает уровень мощности помпы подачи воздуха
	void airSet(int8_t);
	// возвращает текущий уровень мощности помпы подачи воздуха
	uint8_t airGet(void) const;
	// сохранение текущих параметров в eeprom
	bool settingsSave(void);
	// загрузка параметров из eeprom
	bool settingsLoad(void);
	// флаг ошибки
	ErrorList ERR = OK;

private:
	// обработка состояния нагревателя
	void heater(void);
	// получение температуры в свойство класса
	void tempGetToProp(void);
	// обработка состояния таймера обратного отчета
	void countdownCheck(void);
	// обработка состояния воздушной помпы
	void air(void);


	// таймеры для задержки 
	uint64_t loopTime = 0;
	uint64_t currentTime = 0;
	uint8_t tempProbeDelay = 0;
	// таймер обратного отчета
	int16_t countdownTimer = RESET;
	int8_t countdownAlarmContinue = 0;
	// сост. нагревателя
	boolean heaterState = false;
	// требуемая температура
	uint8_t tempNeed = 50;
	// уровень мощности помпы подачи воздуха
	uint8_t airPower = 0;
	// флаг изменения параметров
	boolean valueChanged = true;
	// текущая температуры жидкости	
	int8_t temperature = RESET;
	// ссылка на объект датчика температуры
	DallasTemperature* sensor;

};