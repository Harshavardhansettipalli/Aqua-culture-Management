#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <BlynkSimpleEsp32.h>
#define TRIGGER_PIN 25 // Trigger pin of
the HC
-SR04
#define ECHO_PIN 21 // Echo pin of the
HC
-SR04
#define ONE_WIRE_BUS 14 // Data wire
for DS18B20
#define PH_SENSOR_PIN 24 // Analog pin
for pH sensor
LiquidCrystal_I2C lcd(0x3F, 16, 2);
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
char auth[] =
"Zc50BZo8gUEMOK9ROtMCO7I3A";
char ssid[] = "Fish Tank";
char pass[] = "fishtank01";
float calibration_value = 31.34
- 0.7;
unsigned long int avgval;
int buffer_arr[10], temp;
void setup() {
 Blynk.begin(auth, ssid, pass,
"blynk.cloud", 80);
 Serial.begin(9600);
 lcd.init();
 lcd.backlight();
 sensors.begin();
 pinMode(TRIGGER_PIN, OUTPUT);
 pinMode(ECHO_PIN, INPUT);
17
 Serial.println("Ready"); // Test the serial
monitor
 Blynk.notify("Your device is online."); }
void loop() {
 Blynk.run();
 sensors.requestTemperatures();
 float temperature =
sensors.getTempCByIndex(0);
 for (int i = 0; i < 10; i++) {
 buffer_arr[i] =
analogRead(PH_SENSOR_PIN);
 delay(30);

}
 for (int i = 0; i < 9; i++) {
 for (int j = i + 1; j < 10; j++) {
 if (buffer_arr[i] > buffer_arr[j]) {
 temp = buffer_arr[i];
 buffer_arr[i] = buffer_arr[j];
 buffer_arr[j] = temp;

}

}

}
 avgval = 0;
 for (int i = 2; i < 8; i++)
 avgval += buffer_arr[i];
 float volt = (float)avgval * 5.0 / 4096 / 6;
 float pHValue =
-5.70 * volt +
calibration_value; // Convert the analog into
pH value
 Blynk.virtualWrite(V2, pHValue);
 Serial.print("pH: ");
 Serial.println(pHValue, 2);
 lcd.clear();
 lcd.setCursor(0, 0);
 lcd.print("pH:");
 lcd.setCursor(3, 0);
 lcd.print(String(pHValue, 2));
 lcd.setCursor(0, 1);
 lcd.print("Temp:" + String(temperature,
2));
 Blynk.virtualWrite(V0, temperature);
 // Check water level
 long duration, distance;
 digitalWrite(TRIGGER_PIN, LOW);
 delayMicroseconds(2);
 digitalWrite(TRIGGER_PIN, HIGH);
 delayMicroseconds(10);
18
digitalWrite(TRIGGER_PIN, LOW);
 duration = pulseIn(ECHO_PIN, HIGH);
 distance = (duration / 2) / 29.1; // Convert
the duration to centimeters
 Blynk.virtualWrite(V1, distance);
 if (distance > 10) {
 Blynk.logEvent("water_low");
 }
 // Check temperature
 if (temperature < 20) {
 Blynk.logEvent("temperature_low");
 }
 if (temperature > 40) {
 Blynk.logEvent("temperature_high");
 }
 delay(1000); // Adjust the delay as needed
