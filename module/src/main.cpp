#include <Arduino.h>
#include <Ticker.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <SocketIoClient.h>
#include <WiFiManager.h>
#include <ArduinoJson.h>

#define GPIO_0 0
#define GPIO_2 2

void tick();

void state(const char *payload, size_t length);

Ticker ticker(tick, 500);
SocketIoClient socket;

void setup()
{
	pinMode(GPIO_0, OUTPUT);
	pinMode(GPIO_2, OUTPUT);
	digitalWrite(GPIO_0, HIGH);
	digitalWrite(GPIO_2, LOW);
	Serial.begin(19200);
	Serial.println();
	Serial.println("Connecting...");
	WiFiManager wifiManager;
	wifiManager.resetSettings();
	wifiManager.autoConnect();
	Serial.print("Connected, IP address: ");
	Serial.println(WiFi.localIP());
	ticker.start();
	socket.on("state", state);
	socket.begin("iot-switch-server.herokuapp.com");
	Serial.println("Socket started");
}

void loop()
{
	ticker.update();
	socket.loop();
}

void tick()
{
	int d = digitalRead(GPIO_0);
	digitalWrite(GPIO_0, !d);
}

void state(const char *payload, size_t length)
{
	String value(payload);
	Serial.printf("State (%d): ", length);
	Serial.println(value);
	DynamicJsonDocument json(256);
	json["state"] = value;
	serializeJson(json, Serial);
	Serial.println();
	if (value == "on")
		digitalWrite(GPIO_2, HIGH);
	else if (value == "off")
		digitalWrite(GPIO_2, LOW);
}
