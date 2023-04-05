/*
 * Project EE1301-smart-humidifier
 * Description: Smart humidifier that automatically adjust to fit the desired level.
 * Authors: Nadia Potteiger, Maya Bartholomew, Samanta Saucedo, Brenna Dietrich
 * Date: 3/29/23
 */

#include "PietteTech_DHT.h"

 // doing preprocessor for constants like this saves just a little bit of memory
 // because it substitutes the values in at compile time instead of referencing a variable
 // in memory
#define SENSOR_PIN D2
#define HEARTBEAT_LED D7
#define SERVO_PIN D1
#define DHT_TYPE DHT11
#define POLL_RATE 500

double temp_c;
double humidity;
unsigned long int poll_time;
bool LED_state = FALSE;

Servo output_servo;
PietteTech_DHT DHT(SENSOR_PIN, DHT_TYPE);

// setup() runs once, when the device is first turned on.
void setup() {
  // Put initialization like pinMode and begin functions here.
  pinMode(HEARTBEAT_LED, OUTPUT);
  // pinMode(SERVO_PIN, OUTPUT);
  Serial.begin(9600);
  DHT.begin();
  output_servo.attach(SERVO_PIN);
  Particle.variable("cV_temp", temp_c);
  Particle.variable("cV_humidity", humidity);
  poll_time = millis() + POLL_RATE;
}

// loop() runs over and over again, as quickly as it can execute.
void loop() {
  unsigned long int current_time = millis();
  //? this code is from the example and is helpful for error checking
  int result = DHT.acquireAndWait(1500); // wait up to 1.5 sec
  delay(POLL_RATE);

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

  if (current_time > poll_time) {
    if (result == DHTLIB_OK) {
      temp_c = DHT.getCelsius();
      humidity = DHT.getHumidity();
      Serial.printf("Humidity: %.0f%%\n", DHT.getHumidity());
      Serial.printf("Temp: %.0fC\n", DHT.getCelsius());
      if (LED_state) output_servo.write(90);
      else output_servo.write(0);
    }

    LED_state = !LED_state;
    digitalWrite(HEARTBEAT_LED, LED_state);

    poll_time += POLL_RATE;
  }
}