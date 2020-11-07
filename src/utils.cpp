#include "utils.h"

void log(String msg){
	Serial.println(msg);
}

String timerToStr(const int16_t timer) {
	if (timer < 0)
		return "--:--";
	if (timer == 0)
		return "00:00";
	if (timer > 5999)
		return "XX:XX";

	uint8_t min = 0, sec = 0;
	
	if (timer > 59) {
		min = timer / 60;
	}
	sec = timer % 60;

	char buff[6];
	sprintf(buff, "%02u:%02u", min, sec);
		return String(buff);

}

String airToStr(const int16_t air) {
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