/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#include "Particle.h"
#line 1 "/home/nyadiia/Documents/school/EE1301/EE1301-smart-humidifier/src/EE1301-smart-humidifier.ino"
/*
 * Project EE1301-smart-humidifier
 * Description: Smart humidifier that automatically adjust to fit the desired level.
 * Authors: Nadia Potteiger, Maya Bartholomew, Samanta Saucedo, Brenna Dietrich
 * Date: 3/29/23
 */

#include "PietteTech_DHT.h"

void setup();
void loop();
#line 10 "/home/nyadiia/Documents/school/EE1301/EE1301-smart-humidifier/src/EE1301-smart-humidifier.ino"
const int SENSOR_PIN = D0;
const int HEARTBEAT_LED = D7;
const int DHT_TYPE = DHT11;
double temp_c;
double humidity;
PietteTech_DHT DHT(SENSOR_PIN, DHT11);


// setup() runs once, when the device is first turned on.
void setup() {
  // Put initialization like pinMode and begin functions here.
  // pinMode(SENSOR_PIN, INPUT);
  pinMode(HEARTBEAT_LED, OUTPUT);
  Serial.begin(9600);
  DHT.begin();
  Particle.variable("cV_temp", temp_c);
  Particle.variable("cV_humidity", humidity);
}

// loop() runs over and over again, as quickly as it can execute.
void loop() {

  //? this code is from the example and is helpful for error checking
  int result = DHT.acquireAndWait(1000); // wait up to 1 sec (default indefinitely)

  switch (result) {
  case DHTLIB_OK:
    Serial.println("OK");
    break;
  case DHTLIB_ERROR_CHECKSUM:
    Serial.println("Error\n\r\tChecksum error");
    break;
  case DHTLIB_ERROR_ISR_TIMEOUT:
    Serial.println("Error\n\r\tISR time out error");
    break;
  case DHTLIB_ERROR_RESPONSE_TIMEOUT:
    Serial.println("Error\n\r\tResponse time out error");
    break;
  case DHTLIB_ERROR_DATA_TIMEOUT:
    Serial.println("Error\n\r\tData time out error");
    break;
  case DHTLIB_ERROR_ACQUIRING:
    Serial.println("Error\n\r\tAcquiring");
    break;
  case DHTLIB_ERROR_DELTA:
    Serial.println("Error\n\r\tDelta time to small");
    break;
  case DHTLIB_ERROR_NOTSTARTED:
    Serial.println("Error\n\r\tNot started");
    break;
  default:
    Serial.println("Unknown error");
    break;
  }

  temp_c = DHT.getCelsius();
  humidity = DHT.getHumidity();
  Serial.printf("Humidity: %f %\n", DHT.getHumidity());
  Serial.printf("Temp: %fC\n", DHT.getCelsius());

  //! blocking code for now so it doesn't poll the sensor too much debugging that's readable
  digitalWrite(HEARTBEAT_LED, HIGH);
  delay(250);
  digitalWrite(HEARTBEAT_LED, LOW);
  delay(250);
}