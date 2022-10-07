void onConnect(){ // функция вызывается при подключении к MQTT 
	if (mqtt_enable) {
		// тут пользовательские подписки
  	Serial.println("Connected to mqtt");
		//   jee.subscribe("custom"); // подписка на произвольный топик результат - подписка на топик ID/custom
	}
}

void mqttCallback(String topic, String payload){ // функция вызывается, когда приходят данные MQTT 
	if (mqtt_enable) {
		// тут пользовательская обработка входящих mqtt сообщений
  	Serial.println("Message [" + topic + " - " + payload + "] ");
	}
}

