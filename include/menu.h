#pragma once
#include <Arduino.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>
#include "system.h"
#include "utils.h"


// выводит отформатированное значение таймера
String countdownAsStr(const int16_t&);
// выводит отформатированное значение мощности компрессора
String airToStr(const int8_t&);

enum Selector : uint8_t {  // селектор главного меню. в нем может быть выбран только один из трех пунктов
	TEMP = 0,
	TIMER = 1,
	AIR = 2
};

class ItemMenu {		// любой элемент меню
public:
	ItemMenu();
	ItemMenu(uint8_t, uint8_t, uint8_t, uint8_t, String);
	uint8_t id;						// идентификатор 
	uint8_t size;					// сколько ячеек занимает
	uint8_t locateX;			// позиция по X
	uint8_t locateY;			// позиция по Y
	String label;					// отображаемый текст
	boolean visible;			// видимость
	boolean selected;			// флаг выбран

};

class Menu {
public:

	Menu(System*, LiquidCrystal_I2C*);

	//инициализация
	void init(void);
	// основной исполнительный метод
	void run(void);
	// обрисовывает главное меню
	void showMain(void);
	
	// отработка поворота энкодера
	void turn(bool);
	// отработка клика энкодера
	void click(bool);

private:
	
	// указатель на класс управления системой
	System* system;
	// указатель на дисплей
	LiquidCrystal_I2C* lcd;


	// таймеры для задержки 
	uint64_t loopTime = 0;
	uint64_t currentTime = 0;
	boolean clkX2 = false;

	// флаг, указывающий на то, что на экране есть мигающие элементы, и их надо отработать
	boolean blinkFlag = false;
	// флаг, указывающий на то, что на экране есть выделенный элемент
	boolean highlightFlag = false;

	// структура главного меню, состоящая из списка элементов, созданных в createMainMenu()
	ItemMenu itemList[10];
	// ???
	uint8_t selectedItem = 0;
	// указатель выбранного параметра меню для изменения 
	Selector selected = Selector::TEMP;
	// указатель уровня меню
	uint8_t selectLevel = 0;
	// таймер для выхода из настроек при бездействии
	uint8_t selectTimeout = 0;

	// для временного хранения изменямого значения 
	int16_t changebleValue = RESET;

	// обработчик мигания элемента меню
	void blink(bool);
	// обработчик выделения одного элемента меню
	void highlight(bool);

	// обработчик выбора параметра для изменения
	void selectItem(bool);
	// обработчик изменения выбранного параметра
	void changeItem(bool);
	// обработчик сохранения изменений
	void confirm(void);

	// обработка изменения температуры
	void changeTemp(bool);
	// обработка установки таймера
	void changeTimer(bool);
	// обработка изменения мощности комерессора
	void changeAir(bool);
	// изменение элементов меню, для визуализации изменения параметров
	void displayChange(void);

	// обновление элементов меню из СИСТЕМЫ
	void refreshMenuData(void);
	// создание списка элементов главного меню
	void createMainMenu(void);
	// вывод на экран элемента меню
	void printItem(const ItemMenu&);
	
	// проверка на спецсимволы, и вывод их на экран
	boolean checkSpecSym(const String&);

};


