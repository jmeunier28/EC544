#include <SoftwareSerial.h>
#include <math.h>

/*

VERY ACCURATE TEMPERATURE READINGS
----------------------------------
 
Better voltage reading:
  Used A2 pin (less jumpy)
  Got two measurements each pull, discarded the first

Better R_therm reading:
  Measured V_source accurately
  measured R1 accurately

Better temperature reading:
  Used accurate temperature sensor to get 3 data points
  Solved resulting system of equations for Steinhart-Hart coefficients
  Base conversion (needed base-2 instead of base-10)
  (temp slightly rising at start due to current self-heating unavoidable with thermistor)
  
*/


// Global variables
int AnalogPin = A2; // has much less jumpy readings than A0 (maybe A0 slightly damage from overuse)
float V_source = 4.9; // measured
SoftwareSerial XBee(2,3); // RX, TX



// Calculate thermistor voltage
float Voltage(float RawADC) {
  float meter_calibration = 0; // don't seem to need, matches my multimeter exactly
  float V_therm = (V_source * RawADC) / 1024.0;
  V_therm += meter_calibration;
  return V_therm;
}

// Calculate thermistor resistance
float Thermistor(float V_therm) {
  float R_known = 9000; // measured
  float R_therm =  ((R_known * V_therm) / (V_source - V_therm));
  return R_therm;
}

// Base conversion
float log2(float num) {
  return (log(num) / log(2));
}

// Calculate temperature
float Temperature(float R_therm) {
  float A = 0.0164872;
  float B = -0.00158538;
  float C = 0.0000033813;
  float temp_K = 1 / (A + B*log2(R_therm) + C*pow(log2(R_therm),3));
  return temp_K;
}

// Convert to Celsius or Fahrenheit
float ConvertTemp(float temp_K, char T) {
  if (T == 'C')  
    return temp_K - -273.15;
  else if (T == 'F') 
    return temp_K * 1.8 - 459.67;
}



// Setup
void setup() {
  Serial.begin(9600);
  pinMode(AnalogPin, INPUT);
  XBee.begin(9600);
}

// Main
void loop() {

  // ADC value for debugging
  // Also good to get two analog readings and discard the first (multiplexed-gives time to settle)
  Serial.print("Raw ADC value: ");
  Serial.println(analogRead(AnalogPin));
  
  // Thermistor voltage
  float V_therm = Voltage(analogRead(AnalogPin));
  Serial.print("Voltage across thermistor: ");
  Serial.println(V_therm);

  // Thermistor resistance
  float R_therm = Thermistor(V_therm);    
  Serial.print("Resistance of thermistor: ");
  Serial.println(R_therm);

  // Temperature in Kelvin
  float temp_K = Temperature(R_therm);
  Serial.print("Temperature in Kelvin: ");
  Serial.println(temp_K);

  // Temperature in Celsius and Fahrenheit
  float temp_C = ConvertTemp(temp_K, 'C');
  float temp_F = ConvertTemp(temp_K, 'F');
  Serial.print("Temperature in Celsius: ");
  Serial.println(temp_C);
  Serial.print("Temperature in Fahrenheit: ");
  Serial.println(temp_F);
  Serial.println();

  // XBee
  String mytemp = String(temp_F);
  String msg = String("A, " + mytemp);  
  XBee.print(msg);
  XBee.write("\n");
  
  delay(3000);
}
