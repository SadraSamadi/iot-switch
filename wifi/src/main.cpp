#include <Arduino.h>
#include <Ticker.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <SocketIoClient.h>
#include <WiFiManager.h>
#include <ArduinoJson.h>

// Define General Porpose Input Output (GPIO) pin numbers.
#define GPIO_0 0
#define GPIO_2 2

void tick();

void state(const char *payload, size_t length);

// Define a ticker to call the tick function every 500 ms.
Ticker ticker(tick, 500);
// Define socket io client.
SocketIoClient socket;

void setup()
{
  // Initialize pin states.
  pinMode(GPIO_0, OUTPUT);
  pinMode(GPIO_2, OUTPUT);
  digitalWrite(GPIO_0, HIGH);
  digitalWrite(GPIO_2, LOW);
  // Initialize the default serial.
  Serial.begin(19200);
  Serial.println();
  Serial.println("Connecting...");
  // Create a WiFi manager to connect the module to an available modem.
  // Connect to the ESP WiFi then go to 192.168.4.1 with any browser.
  WiFiManager wifiManager;
  // Clear the previous settings.
  wifiManager.resetSettings();
  // Start the manager with default configuration and wait for a connection.
  wifiManager.autoConnect();
  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());
  // Start blinking.
  ticker.start();
  // Listen to the state event.
  socket.on("state", state);
  // Connect to the socket server.
  socket.begin("iot-switch-server.herokuapp.com");
  Serial.println("Socket started");
}

void loop()
{
  // Update ticher and socket continuesly.
  ticker.update();
  socket.loop();
}

// Will be called every 500 ms.
void tick()
{
  // Blink the pin number 0.
  int d = digitalRead(GPIO_0);
  digitalWrite(GPIO_0, !d);
}

// Will be called when the state is changed.
void state(const char *payload, size_t length)
{
  // Cast payload to string.
  String value(payload);
  Serial.printf("State (%d): ", length);
  Serial.println(value);
  // Create an empty json with maximum length of 256 characters.
  DynamicJsonDocument json(256);
  json["state"] = value;
  // Send the json to the default serial.
  serializeJson(json, Serial);
  Serial.println();
  // Check the state and change the LED value.
  if (value == "on")
    digitalWrite(GPIO_2, HIGH);
  else if (value == "off")
    digitalWrite(GPIO_2, LOW);
}
