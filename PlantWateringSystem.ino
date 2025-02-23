// Include the necessary libraries
#define BLYNK_TEMPLATE_ID "TMPL3YzovCMnj"
#define BLYNK_TEMPLATE_NAME "Plant Watering System"
#include <LiquidCrystal_I2C.h>
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <M2M_LM75A.h>

// Initialize the LM75A sensor
M2M_LM75A lm75;

// Initialize the LCD display
LiquidCrystal_I2C lcd(0x27, 16, 2);

char auth[] = "s07zC6sjCmTOgyIyt_HldC2Ik_dfNMWg"; // Replace with your Blynk Auth Token
char ssid[] = "Tenda_4E0A48";                      // Replace with your Wi-Fi SSID
char pass[] = "91372376";                          // Replace with your Wi-Fi Password

BlynkTimer timer;
bool Relay = 0;

// Define component pins
#define sensor A0       // Soil moisture sensor pin
#define waterPump D3    // Water pump relay pin

// Variables to store readings
int moisture = 0;
float temperature = 0.0;

void setup() {
  Serial.begin(9600);
  pinMode(waterPump, OUTPUT);
  digitalWrite(waterPump, HIGH); // Keep the water pump off initially
  
  lcd.init();
  lcd.backlight();

  // Blynk setup
  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);

  lcd.setCursor(1, 0);
  lcd.print("System Loading");
  for (int a = 0; a <= 15; a++) {
    lcd.setCursor(a, 1);
    lcd.print(".");
    delay(300);
  }
  lcd.clear();

  // Initialize I2C communication for LM75A sensor
  Wire.begin(D2, D1); // SDA on D2, SCL on D1 (NodeMCU)

  // Verify LM75A connection
  if (lm75.isConnected()) {
    Serial.println("LM75A sensor connected!");
    lcd.setCursor(0, 0);
    lcd.print("LM75 Ready!");
    delay(2000);
    lcd.clear();
  } else {
    Serial.println("LM75A not found!");
    lcd.setCursor(0, 0);
    lcd.print("LM75 Error!");
    delay(2000);
    lcd.clear();
  }

  // Set timers for periodic tasks
  timer.setInterval(1000L, updateReadings); // Read sensors every 1 second
  timer.setInterval(500L, displayReadings); // Update LCD every 0.5 seconds
}

// Blynk button handler to control the water pump
BLYNK_WRITE(V1) {
  Relay = param.asInt();

  if (Relay == 1) {
    digitalWrite(waterPump, LOW);
    displayPumpStatus("Pump: ON");
  } else {
    digitalWrite(waterPump, HIGH);
    displayPumpStatus("Pump: OFF");
  }
}

// Function to display pump status temporarily
void displayPumpStatus(const char *status) {
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print(status);
  delay(2000); // Display for 2 seconds
  lcd.clear();
}

// Function to read soil moisture and temperature
void updateReadings() {
  // Read soil moisture
  int value = analogRead(sensor);
  value = map(value, 0, 1024, 0, 100); // Map raw data to percentage
  moisture = (value - 100) * -1;       // Invert for correct scaling

  // Read temperature
  if (lm75.isConnected()) {
    temperature = lm75.getTemperature();
  } else {
    temperature = 0.0; // Set to 0 if sensor is disconnected
    Serial.println("LM75A not connected!");
  }

  // Send data to Blynk
  Blynk.virtualWrite(V0, moisture);
  Blynk.virtualWrite(V2, temperature);
}

// Function to display readings on LCD
void displayReadings() {
  lcd.setCursor(0, 0);
  lcd.print("Moist:");
  lcd.print(moisture);
  lcd.print("% ");

  lcd.setCursor(0, 1);
  lcd.print("Temp:");
  lcd.print(temperature);
  lcd.print("C  ");
}

void loop() {
  Blynk.run();  // Run the Blynk library
  timer.run();  // Run the Blynk timer
}
