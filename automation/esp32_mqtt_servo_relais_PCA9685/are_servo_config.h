#define SERVO_A_PIN 13;
// 16 servo objects can be created on the ESP32
// Recommended PWM GPIO pins on the ESP32 include 2,4,12-19,21-23,25-27,32-33  

#define SERVO_A_DUTYCYCLE 20;  // dutycycle [ms], a standard 50Hz servo has 20 ms dutycycle
#define SERVO_A_MIN_PULSEWIDTH 1000;
#define SERVO_A_MAX_PULSEWIDTH 2000;
#define SERVO_A_MIN_ANGLE -90;
#define SERVO_A_MAX_ANGLE 90;
#define SERVO_A_UPDATE_PERIOD 10;

#define SERVO_A_CLOSE -90;
#define SERVO_A_OPEN 90;
 

#define SERVO_A_DEFAULT_SPEED 50; // time [ms] to move full sweep
#define SERVO_A_INITIAL_ANGLE 0;

/*  SETTING:
 *     
 *     AZ-Delivery MG996:
 *         dutycycle: 20 ms (50Hz)
 *         pulsewidth 500 us - 2500 ms
 *         angle:     0° - 180°
 *     
 *     AZ-Delivery MG996:
 *         dutycycle: 20 ms (50Hz)
 *         pulsewidth 500 us - 2500 ms
 *         angle:     0° - 190°
 *     
 *     KUMAN KY66&SG9:
 *         dutycycle: 20 ms (50Hz)
 *         pulsewidth 1000 us - 2000 ms
 *         angle:     -90° - +90°
 *     


 * 
 * 
 */
