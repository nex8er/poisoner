#include "menu.h"

//символы для дисплея
byte lcdchars[][8] = { 
	{ B00111, B00101,	B00111,	B00000,	B00000, B00000, B00000 }, // кодируем символ градуса
	{ B00100,	B00100,	B00100,	B00000,	B00100,	B00000,	B10101 }	// кодируем символ низкого уровня жидкости
};
#define DEG byte(0)
#define LLL byte(1)

#define SELECT_TIMEOUT 15  // таймаут выхода из настроек при бездействии
	

// идентификаторы для элементов меню
#define TempCurrent 0
#define TempSymbol1 1
#define TempSymbolArr 2
#define TempNeeded 3
#define TempSymbol2 4
#define LiquidLevel 5
#define TimerSymbol 6
#define TimerValue 7
#define AirSymbol 8
#define AirValue 9

// уровни меню
#define NOT_SEL 0
#define ITEM_SELECT 1
#define ITEM_CHANGE 2

#define RIGHT true
#define LEFT false


Menu::Menu(System* _s, LiquidCrystal_I2C* _lcd) {
	system = _s;
	lcd = _lcd;
}

void Menu::init() {

	// загрузка спецсимволов
	for(uint8_t i = 0; i < sizeof(lcdchars)/sizeof(lcdchars[0]); i++) {
		lcd->createChar(i, lcdchars[i]);
	}

	lcd->clear();
	// создаем список элементов главного меню
	createMainMenu();
}

void Menu::run() {
	currentTime = millis(); 
	if (currentTime <= (loopTime + (clk>>1))) {
		return;
	}
	loopTime = currentTime;
	clkX2 = !clkX2;
	blinkFlag = !blinkFlag;
	
	// выход из настроек по таймауту
	if (selectTimeout > 0) {
		selectTimeout--;
	} else {
		selectLevel = NOT_SEL;
		// делаем все элементы видимыми
		blink(false);
	}
	
	// в режиме выбора параметра - мигаем
	if (selectLevel == ITEM_SELECT) {
		blink(blinkFlag);
	}

	// обновляем значения пунктов, если не изменяем знач. параметров, и с частотой clk*2
	if (clkX2 && selectLevel != ITEM_CHANGE) {
		refreshMenuData();
		showMain();
	}
}

void Menu::turn(bool dir) {
	delay(70);
	// сбрасываем таймер бездействия
	selectTimeout = SELECT_TIMEOUT;

	// передаем действие след. обработчику в зависимости от текущего уровня
	if (selectLevel == NOT_SEL) {
		selectLevel++;
		return;
	}
	if (selectLevel == ITEM_SELECT) {
		selectItem(dir);
		return;
	}
	if (selectLevel == ITEM_CHANGE) {
		changeItem(dir);
	}
}

void Menu::click(bool param) {
	selectTimeout = SELECT_TIMEOUT;
	// передаем действие след. обработчику в зависимости от текущего уровня
	if (selectLevel == NOT_SEL) {
		return;
	}
	if (selectLevel == ITEM_SELECT) {
		selectLevel++;
		
		// переносим текущее знач. параметра во временную переменную, с учетом положения селектора
		switch (selected) {
			case TEMP:
				changebleValue = system->tempNeededGet();	
				break;
			case TIMER:
				changebleValue = system->countdownGet();
				break;
			case AIR:
				changebleValue = system->airGet();
				break;
		}
		// выделяем элементы меню выбранного параметра
		highlight(true);
		return;
	}
	if (selectLevel == ITEM_CHANGE) {
		confirm();
	}
}

void Menu::changeItem(bool dir) {
	switch (selected) {
		case TEMP:
			changeTemp(dir);
			break;
		case TIMER:
			changeTimer(dir);
			break;
		case AIR:
			changeAir(dir);
			break;
	}
	// отобразить сделанные изменения
	displayChange();
}

void Menu::changeTemp(bool dir) {
	// сценарий изменения температуры
	if (dir == RIGHT) {
		changebleValue++;
		if (changebleValue > 70) {
			changebleValue = 70;
		}
	}
	if (dir == LEFT) {
		changebleValue--;
		if (changebleValue < 20) {
			changebleValue = 20;
		}
	}
}
void Menu::changeTimer(bool dir) {
	// если таймер сброшен, берем значение за 0
	if (changebleValue == RESET) {
		changebleValue = 0;
	}
	// сценарий изменения таймера обратного отчета
	if (dir == RIGHT) {
		changebleValue += 60;
		if (changebleValue > 5999) {
			changebleValue = 5999;
		}
	}
	if (dir == LEFT) {
		changebleValue -= 60;
		if (changebleValue < 0) {
			changebleValue = 0;
		}
	}
}

void Menu::changeAir(bool dir) {
	// сценарий изменения мощности компрессора
	if (dir == RIGHT) {
		changebleValue += 10;
		if (changebleValue > 100) {
			changebleValue = 100;
		}
	}
	if (dir == LEFT) {
		changebleValue -= 10;
		if (changebleValue < 0) {
			changebleValue = 0;
		}
	}
}

void Menu::displayChange() {
		switch (selected) {
		case TEMP:
			itemList[TempNeeded].label = changebleValue;
			break;
		case TIMER:
			itemList[TimerValue].label = timerToStr(changebleValue);
			break;
		case AIR:
			itemList[AirValue].label = airToStr(changebleValue);
			break;
	}
	// перерисовать меню
	showMain();
}

void Menu::confirm() {
	// сохранение новых значений в СИСТЕМУ
	switch (selected) {
		case TEMP:
			system->tempNeededSet(changebleValue);
			break;
		case TIMER:
			// если таймер установлен на 0, то сразу его сбрасываем
			if (changebleValue == 0) {
				changebleValue = RESET;
			}
			system->countdownSet(changebleValue);
			break;
		case AIR:
			system->airSet(changebleValue);
			break;
	}
	// сбрасываем временное значение и уровень меню
	selectLevel = NOT_SEL;
	changebleValue = RESET;

	// делаем все видимым и перерисовываем меню с новыми параметрами
	blink(false);
	refreshMenuData();
	showMain();
}

void Menu::blink(bool state) {
	
	if (!state) {
		// устанавливаем видимость для всех элементов из списка
		for (ItemMenu &item : itemList) {
			item.visible = true;
		}
	}	else {
		// или невидимыми для конкретных элементов меню
		if (selectLevel == ITEM_SELECT) {
			switch (selected) {
				case TEMP:
					itemList[TempNeeded].visible = false;
					itemList[TempSymbol2].visible = false;
					break;
				case TIMER:
					itemList[TimerSymbol].visible = false;
					itemList[TimerValue].visible = false;
					break;
				case AIR:
					itemList[AirSymbol].visible = false;
					itemList[AirValue].visible = false;
					break;
			}
		}
	}
	showMain();
}
void Menu::highlight(bool state) {
	// делаем все элементы меню невидимыми
	for (ItemMenu &item : itemList) {
		item.visible = false;
	}
	// и включаем только для выбранных, где меняется параметр
	if (selectLevel == ITEM_CHANGE) {
		switch (selected) {
			case TEMP:
				itemList[TempNeeded].visible = true;
				itemList[TempSymbol2].visible = true;
				break;
			case TIMER:
				itemList[TimerSymbol].visible = true;
				itemList[TimerValue].visible = true;
				break;
			case AIR:
				itemList[AirSymbol].visible = true;
				itemList[AirValue].visible = true;
				break;
		}
	}
	showMain();
}
void Menu::selectItem(bool dir) {
	int8_t position;
	// изменяем положение селектора, бесконечно, каруселью
	if (dir == RIGHT) {
		// получаем Int из типа Selector
		position = ((int8_t)selected+1);
		// если число больше чем есть в селекторе
		if (position > 2) {
			// то возвращаем его в начало
			selected = Selector(0);
		} else {
			// получаем тип Selector из Int
			selected = (Selector)(position);
		}
	}
	if (dir == LEFT) {
		position = ((int8_t)selected-1);
		if (position < 0) {
			// и наоборот
			selected = Selector(2);
		} else {
			selected = (Selector)(position);
		}
	}
	
	// делаем все видимым, тк выбранный пункт меняется
	blinkFlag = false;
	blink(blinkFlag);
	// и сбрасываем отчет таймера, для ровности мигания
	loopTime = 0;
}

void Menu::refreshMenuData() {
	itemList[TempCurrent].label = String(system->tempGet());
	itemList[TempNeeded].label = String(system->tempNeededGet());
	itemList[LiquidLevel].label = system->liquidLevelCheck() ? " " : "lll";
	itemList[TimerValue].label = countdownAsStr(system->countdownGet());
	itemList[AirValue].label = airToStr(system->airGet());
}

void Menu::showMain() {
	for (const ItemMenu &item : itemList) {
		printItem(item);
	}
}

void Menu::createMainMenu() {
	// хардкодный список элементов главного меню, и описанием их свойств
	itemList[TempCurrent] = ItemMenu(TempCurrent, 2, 3, 0, String(system->tempGet()));
	itemList[TempSymbol1] = ItemMenu(TempSymbol1, 2, 5, 0, "degC");
	itemList[TempSymbolArr] = ItemMenu(TempSymbolArr, 2, 7, 0, "->");
	itemList[TempNeeded] = ItemMenu(TempNeeded, 2, 9, 0, String(system->tempNeededGet()));
	itemList[TempSymbol2] = ItemMenu(TempSymbol2, 2, 11, 0, "degC");
	itemList[LiquidLevel] = ItemMenu(LiquidLevel, 1, 15, 0, system->liquidLevelCheck() ? " " : "lll");
	itemList[TimerSymbol] = ItemMenu(TimerSymbol, 2, 0, 2, "T:");
	itemList[TimerValue] = ItemMenu(TimerValue, 5, 3, 1, countdownAsStr(system->countdownGet()));
	itemList[AirSymbol] = ItemMenu(AirSymbol, 3, 10, 1, "Air");
	itemList[AirValue] = ItemMenu(AirValue, 3, 13, 1, airToStr(system->airGet()));

}
void Menu::printItem(const ItemMenu& item) {
	// устанавливаем положение курсора
	lcd->setCursor(item.locateX, item.locateY);
	// выводим, если элемент видимый
	if (item.visible) {
		// с проверкой на спейсимволы
		if (!checkSpecSym(item.label)) {
			lcd->print(item.label);
		}
	} else {
		// если не видимый, то заполняем его место пробелами
		for (uint8_t i = item.size; i > 0; i--) {
			lcd->print(" ");
		}
	}
}
boolean Menu::checkSpecSym(const String& str) {
	// известные спецсимволы
	if (str == "degC") {
		lcd->write(DEG);
		lcd->print("C");
		return true;
	}
	if (str == "lll") {
		lcd->write(LLL);
		return true;
	}
	return false;
}


ItemMenu::ItemMenu(uint8_t _id, uint8_t _size, uint8_t _locateX, uint8_t _locateY, String _label) {
	 // конструктор элемента меню
	 id = _id;
	 size = _size;
	 locateX = _locateX;
	 locateY = _locateY;
	 label = _label;
	 visible = true;
	 selected = false;
}
ItemMenu::ItemMenu() {
	// дефолтный конструктор
	id = 0;
	 size = 0;
	 locateX = 0;
	 locateY = 0;
	 label = "";
	 visible = true;
	 selected = false;
}



String countdownAsStr(const int16_t& timeValue) {
	// значения за пределами
	if (timeValue < 0)
		return "--:--";
	if (timeValue == 0)
		return "00:00";
	if (timeValue > 5999)
		return "XX:XX";

	uint8_t min = 0, sec = 0;
	
	if (timeValue > 59) {
		min = timeValue / 60;
	}
	sec = timeValue % 60;

	char buff[6];
	// форматируем строку
	sprintf(buff, "%02u:%02u", min, sec);
		return String(buff);
}

String airToStr(const uint8_t& air) {
	if (air <= 0) {
		return String("OFF");
	} else if (air == 100) {
		return String("MAX");
	} else {
		char buff[4];
		sprintf(buff, "%u%%", air);
		return String(buff);
	}
}