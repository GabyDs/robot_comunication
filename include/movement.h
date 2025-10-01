#ifndef MOVEMENT_H
#define MOVEMENT_H

#include <Arduino.h>

// Declaraciones de funciones
void setupMotors();
void moveMotors(int leftSpeed, int rightSpeed);
void stopMotors();
void moveForward(int speed = 150);
void moveBackward(int speed = 150);
void turnLeft(int speed = 100);
void turnRight(int speed = 100);

#endif