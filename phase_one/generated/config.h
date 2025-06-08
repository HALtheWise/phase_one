// MarIOnette generated config file for motors and LEDs //
#define BAUD_RATE_SERIAL 115200

#define SD_ENABLE 0
#define USE_SD_INTERNAL 0
#define SD_CS_PIN 0

#define TOTAL_MOTORS 1
#define TOTAL_SERVOS 1
#define TOTAL_PWM_MOTORS 0
#define TOTAL_STEPPERS 0
#define TOTAL_BUS_SERVOS 0
#define TOTAL_DYNAMIXELS 0

#define TOTAL_LEDS 0
#define TOTAL_PWM_LEDS 0
#define TOTAL_NEOPIXELS 0

unsigned long motor_values[1][10] = {{1, 4, 0, 0, 0, 0, 0, 0, 0, 0}};

unsigned long led_values[0][0];
#include "Arduino.h"
#include "animations.h"

// For AVR - based boards without a dedicated Serial buffer, we need to add a small delay between Serial reads
// Teensy and ESP boards appear to use a default 12-bit resolution for Analog writes...
#if defined(__AVR__)
 #define IS_AVR 1
 #define SERIAL_DELAY 1
 #define ANALOG_MAX 255
#else
 #define IS_AVR 0
 #define SERIAL_DELAY 5
 #define ANALOG_MAX 4095
#endif

// MarIOnette serial
unsigned int counter = 0;
unsigned int howManyBytes = 0;

// Expected packet
const int expectedMotorBytes = TOTAL_MOTORS * 2;
const int expectedSpeedBytes = 2;

// Servos
#include <Servo.h>
Servo servos[1];
unsigned int busServoSpeed;
unsigned int readingPositions = 0;

// Animations
unsigned long currentFrame;
unsigned int FPS;
unsigned long totalFrames;
unsigned int frameByteLength;
unsigned long frameInterval;
unsigned long animationTimer;
char filename[20];

// Memory Animation
unsigned int isMemoryAnimation = 0;
void*globalArrayPointer;
unsigned char* animationArrayPointer;
unsigned int playingAnimation;

// Auto-generated function from MarIOnette
void setupAll(){

 for(int i = 0; i < TOTAL_MOTORS; i++){
  // Servos
  if(motor_values[i][0] == 1){
   servos[i].attach(motor_values[i][1]);
  }

  // PWM Pin
  if(motor_values[i][0] == 2){
   pinMode(motor_values[i][1], OUTPUT);
  }

  // ON/OFF Pin
  if(motor_values[i][0] == 3){
   pinMode(motor_values[i][1], OUTPUT);
  }

  // PWM Bi-Directional
  if(motor_values[i][0] == 4){
   pinMode(motor_values[i][1], OUTPUT);
   pinMode(motor_values[i][2], OUTPUT);
  }

 }
}

void updateMotorsAndLEDs(char frame_buffer[], int mode){
 int servo_index = 0;
 int stepper_index = 0;

 for(int i = 0; i < TOTAL_MOTORS; i++){
  int offset = i*2;

  // Speed bytes are sent in case we have bus servos...
  if(mode == 0){
   offset = i*2+expectedSpeedBytes;
  }

  unsigned int motor_value = word(frame_buffer[offset], frame_buffer[offset+1]);

  // Min and Max values
  if(motor_value > motor_values[i][9] && motor_values[i][8] != 0 && motor_values[i][9] != 0){
   motor_value = motor_values[i][9];
  }

  else if(motor_value < motor_values[i][8] && motor_values[i][8] != 0 && motor_values[i][9] != 0){
   motor_value = motor_values[i][8];
  }

  // Set servo position
  if(motor_values[i][0] == 1){
   servos[i].writeMicroseconds(motor_value);
   servo_index++;
  }

  // Set PWM value
  if(motor_values[i][0] == 2){
   analogWrite(motor_values[i][1], map(motor_value, 0, 4000, 0, ANALOG_MAX));
  }

  // Set ON/OFF value
  if(motor_values[i][0] == 3){
   if(motor_value > 0){
    digitalWrite(motor_values[i][1], HIGH);
   }

   else{
    digitalWrite(motor_values[i][1], LOW);
   }
  }

  // Set Bi-directional PWM
  if(motor_values[i][0] == 4){
   if(motor_values[i][3] == 1){
    if(motor_value > ANALOG_MAX/2){
     analogWrite(motor_values[i][1], map(motor_value, ANALOG_MAX/2, ANALOG_MAX, 0, ANALOG_MAX));
     digitalWrite(motor_values[i][2], HIGH);
    }

    else{
     analogWrite(motor_values[i][1], map(motor_value, ANALOG_MAX/2, 0, 0, ANALOG_MAX));
     digitalWrite(motor_values[i][2], LOW);
    }
   }

   else{
    if(motor_value > ANALOG_MAX/2){
     analogWrite(motor_values[i][2], map(motor_value, ANALOG_MAX/2, ANALOG_MAX, 0, ANALOG_MAX));
     digitalWrite(motor_values[i][1], LOW);
    }

    else{
     analogWrite(motor_values[i][1], map(motor_value, ANALOG_MAX/2, 0, 0, ANALOG_MAX));
     digitalWrite(motor_values[i][2], LOW);
    }
   }
  }
 }
}

void readAnimationFileMemory(){
 unsigned int* fpsPtr = (unsigned int*)*((void**)globalArrayPointer);
 unsigned long* tfPtr = (unsigned long*)*(((void**)globalArrayPointer) + 1);
 unsigned int* fblPtr = (unsigned int*)*(((void**)globalArrayPointer) + 2);
 totalFrames = *tfPtr;
 FPS = *fpsPtr;
 frameByteLength = *fblPtr;
 frameInterval = 1000*1000/FPS; // In microseconds
 Serial.print("Total frames: ");
 Serial.print(totalFrames);
 Serial.print(" | FPS: ");
 Serial.print(FPS);
 Serial.print(" | Bytes per frame: ");
 Serial.println(frameByteLength);
 playingAnimation = 1;
 currentFrame = 0;
 isMemoryAnimation = 1;
 animationTimer = micros();
}

void playAnimationMemory(){
 if(playingAnimation && isMemoryAnimation){
  if(micros() - animationTimer > frameInterval){
    if(currentFrame == totalFrames){
     playingAnimation = 0;
     isMemoryAnimation = 0;
     Serial.println("Animation done!");
     return;
   }

    char frame_buffer[frameByteLength];
    for(unsigned int i = 0; i < frameByteLength; i++){
     frame_buffer[i] = animationArrayPointer[currentFrame*frameByteLength+i];
    }

    updateMotorsAndLEDs(frame_buffer, 1);
    currentFrame++;
    animationTimer = micros();
  }
 }
}

void playAnimationFile(){}

void SDHelper(int mode){}

void readAnimationFile(){}

void updateSteppers(){}