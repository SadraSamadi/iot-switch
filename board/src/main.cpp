#include <Arduino.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>

SoftwareSerial sws(PD2, PD3);

void copy(Stream *src, Stream *dest);

void setup()
{
	pinMode(LED_BUILTIN, OUTPUT);
	pinMode(PD4, INPUT_PULLUP);
	Serial.begin(19200);
	sws.begin(19200);
}

void loop()
{
	if (digitalRead(PD4))
	{
		String line = sws.readStringUntil('\n');
		if (line != "")
		{
			Serial.println(line);
			DynamicJsonDocument json(256);
			DeserializationError error = deserializeJson(json, line);
			if (!error)
			{
				String state = json["state"];
				Serial.println("State: " + state);
				if (state == "on")
					digitalWrite(LED_BUILTIN, HIGH);
				else if (state == "off")
					digitalWrite(LED_BUILTIN, LOW);
			}
		}
	}
	else
	{
		copy(&Serial, &sws);
		copy(&sws, &Serial);
	}
}

void copy(Stream *src, Stream *dest)
{
	while (src->available())
		dest->write(src->read());
}
