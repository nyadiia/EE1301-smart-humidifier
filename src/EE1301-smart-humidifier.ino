/*
 * Project EE1301-smart-humidifier
 * Description: Smart humidifier that automatically adjust to fit the desired level.
 * Authors: Nadia Potteiger, Maya Bartholomew, Samanta Saucedo, Brenna Dietrich
 * Date: 3/29/23
 */

#include "PietteTech_DHT.h"
#include "pid.h"
// doing preprocessor for constants like this saves just a little bit of memory
// because it substitutes the values in at compile time instead of referencing a variable
// in memory
#define SENSOR_PIN D2
#define HEARTBEAT_LED D7
#define SERVO_PIN D1
#define DHT_TYPE DHT11
#define POLL_RATE 1000  // read every 1 sec
#define TOLERANCE 5     // tolerance is defined from the sensor accuracy
#define __DEBUG

double temp_c, humidity, desired_humidity;

unsigned long int poll_time;
bool LED_state = FALSE;


Servo output_servo;
double servo_pos;

double* Setpoint = &desired_humidity;
double* Input = &humidity;
// point to servo later
double* Output = &servo_pos;
//Specify the links and initial tuning parameters
PID pid_controller(Input, Output, Setpoint, 8, 0.5, 0, PID::DIRECT);
PietteTech_DHT DHT(SENSOR_PIN, DHT_TYPE);

// setup() runs once, when the device is first turned on.
void setup() {
  // Put initialization like pinMode and begin functions here.
  pinMode(HEARTBEAT_LED, OUTPUT);
  // pinMode(SERVO_PIN, OUTPUT);
  Serial.begin(9600);
  DHT.begin();
  output_servo.attach(SERVO_PIN);
  pid_controller.SetMode(PID::AUTOMATIC);
  // cloud variables for the webpage
  Particle.variable("cV_temp", temp_c);
  Particle.variable("cV_humidity", humidity);
  Particle.function("cF_humidity_setpoint", humidity_setpoint_from_string);
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
      pid_controller.Compute();
      output_servo.write(5*servo_pos);

      #ifdef __DEBUG
      desired_humidity = 40;
      Serial.printf("Temp: %.0fC\n", temp_c);
      Serial.printf("Humidity: %.0f%%\n", humidity);
      Serial.printf("Setpoint: %.0f%%\n", desired_humidity);
      Serial.printf("Output: %0.2f\n\n", 5*servo_pos);
      #endif      
    }

    // toggle LED
    LED_state = !LED_state;
    digitalWrite(HEARTBEAT_LED, LED_state);

    // add poll rate (ms) to poll time to reset the whole loop
    poll_time += POLL_RATE;
  }
}

int humidity_setpoint_from_string(String input_string) {
  desired_humidity = input_string.toFloat();
  return 1;
}

// this code is from the example and is helpful for error checking
void acquire_data(int &result) {
  result = DHT.acquireAndWait();

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