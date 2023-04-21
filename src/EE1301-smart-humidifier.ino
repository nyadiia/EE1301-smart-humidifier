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
#define POLL_RATE 2000  // read every 2 sec
#define TOLERANCE 5     // tolerance is defined from the sensor accuracy

double temp_c, humidity, desired_humidity;
unsigned long int poll_time;
bool LED_state = FALSE;
bool servo_state = FALSE;

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
  // cloud variables for the webpage
  Particle.variable("cV_temp", temp_c);
  Particle.variable("cV_humidity", humidity);
  Particle.variable("cV_humidity_setpoint", desired_humidity);
  // non-blocking method but doesn't read the sensor too much
  poll_time = millis() + POLL_RATE;
}

// loop() runs over and over again, as quickly as it can execute.
void loop() {
  unsigned long int current_time = millis();

  // if the current time is greater than the poll delay, run the code
  if (current_time > poll_time) {
    int result;
    acquire_data(result);
    // if we're actually able to read the sensor
    if (result == DHTLIB_OK) {
      temp_c = DHT.getCelsius();
      humidity = DHT.getHumidity();
      Serial.printf("Humidity: %.0f%%\n", DHT.getHumidity());
      Serial.printf("Temp: %.0fC\n", DHT.getCelsius());
      desired_humidity = 40;
      // this is pwm like control. if the current humidity is above the desired minus the
      // tolerance that was speified, turn off the humidifier
      if (humidity >= desired_humidity-TOLERANCE) {
        // turn it off
        turn_servo(FALSE);
      }
      // otherwise turn it on
      else {
        turn_servo(TRUE);
      }
    }

    // toggle LED
    LED_state = !LED_state;
    digitalWrite(HEARTBEAT_LED, LED_state);

    // add poll rate (ms) to poll time to reset the whole loop
    poll_time += POLL_RATE;
  }
}

void turn_servo(bool state) {
  if (state && servo_state)
    // if it's already on, don't do anything
    output_servo.write(90);
  else if (state) {
    // if it's on, set it to 180 degrees with continuous rotation servo
    // this turns it on
    output_servo.write(180);

    // delay for 1 second to let the servo turn the dial fully
    delay(1000);
    output_servo.write(90);
    servo_state = state;
  }
  else
    // if it's off, set it to 90 degrees with continuous rotation servo
    // this stops it
    output_servo.write(90);
    servo_state = state;
}

// this code is from the example and is helpful for error checking
void acquire_data(int &result) {
  result = DHT.acquireAndWait(2000);

  switch (result) {
  case DHTLIB_OK:
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
}