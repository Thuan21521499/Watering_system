#define BLYNK_TEMPLATE_ID "TMPL6bfjKkBo3"
#define BLYNK_TEMPLATE_NAME "HE THONG TUOI TU DONG"
#define BLYNK_AUTH_TOKEN "3cX7Knon1URNMMGIuB-OyYIlAHK9UhBz"
#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include "DHT.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
//#include <FastLED.h>

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "Duonguit";
char pass[] = "888888888";

#define DHTPIN 17
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);
#define SENSOR_PIN 35
//const int soilMoisturePin = 36; // Analog pin for the soil moisture sensor
//const int ledPin = 2; // LED control pin
//const int pumpPin = 25; // Pump control pin
#define pumpPin 16
const int SOIL_MOISTURE_LOW = 50;
const int SOIL_MOISTURE_HIGH = 80;

int pumpState = LOW; // Initialize pumpState to LOW (off)
int pumpMode = 0; // 0 for automatic, 1 for manual
float mappedSoilMoisture = 0; // Declare mappedSoilMoisture as a global variable

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1 // Reset pin # (or -1 if sharing Arduino reset pin)
   Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

BLYNK_WRITE(V2) { // Manual Control of the pump
  if (pumpMode == 1) { // Check if the mode is manual
    pumpState = param.asInt(); // Read the state of the pump button (0 for OFF, 1 for ON)
    digitalWrite(pumpPin, pumpState); // Turn the pump on or off based on button state
    //notifyPumpState(pumpState); // Send notification about pump state
  }
}

BLYNK_WRITE(V4) { // Change the pump mode
  pumpMode = param.asInt(); // Read the state of the mode button (0 for automatic, 1 for manual)
  
}

void setup() {
  Serial.begin(115200);
  pinMode(pumpPin, OUTPUT);
  dht.begin();
  Blynk.begin(auth, ssid, pass);
  Blynk.virtualWrite(V4, LOW); // Initialize the mode button to automatic mode
  Blynk.virtualWrite(V2, LOW); // Initialize the pump button to OFF
  // Khởi tạo màn hình OLED
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.display();
  delay(2000); // Pause for 2 seconds
  display.clearDisplay();
}
void loop() {
  Blynk.run();
  
  unsigned long currentMillis = millis();
  static unsigned long previousMillis = 0;
  const long interval = 1000;

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    
    int soilSensorValue = analogRead(SENSOR_PIN);
      mappedSoilMoisture = map(soilSensorValue, 1400 , 4095, 100, 0);
      Blynk.virtualWrite(V3, mappedSoilMoisture); 
      Serial.printf("Soil Moisture: ");
      Serial.print(mappedSoilMoisture);
      Serial.println("%");
    if (pumpMode == 0) {  
      autoControlPump(mappedSoilMoisture); // Calling for auto if mode = 1
    }
    //controlLED(mappedSoilMoisture);
    readAndSendSensorData();
  
  }
    
}

void autoControlPump(float mappedSoilMoisture) {
  if (pumpMode == 0) {
    Blynk.virtualWrite(V2, pumpState);
  }
  //int previousPumpState = pumpState; // Save the previous pump state
  
  if (mappedSoilMoisture <= SOIL_MOISTURE_LOW) {
    pumpState = HIGH;
  } else if (mappedSoilMoisture >= SOIL_MOISTURE_HIGH) {
    pumpState = LOW;
  }
  digitalWrite(pumpPin, pumpState);
   
 /* if (pumpState != previousPumpState) {
    notifyPumpState(pumpState); // Send notification if pump state changes
  }
*/
  
}

/*void controlLED(float mappedSoilMoisture) {
  if (mappedSoilMoisture <= SOIL_MOISTURE_LOW) {
    //digitalWrite(ledPin, HIGH);
    Blynk.virtualWrite(V2, HIGH);
    //Blynk.setProperty(V2, "color", "#00FF00");
    //Blynk.virtualWrite(V2, "LED ON");
  } else {
    digitalWrite(ledPin, LOW);
   // Blynk.virtualWrite(V2, LOW);
   // Blynk.virtualWrite(V2, "LED OFF");
  }
}
*/
void readAndSendSensorData() {
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  Serial.print(F("Humidity: "));
  Serial.print(humidity);
  Serial.print(F("%  Temperature: "));
  Serial.print(temperature);
  Serial.print("\n");
  Blynk.virtualWrite(V0, temperature);
  Blynk.virtualWrite(V1, humidity); // Update humidity on Blynk
   
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print("Nhiet do: ");
  display.print(temperature);
  display.print(" C");

  display.setCursor(0, 10);
  display.print("Do am: ");
  display.print(humidity);
  display.print(" %");

  display.setCursor(0, 20);
  display.print("Do am dat: ");
  display.print(mappedSoilMoisture);
  display.print(" %");

  display.display();
}



