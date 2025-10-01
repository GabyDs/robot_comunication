#include "movement.h"

// Pines para control de motores
#define MOTOR_LEFT_PWM   2   // pwma
#define MOTOR_RIGHT_PWM  21  // pwmb
#define MOTOR_LEFT_DIR1  8   // ma1
#define MOTOR_LEFT_DIR2  7   // ma2
#define MOTOR_RIGHT_DIR1 10  // mb1
#define MOTOR_RIGHT_DIR2 20  // mb2
#define STANDBY_PIN      9   // stby

void setupMotors() {
  // Configurar PWM
  ledcSetup(0, 2600, 8);  // Canal 0, 2600Hz, 8-bit
  ledcSetup(1, 2600, 8);  // Canal 1, 2600Hz, 8-bit
  
  // Asignar canales PWM a pines
  ledcAttachPin(MOTOR_LEFT_PWM, 0);
  ledcAttachPin(MOTOR_RIGHT_PWM, 1);
  
  // Configurar pines de dirección
  pinMode(MOTOR_LEFT_DIR1, OUTPUT);
  pinMode(MOTOR_LEFT_DIR2, OUTPUT);
  pinMode(MOTOR_RIGHT_DIR1, OUTPUT);
  pinMode(MOTOR_RIGHT_DIR2, OUTPUT);
  pinMode(STANDBY_PIN, OUTPUT);
  
  // Activar driver de motores
  digitalWrite(STANDBY_PIN, HIGH);
}

void moveMotors(int leftSpeed, int rightSpeed) {
  // Usar la misma lógica que la función motores() que funcionaba
  leftSpeed = constrain(leftSpeed, -255, 255);
  rightSpeed = constrain(rightSpeed, -255, 255);
  
  // Motor izquierdo (misma lógica que motores())
  if (leftSpeed < 0) {
    digitalWrite(MOTOR_LEFT_DIR2, HIGH);
    digitalWrite(MOTOR_LEFT_DIR1, LOW);
  } else {
    digitalWrite(MOTOR_LEFT_DIR2, LOW);
    digitalWrite(MOTOR_LEFT_DIR1, HIGH);    
  }
  
  // Motor derecho (misma lógica que motores())
  if (rightSpeed < 0) {    
    digitalWrite(MOTOR_RIGHT_DIR2, HIGH);
    digitalWrite(MOTOR_RIGHT_DIR1, LOW);
  } else {
    digitalWrite(MOTOR_RIGHT_DIR2, LOW);
    digitalWrite(MOTOR_RIGHT_DIR1, HIGH);    
  }
  
  // Aplicar PWM
  ledcWrite(0, abs(leftSpeed));  // Using channel 0 for pwma
  ledcWrite(1, abs(rightSpeed)); // Using channel 1 for pwmb
}

void stopMotors() {
  moveMotors(0, 0);
}

void moveForward(int speed) {
  moveMotors(speed, speed);
}

void moveBackward(int speed) {
  moveMotors(-speed, -speed);
}

void turnLeft(int speed) {
  moveMotors(-speed, speed);
}

void turnRight(int speed) {
  moveMotors(speed, -speed);
}