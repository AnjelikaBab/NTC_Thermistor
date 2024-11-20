const double VCC = 3.3;             // ESP32 on-board 3.3V VCC
const double R2 = 10000;            // 10k ohm series resistor
const double adc_resolution = 4095; // 12-bit ADC resolution for ESP32

const double A = 0.001129148;   // thermistor equation parameters
const double B = 0.000234125;
const double C = 0.0000000876741; 

//const int batteryPin = 32;  // ADC pin to read battery voltage (adjust to your actual pin)
//const double R1 = 47000;    // Resistor R1 (6.2k ohms)
//const double R2_battery = 22000;    // Resistor R2 (1k ohms) for battery voltage divider
//const double MIN_VOLTAGE = 6.0;  // Approximate voltage when battery is "empty"
//const double MAX_VOLTAGE = 9.0;  // Fully charged 9V battery

void setup() {
  Serial.begin(115200);  // Define baud rate for serial communication
}

void loop() {
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

//  // Battery Voltage Measurement Code
//  int adc_value_battery = analogRead(batteryPin);  // Read ADC value for battery voltage
//  double Vout_battery = (adc_value_battery * VCC) / adc_resolution; // Convert ADC to voltage
//  double batteryVoltage = Vout_battery * (R1 + R2_battery) / R2_battery;  // Calculate battery voltage using voltage divider formula
//  
//  // Map battery voltage to percentage
//  int batteryPercentage = map(batteryVoltage * 1000, MIN_VOLTAGE * 1000, MAX_VOLTAGE * 1000, 0, 100);
//
//  // Constrain battery percentage to 0-100% range
//  batteryPercentage = constrain(batteryPercentage, 0, 100);

//  // Display the battery voltage and remaining percentage
//  Serial.print("Battery Voltage = ");
//  Serial.print(batteryVoltage);
//  Serial.print(" V, ");
//  Serial.print("Battery Remaining = ");
//  Serial.print(batteryPercentage);
//  Serial.println("%");
  
  delay(1000);  // Delay for 1 second before the next reading
}
