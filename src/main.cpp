#include <Arduino.h>
#include "JeeUI2.h" // Подключаем JeeUI2 фреймворк к проекту
jeeui2 jee; // Создаем объект класса для работы с JeeUI2 фреймворком

#define RELAY 12 // D6

#include "interface.h"  // в этот файл вынесена работа с параметрами и с веб интерфейсом
#include "ds18b20.h"    // в этом файле работа с датчиком ds18b20
#include "relay.h"      // в этом файле работа с реле
#include "timing.h"
#include "led7seg.h"
#include "mqtt_code.h"

//void sendData();

void setup() {
	Serial.begin(115200);
	// Initialize LCD
	lc.shutdown(0, false);
	// Set the brightness to a low value
	lc.setIntensity(0, 0.7);
	// and clear the display
	lc.clearDisplay(0); 
  configModeCallback();
	jee.mqtt("mqtt-server.com", 1883, "user", "password", mqttCallback, onConnect,  true); // суперфункция, обеспечит всю работу с mqtt, последний аргумент - разрешение удаленного управления
	jee.udp(); // Ответ на UDP запрс. port 4243
	jee.led(16, false); // назначаем пин на светодиод, который нам будет говорит о состоянии устройства. (быстро мигает - пытается подключиться к точке доступа, просто горит (или не горит) - подключен к точке доступа, мигает нормально - запущена своя точка доступа)
	ShowConnect();
	jee.ap(20000); // если в течении 20 секунд не удастся подключиться к Точке доступа - запускаем свою (параметр "wifi" сменится с AP на STA)
	parameters(); // создаем параметры (смотрите файл interface.h)
	jee.ui(interface); // обратный вызов - интерфейс (смотрите файл interface.h)
	jee.update(update); // обратный вызов - вызывается при введении данных в веб интерфейс, нужна для сравнения значений пременных с параметрами  (смотрите файл interface.h)
	jee.begin(true); // Инициализируем JeeUI2 фреймворк. Параметр bool определяет, показывать ли логи работы JeeUI2 (дебаг)
	update(); // 'этой функцией получаем значения параметров в переменные  (смотрите файл interface.h)
	if (wifiFirstConnected) {
		Serial.print(F("\nАдрес управления: "));
		Serial.println(strIPaddr);
		IP_Show();
	}
}

void loop() {
	jee.handle(); // цикл, необходимый фреймворку
	ds_handle(ds_int); // цикл замера температуры ds18b20
	if (time_setup) {
		update_handle(NTP_req*60000);
	} else {
		update_handle(5000);
	}
	date_handle(YearTime * 1000);
	if (!YearShow) time_handle();
	relay();
}
