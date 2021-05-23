#include <ESP32Servo.h>
#include "are_servo_config.h"

Servo servo_A;  // create servo object to control a servo
Servo servo_B;  // create servo object to control a servo
Servo servo_C;  // create servo object to control a servo
Servo servo_D;  // create servo object to control a servo
Servo servo_E;  // create servo object to control a servo
Servo servo_F;  // create servo object to control a servo

int servo_A_pin = SERVO_A_PIN;
int servo_A_dutycycle = SERVO_A_DUTYCYCLE;
int servo_A_pulsewidth_min = SERVO_A_MIN_PULSEWIDTH;
int servo_A_angle_min = SERVO_A_MIN_ANGLE;
int servo_A_pulsewidth_max = SERVO_A_MAX_PULSEWIDTH;
int servo_A_angle_max = SERVO_A_MAX_ANGLE;
int servo_A_angle = SERVO_A_INITIAL_ANGLE;
int servo_A_speed = SERVO_A_DEFAULT_SPEED;
int servo_A_update_period = SERVO_A_UPDATE_PERIOD;

float servo_A_position;

// 16 servo objects can be created on the ESP32
// Recommended PWM GPIO pins on the ESP32 include 2,4,12-19,21-23,25-27,32-33  

float get_pw_of_angle(float angle) {
  // calculate target pulse widtg from angle
    float rel_pos = (angle - servo_A_angle_min) / (servo_A_angle_max - servo_A_angle_min);
    float pw = servo_A_pulsewidth_min + rel_pos * (servo_A_pulsewidth_max - servo_A_pulsewidth_min);
    return pw;
}

void servo_set_angle_instant(Servo * servo, float target_angle) {
    float target_pw = get_pw_of_angle(target_angle);
    Serial.print("Setting pulse width Servo A to ");
    Serial.print(target_pw);
    Serial.print(" (");
    Serial.print(target_angle);
    Serial.print("°) ... ");
    servo->write(target_pw);
}

void servo_A_set_angle(float target_angle) {

    // determine required speed
    float d_pw = servo_A_pulsewidth_max - servo_A_pulsewidth_min;
    float steps_per_ms = d_pw / servo_A_speed;

    float target_pw = get_pw_of_angle(target_angle);

    Serial.print("Setting pulse width Servo A to ");
    Serial.print(target_pw);
    Serial.print(" (");
    Serial.print(target_angle);
    Serial.print("°) ... ");
    
    // move up wards if too low
    while (servo_A_position < target_pw - steps_per_ms * servo_A_update_period) {
        servo_A_position += steps_per_ms * servo_A_update_period;
        servo_A.write((int)servo_A_position);
        //Serial.println((int)servo_A_position);
        delay(servo_A_update_period); 
    }

    // move down if too high
    while (servo_A_position > target_pw + steps_per_ms * servo_A_update_period) {
        servo_A_position -= steps_per_ms * servo_A_update_period;
        servo_A.write((int)servo_A_position);
        //Serial.println((int)servo_A_position);
        delay(servo_A_update_period); 
    }

    //finally, set exact position
    Serial.println("done.");
}
