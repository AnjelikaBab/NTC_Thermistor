#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#define DEVICE_NAME   "MRSensor"

#define BATTERY_SERVICE_UUID                "180F"
#define BATTERY_LEVEL_CHARACTERISTIC_UUID   "2A19"

#define THERMOMETER_SERVICE_UUID            "1809"
#define TEMPERATURE_CHARACTERISTIC_UUID     "2A1C"

/* Thermistor Constants */
const double VCC = 3.3;             // ESP32 on-board 3.3V VCC
const double R2 = 10000;            // 10k ohm series resistor
const double adc_resolution = 4095; // 12-bit ADC resolution for ESP32

const double A = 0.001129148;   // thermistor equation parameters
const double B = 0.000234125;
const double C = 0.0000000876741; 

/* Battery Level Constants */
const int batteryPin = 32;  // ADC pin to read battery voltage (adjust to your actual pin)
const double R1 = 6200;    // Resistor R1 (6.2k ohms)
const double R2_battery = 1000;    // Resistor R2 (1k ohms) for battery voltage divider
const double MIN_VOLTAGE = 6.0;  // Approximate voltage when battery is "empty"
const double MAX_VOLTAGE = 9.0;  // Fully charged 9V battery

/* Bluetooth */
BLECharacteristic *batteryCharacteristic;
BLECharacteristic *temperatureCharacteristic;

bool deviceConnected = false;

//Setup callbacks onConnect and onDisconnect
class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    deviceConnected = true;
    Serial.println("Device Connected");
  };
  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
    Serial.println("Device Disconnected");
    BLEDevice::startAdvertising();
  }
};

void initBluetooth() {
  Serial.println("Starting BLE");

  BLEDevice::init(DEVICE_NAME);
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  Serial.println("Server started");

  // create the battery service
  BLEService *batteryService = pServer->createService(BATTERY_SERVICE_UUID);
  // battery level set to read/notify (client can listen to but not write to the topic)
  batteryCharacteristic =
    batteryService->createCharacteristic(BATTERY_LEVEL_CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
  batteryService->start();

  // create the thermistor service
  BLEService *temperatureService = pServer->createService(THERMOMETER_SERVICE_UUID);
  // temperature set to read/notify (client can listen to but not write to the topic)
  temperatureCharacteristic = 
    temperatureService->createCharacteristic(TEMPERATURE_CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
  temperatureService->start();
  
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(BATTERY_SERVICE_UUID);
  pAdvertising->addServiceUUID(THERMOMETER_SERVICE_UUID);
  pAdvertising->setScanResponse(true);  
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  
  BLEDevice::startAdvertising();
  Serial.println("Characteristics defined");
}

int calculateBatteryLevel() {
  int full_adc_value = 3900;
  int dead_adc_value = 2600;
  // Battery Voltage Measurement Code
  int adc_value_battery = analogRead(batteryPin);  // Read ADC value for battery voltage
  float batteryPercentage = ((float) ((adc_value_battery - dead_adc_value)) / (full_adc_value - dead_adc_value)) * 100;

  Serial.println(adc_value_battery);
  Serial.println(batteryPercentage);

  // Display the battery voltage and remaining percentage

  return batteryPercentage;

}

double calculateTemperature() {  
  // Temperature Measurement Code
  double Vout, Rth, temperature, adc_value; 
  adc_value = analogRead(34);  
  Vout = (adc_value * VCC) / adc_resolution;
  Rth = (VCC * R2 / Vout) - R2;

  /* Steinhart-Hart Thermistor Equation:
   *  Temperature in Kelvin = 1 / (A + B[ln(R)] + C[ln(R)]^3)
   */
  temperature = (1 / (A + (B * log(Rth)) + (C * pow((log(Rth)), 3))));   // Temperature in Kelvin

  temperature = temperature - 221;  // Convert temperature to Celsius
  Serial.print("Temperature = ");
  Serial.print(temperature);
  Serial.println(" degree Celsius");

  return temperature;
}

void setup() {
  Serial.begin(115200);
  
  initBluetooth();
}

void loop() {
  // put your main code here, to run repeatedly:
  if (deviceConnected) {
    double temperature = calculateTemperature();
    float battery = calculateBatteryLevel();

    uint16_t parsedTemperature = (uint16_t) (temperature * 100);
    uint16_t parsedBattery = (uint16_t) (battery * 100);

    batteryCharacteristic->setValue(parsedBattery);
    batteryCharacteristic->notify();

    temperatureCharacteristic->setValue(parsedTemperature); 
    temperatureCharacteristic->notify();

    delay(5000);  // wait 5s before sending next value
  }
}
