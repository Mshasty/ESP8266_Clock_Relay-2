// глобальные переменные для работы с ними в программе
unsigned int ds_int; // интервал замера ds18b20 в секундах 
unsigned int mqtt_int; // интервал отправки данных по MQTT в секундах 
float tem; // тут храним температуру
float p_tem; // температура включения реле
float h_tem; // гистерезис
String mode; // режим работы по температуре/ручной ("TEM"/"MAN")
String ntp_srv;
int tz;
boolean ch1_on;
boolean ch2_on;
String tm[2];
uint8_t YearTime; // Период показа даты
uint8_t YearView;  // Время показа даты
unsigned int NTP_req; // Период запроса NTP-сервера
boolean eff_time = true; // показ эффекта перед часами
unsigned int td = 25; // скорость эффекта перед часами
boolean mqtt_enable = true;



void parameters(){
// создаем параметры для нашего проекта
  jee.var(F("wifi"), F("STA")); // режим работы WiFi по умолчанию ("STA" или "AP")  (параметр в энергонезависимой памяти)
  jee.var(F("ssid"), F("Bikar6")); // имя точки доступа к которой подключаемся (параметр в энергонезависимой памяти)
  jee.var(F("pass"), F("htdjkzwbz1917")); // пароль точки доступа к которой подключаемся (параметр в энергонезависимой памяти)

  jee.var(F("p_tem"), F("25"));  // температура включения реле
  jee.var(F("h_tem"), F("2")); // гистерезис
  jee.var(F("ds_int"), F("10")); // это будет интервал замеров DH22 в секундах
  jee.var(F("mode"), F("MAN")); // по заводской настройке работаем вручную ("TEM"/"MAN")

  jee.var(F("ntp_srv"), F("ntp2.belbone.be"));
  jee.var(F("tz"), F("3")); // Тайм-зона
  jee.var(F("ch1_on"), F("true"));
  jee.var(F("ch2_on"), F("true"));
  jee.var(F("tm1"), F("150107202130"));
  jee.var(F("tm2"), F("630216201940"));
  jee.var(F("YearTime"), F("12")); // Период покаха времени, сек
  jee.var(F("YearView"), F("5")); // Период показа данных, сек
  jee.var(F("NTP_req"), F("30")); // Период опроса NTP-сервера, мин.
  jee.var(F("eff_time"), F("true")); 
  jee.var(F("td"), F("25"));
  jee.var(F("mqtt_enable"), F("true"));
}

void relayState(int pin, bool state, String id);

void update(){ // функция выполняется после ввода данных в веб интерфейсе. получение параметров из веб интерфейса в переменные
  // получаем данные в переменную в ОЗУ для дальнейшей работы
  ds_int = jee.param("ds_int").toInt(); // читаем параметр в переменную с переобразованием в нужный тип данных
	p_tem = jee.param("p_tem").toFloat(); // читаем параметр в переменную с переобразованием в нужный тип данных
  h_tem = jee.param("h_tem").toFloat(); // читаем параметр в переменную с переобразованием в нужный тип данных
  mode = jee.param("mode"); // читаем параметр в переменную как String
  ntp_srv = jee.param("ntp_srv");
  tz = jee.param("tz").toInt();
  ch1_on = (jee.param("ch1_on") == "true");
  ch2_on = (jee.param("ch2_on") == "true");
  tm[0] = jee.param("tm1");
  tm[1] = jee.param("tm2");
  YearTime = jee.param("YearTime").toInt();
  YearView = jee.param("YearView").toInt();
  NTP_req = jee.param("NTP_req").toInt();
  eff_time = (jee.param("eff_time") == "true");
  td = jee.param("td").toInt();
  mqtt_enable = (jee.param("mqtt_enable") == "true");

  if (mode == "MAN"){ // обеспечиваем ручную работу реле
    if (jee.param("relay") == "true"){
      relayState(RELAY, true, "relay");
    }
    else { 
      relayState(RELAY, false, "relay");
    }
  }
}

void interface(){ // функция в которой мы формируем веб интерфейс и интерфейс в приложении JeeUI2

  jee.app("AQUATIMER"); // название программы (отображается в веб интерфейсе)

  // создаем меню
  jee.menu("Устройство");
  jee.menu("Таймеры");
  jee.menu("Настройки MQTT");
  jee.menu("Настройки Wi-Fi");
  // создаем контент для каждого пункта меню

  jee.page(); // разделитель между страницами
  // Страница "Датчик"

  // эта суперфункция покажет в веб интерфейсе температуру 
  // и она же обепечит автоматическую отправку по mqtt по 
  // одноименному топику при обновлении параметра (первый аргумент)
  // как только вы запишите в "tem" значение - туту же 
  // отправится сообщение на топик ID/tem с заданным значением вот так: jee.var("tem", "значение")
  jee.pub("tem", "Температура", "°C", "#6060ff", "#f5f5f5"); 

  
  jee.checkbox("relay", "Реле"); // Это переключатель, чтобы щелкать релющкой вручную
	jee.range("p_tem", -10, 36, .5, "Порог температуры"); // ползунок с заданными минимальными и максимальными значениями и шагом
  jee.range("h_tem", 0, 5, .1, "Гистерезис");  // ползунок с заданными минимальными и максимальными значениями и шагом

  jee.option("TEM", "Термостат"); // опция для элемента селекта
  jee.option("MAN", "Ручной"); // опция для элемента селекта
  jee.option("TMR", "Таймер"); // опция для элемента селекта
  jee.option("MQT", "MQTT"); // опция для элемента селекта
  jee.select("mode", "Режим работы устройства"); // Собственно сам элемент селект

  jee.number("ds_int", "Интервал опроса датчика сек.");
  
  jee.checkbox("mqtt_enable", "MQTT");

  jee.page(); // разделитель между страницами
  // Страница "Настройки таймера"
  jee.text("ntp_srv", "NTP server");
  jee.number("tz", "Тайм-зона");
  jee.range("NTP_req", 15, 1440, 15, "Период опроса NTP-сервера, мин.");
  jee.range("YearTime", 6, 54, 3, "Период показа премени");
  jee.range("YearView", 2, 12, 1, "Период показа данных");
  jee.checkbox("eff_time", "Эфффект"); // Включаем эффект перед показом времени
  jee.range("td", 5, 150, 5, "Скорость эффекта перед часами");

  jee.checkbox("ch1_on", "Канал1"); // Это переключатель, чтобы щелкать релющкой вручную
  jee.checkbox("ch2_on", "Канал2"); // Это переключатель, чтобы щелкать релющкой вручную
  jee.text("tm1", "Таймер № 1");
  jee.text("tm2", "Таймер № 2");

  jee.page(); // разделитель между страницами
  // Страница "Настройки MQTT"
  jee.formMqtt(); // готовая форма настроек MQTT

  jee.page(); // разделитель между страницами
  // Страница "Настройки WiFi"
  jee.formWifi(); // готовая форма настроек Wi-Fi
  //jee.button("rst", "#00dd00", "Reset", 1);
  jee.page(); // разделитель между страницами

}