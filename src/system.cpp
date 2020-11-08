#include "system.h"

// метки 
#define STORE_TEMP 0
#define STORE_AIR 1

void System::run(){
	// реализация задержки
	currentTime = millis(); 
	if (currentTime <= (loopTime + DELAY)) {
		return;
	}
	loopTime = currentTime;   

	// задерка обновления температуры
	if (tempProbeDelay <= 0) {
		tempGetToProp();
		tempProbeDelay = 5;
	}
	tempProbeDelay--;
	
	heater();
	countdownCheck();
	air();
}
void System::tempSensorInit(DallasTemperature* _ds) {
	sensor = _ds;
	sensor->begin();
	// установка точности температуры
	sensor->setResolution(9);
	// отключаем выдержку
	sensor->setWaitForConversion(false);

	int8_t temp = tempGet();
	if (temp < 0 ) {
		return;
	}
	// передаем в свойство класса
	temperature = temp;
}
int8_t System::tempGet() {
	// изначальное значение температуры
	int8_t temp = -100;
	// количество попыток чтения датчика
	uint8_t att = 5;

	// читаем пока значения не будут в пределах от -55 до 125, и не рано 85(это ошибка чтения)
	while(temp < -55 || temp > 125 || temp == 85) {
		if (att < 5) {
			delay(200);			// задержка, если это не первая попытка
		}
		sensor->requestTemperaturesByIndex(0);
		temp = sensor->getTempCByIndex(0);
		if (!att) {
			// попытки закончились
			// устанавливаем флаг ошибки
			ERR = SENSOR_ERROR;		
			return(-100);		
		}
		att--;
	}
	// приделы рабочей температуры
	if (temp < 1 || temp > 70) {			
		ERR = OUT_OF_RANGE;
		return -1;
	}
	// если все хорошо
	return temp;
}
void System::tempGetToProp() {
	// просто передаем значение в свойство метода
	temperature = tempGet();
}
void System::countdownCheck() {
	if (countdownTimer > 0) {
		countdownTimer--;
		return;
	}
	// если отчет закончин
	if (countdownTimer == 0) {
		tone(BUZZER_PIN, 440, 400);
		// считаем продолжительность сигнализации
		countdownAlarmContinue++;
		if (countdownAlarmContinue > ALARM_CONTINUE) {
			// после чего сбрасываем таймер
			countdownTimer = RESET;
			countdownAlarmContinue = 0;
			return;
		}
	}
}
void System::countdownSet(int16_t& newValue) {
	countdownTimer = newValue;
}
int16_t System::countdownGet() const {
	return countdownTimer;
}
int8_t System::tempNeededGet() const {
	return tempNeed;
}
void System::tempNeededSet(int8_t newValue) {
	if (newValue > 20 && newValue < 70) {
		tempNeed = newValue;
	}
}
bool System::liquidLevelCheck() const {
	return digitalRead(LLS_PIN);
}
void System::heater() {
	if (!liquidLevelCheck() || temperature < 1) {
		HEATER_OFF;
		return;
	}
	if (temperature < tempNeed + 1) {
		HEATER_ON;
	}
	if (temperature > tempNeed - 1) {
		HEATER_OFF;
	}
}
void System::air() {
	if (airPower == 0) {
		AIR_OFF;
	} else {
		uint8_t pwm = map(airPower, 0, 100, 96, 0);
		analogWrite(PUMP_PIN, pwm);
	}
}
void System::airSet(int8_t newValue) {
	if (newValue >= 0 && newValue <= 100) {
		airPower = newValue;
		valueChanged = true;
	}

}
uint8_t System::airGet() const {
	return airPower;
}
bool System::settingsLoad(void) {
	tempNeed = EEPROM.read(STORE_TEMP);
	airPower = EEPROM.read(STORE_AIR); 
}
bool System::settingsSave(void) {
	EEPROM.write(STORE_TEMP, tempNeed);
	EEPROM.write(STORE_AIR, airPower);
	return true;
}