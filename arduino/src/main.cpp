#include <Arduino.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>

// Define a virtual UART serial connector on pins number 2 and 3.
// This serial will be connected to the WiFi module.
SoftwareSerial sws(PD2, PD3);

void copy(Stream *src, Stream *dest);

void setup()
{
  // Set builtin LED mode to output.
  pinMode(LED_BUILTIN, OUTPUT);
  // Set pin number 4 mode to input with default HIGH value.
  pinMode(PD4, INPUT_PULLUP);
  // Initial the default serial that is connected to the PC.
  Serial.begin(19200);
  // Initial the virtual serial.
  sws.begin(19200);
}

void loop()
{
  // Check the pin number 4 value.
  if (digitalRead(PD4))
  {
    // IDLE MODE
    // Get data from the WiFi module and check the state.
    // Read a line from WiFi module.
    String line = sws.readStringUntil('\n');
    if (line != "")
    {
      // Send the line to the default serial.
      Serial.println(line);
      // Parse the line as a JSON with maximum length of 265.
      DynamicJsonDocument json(256);
      DeserializationError error = deserializeJson(json, line);
      if (!error)
      {
        // Get the state value.
        String state = json["state"];
        // Send the state to the default serial.
        Serial.println("State: " + state);
        // Set the builtin LED value based on the state value.
        if (state == "on")
          digitalWrite(LED_BUILTIN, HIGH);
        else if (state == "off")
          digitalWrite(LED_BUILTIN, LOW);
      }
    }
  }
  else
  {
    // PROGRAMMING MODE
    // Act as a connector between the PC and the WiFi module and do nothing more.
    copy(&Serial, &sws);
    copy(&sws, &Serial);
  }
}

// Copy all available data in the source stream to the destination.
void copy(Stream *src, Stream *dest)
{
  while (src->available())
    dest->write(src->read());
}
