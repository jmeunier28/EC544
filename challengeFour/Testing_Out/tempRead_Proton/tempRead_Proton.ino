#include <math.h>
const int PULLUP_RES = 10000; // in Ohm( 10kOm )
const double BETA = 10000; // in K for Semitec 104 GTA-2
const double THERMISTOR_RES = 100000; // in Ohm
const double THERMISTOR_NOM_TEMP = 25; // Celsius, C
void setup()
{
}
void loop()
{
 thermister_temp(analogRead(4));
 delay(3000);
}
void thermister_temp(int aval)
{
 double R, T, C, F;
R = (double) PULLUP_RES / ( (4095 / (double) aval ) - 1 );
 
 T = 1 / ( ( 1 / (THERMISTOR_NOM_TEMP + 273.15 )) + ( ( 1 / BETA) * log ( R / THERMISTOR_RES ) ) );
 
  C = T - 273.15; // converting to C from K
 F = T * 1.8 - 459.47; // converting to F from K
 
 // return degrees C
 Spark.publish("Temperature (°C)", String(C) + " °C");
 Spark.publish("Temperature (°F)", String(F) + " °F");
}
