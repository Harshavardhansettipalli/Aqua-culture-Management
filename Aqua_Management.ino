#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <OneWire.h>
#include <DallasTemperature.h>

const char* ssid = "your_SSID"; // replace with your WiFi SSID
const char* password = "your_PASSWORD"; // replace with your WiFi password

const int pHpin = A0;

#define ONE_WIRE_BUS 4
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

const int trigPin = 5;
const int echoPin = 18;

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  lcd.begin();
  lcd.backlight();
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  sensors.begin();
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

void loop() {
  int pHValue = analogRead(pHpin);
  float voltage = pHValue * (5.0 / 1024.0);
  float pH = 3.5 * voltage;

  sensors.requestTemperatures();
  float temperature = sensors.getTempCByIndex(0);

  long duration;
  float distance;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("pH: ");
  lcd.print(pH);
  lcd.setCursor(0, 1);
  lcd.print("Temp: ");
  lcd.print(temperature);
  lcd.print(" C");

  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    if (client.connect("your.server.com", 80)) { // replace with your server URL
      String getData = "GET /update?api_key=your_API_key"; // replace with your API key
      getData += "&pH=";
      getData += String(pH);
      getData += "&temperature=";
      getData += String(temperature);
      getData += "&distance=";
      getData += String(distance);
      getData += " HTTP/1.1";
      client.println(getData);
      client.println("Host: your.server.com"); // replace with your server URL
      client.println("Connection: close");
      client.println();
      client.stop();
    }
  }

  delay(2000);
}