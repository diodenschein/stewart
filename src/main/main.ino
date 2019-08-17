/*
   6dof-stewduino
   Copyright (C) 2018  Philippe Desrosiers

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

//=== Includes
#include <Arduino.h>

#include "config.h"
#include "Platform.h"
//#include "Logger.h"
  #include <Wire.h>

  #include <Adafruit_PWMServoDriver.h>

#ifdef ENABLE_NUNCHUCK
#include "nunchuck.h"

#endif

#ifdef ENABLE_SERIAL_COMMANDS
#include "commands.h"
#endif

#ifdef ENABLE_TOUCHSCREEN
#include "touch.h"
#endif

//=== Macros
#ifdef CORE_TEENSY
//Software reset macros / MMap FOR TEENSY ONLY
#define CPU_RESTART_VAL 0x5FA0004                           // write this magic number...
#define CPU_RESTART_ADDR (uint32_t *)0xE000ED0C             // to this memory location...
#define CPU_RESTART (*CPU_RESTART_ADDR = CPU_RESTART_VAL)   // presto!

// #else
//
// #define CPU_RESTART asm volatile ("  jmp 0")                // close enough for arduino

#endif

// this is the magic trick for printf to support float
asm(".global _printf_float");
// this is the magic trick for scanf to support float
asm(".global _scanf_float");

//=== Actual code


#define  DO_LOGGING


int numPoints = 0;
boolean pathWorking = true;

boolean motionWorking = false;

float max_servo_speed = 0.0;
float sum_servo_speed = 0.0;
float num_servo_speed = 1.0;

void moveTo( float to[], boolean s);
boolean addPoint(float to[], int y );
void setSpeed(float ratio);

#define GESTURE_SELFTEST 0
#define GESTURE_PARK 1
#define GESTURE_WAVE 2
#define GESTURE_OI 3
#define GESTURE_HELLO 4
#define GESTURE_LEAVING 5
#define GESTURE_BORED 6
#define GESTURE_INTERESTED 7
#define GESTURE_LOOKING_ROUND 8
#define GESTURE_REVOLVE_ALL_LEFT 22
#define GESTURE_REVOLVE_ALL_RIGHT 23
#define GESTURE_STAY_BORED 24


#define GESTURE_LAST GESTURE_LOOKING_ROUND

int delayMotion = 0;

int currentGesture = -1;

long gestureStartMillis = 0;
long eventStartMillis = 0;

xy_coordf setpoint = DEFAULT_SETPOINT;

Platform stu;            // Stewart platform object.

   Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
    int servos[6];
#ifdef ENABLE_SERVOS
 
    Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
    int servos[6];

#endif

float sp_servo[6];      // servo setpoints in degrees, between SERVO_MIN_ANGLE and SERVO_MAX_ANGLE.
// // Logger* // Logger = // Logger::instance();

float _toUs(int value) {
  return map(value, SERVO_MIN_ANGLE, SERVO_MAX_ANGLE, SERVO_MIN_US, SERVO_MAX_US);
}

float _toAngle(float value) {
  return map(value, SERVO_MIN_US, SERVO_MAX_US, SERVO_MIN_ANGLE, SERVO_MAX_ANGLE);
}

//Set servo values to the angles represented by the setpoints in sp_servo[].
//DOES: Apply trim values.
//DOES: Automatically reverse signal for reversed servos.
//DOES: Write signals to the physical servos.
void updateServos() {
  static float sValues[6];

  for (int i = 0; i < 6; i++) {
    //sp_servo holds a value between SERVO_MIN_ANGLE and SERVO_MAX_ANGLE.
    //apply reverse.
    float val = sp_servo[i];
    if (SERVO_REVERSE[i]) {
      val = SERVO_MIN_ANGLE + (SERVO_MAX_ANGLE - val);
    }

    //translate angle to pulse width
    val = _toUs(val);

    if (val != sValues[i]) {
      //don't write to the servo if you don't have to.
      sValues[i] = val;
    //  // Logger::trace("SRV: s%d = %.2f + %d (value + trim)", i, val, SERVO_TRIM[i]);

//#ifdef ENABLE_SERVOS
            pwm.setPWM(i+4, 0, (int)constrain(val + SERVO_TRIM[i], SERVO_MIN_US, SERVO_MAX_US));

     //   servos[i].writeMicroseconds((int)constrain(val + SERVO_TRIM[i], SERVO_MIN_US, SERVO_MAX_US));
     // #endif
//#endif
    }
  }
}

// Calculates and assigns values to sp_servo.
// DOES: Ignore out-of-range values. These will generate a warning on the serial monitor.
// DOES NOT: Apply servo trim values.
// DOES NOT: Automatically reverse signal for reversed servos.
// DOES NOT: digitally write a signal to any servo. Writing is done in updateServos();
void setServo(int i, int angle) {
  int val = angle;
  if (val >= SERVO_MIN_ANGLE && val <= SERVO_MAX_ANGLE) {
    sp_servo[i] = val;
//    // Logger::trace("setServo %d - %.2f degrees", i, sp_servo[i]);
  } else {
//    // Logger::warn("setServo: Invalid value '%.2f' specified for servo #%d. Valid range is %d to %d degrees.", val, i, SERVO_MIN_ANGLE, SERVO_MAX_ANGLE);
  }
}

void setServoMicros(int i, int micros) {
  int val = micros;
  if (val >= SERVO_MIN_US && val <= SERVO_MAX_US) {
    sp_servo[i] = _toAngle(val);
//    // Logger::trace("setServoMicros %d - %.2f us", i, val);
  } else {
 //   // Logger::warn("setServoMicros: Invalid value '%.2f' specified for servo #%d. Valid range is %d to %d us.", val, i, SERVO_MIN_US, SERVO_MAX_US);
  }
}

void setupTouchscreen() {
  #ifdef ENABLE_TOUCHSCREEN
 // // Logger::debug("Touchscreen ENABLED.");
  rollPID.SetSampleTime(ROLL_PID_SAMPLE_TIME);
  pitchPID.SetSampleTime(PITCH_PID_SAMPLE_TIME);
  rollPID.SetOutputLimits(ROLL_PID_LIMIT_MIN, ROLL_PID_LIMIT_MAX);
  pitchPID.SetOutputLimits(PITCH_PID_LIMIT_MIN, PITCH_PID_LIMIT_MAX);
  rollPID.SetMode(AUTOMATIC);
  pitchPID.SetMode(AUTOMATIC);
  #else
//  // Logger::debug("Touchscreen DISABLED.");
  #endif
}

void setupPlatform() {
  stu.home(sp_servo); // set platform to "home" position (flat, centered).
     Wire.begin();
  pwm.begin();
  pwm.setPWMFreq(60);  // Analog servos run at ~60 Hz updates
  
  updateServos();
  delay(300);
}

//Initialize servo interface, sweep all six servos from MIN to MAX, to MID, to ensure they're all physically working.
void setupServos() {
  int d=2000;


  for (int i = 0; i < 6; i++) {
#ifdef ENABLE_SERVOS
   // servos[i].attach(SERVO_PINS[i]);
#endif
    setServo(i, SERVO_MIN_ANGLE);
  }
  updateServos();
  delay(d);

  for(int pos=SERVO_MIN_ANGLE;pos<SERVO_MID_ANGLE;pos+=4){
    for (int i = 0; i < 6; i++) {
      setServo(i,pos);
    }
    updateServos();
    delay(10);
  }

   for (int i = 0; i < 6; i++) {
     setServo(i, SERVO_MAX_ANGLE);
   }
   updateServos();
   delay(d);
  
   for (int i = 0; i < 6; i++) {
     setServo(i, SERVO_MID_ANGLE);
   }
   updateServos();
   delay(d);
}

/**
Setup serial port and add commands.
*/
void setupCommandLine(int bps=115200) {
  Serial.begin(bps);
  delay(50);

  // Logger::info("Studuino, v1");
  // Logger::info("Built %s, %s",__DATE__, __TIME__);
  // Logger::info("=======================");

  #ifdef ENABLE_SERIAL_COMMANDS
  // Logger::debug("Command-line is ENABLED.");

  shell_init(shell_reader, shell_writer, 0);

  const int c1 = sizeof(commands);
  if(c1 > 0) {
    const int c2 = sizeof(commands[0]);
    const int ccount = c1 / c2;

    for (int i = 0; i < ccount; i++) {
      // Logger::debug("Registering command: %s",commands[i].shell_command_string);
      shell_register(commands[i].shell_program, commands[i].shell_command_string);
    }
  }
  #else

  // Logger::debug("Command-line is DISABLED.");

  #endif
  delay(100);
}

void setupNunchuck() {
  #ifdef ENABLE_NUNCHUCK
  // Logger::debug("Nunchuck support is ENABLED.");

  nc.begin();
  #else
  // Logger::debug("Nunchuck support is DISABLED.");
  #endif
}

void setup() {
  // Logger::level = LOG_LEVEL;    //config.h

  pinMode(LED_BUILTIN, OUTPUT); //power indicator
  digitalWrite(LED_BUILTIN, HIGH);

  setupCommandLine(115200);

  setupNunchuck();

  setupPlatform();

  setupTouchscreen();

  setupPosition();
  setupMotion();


   
  setupServos();  //Servos come last, because this setup takes the most time...
    Serial.begin(115200);
  startGesture(GESTURE_SELFTEST);
 
}


void loopGestures() 
{ 

  int newGesture = checkForEvents(); // change this function so it delivers a gesture number when the person on the call want to perform a gesture
  
  boolean done = ! loopPath();

   if( newGesture != -1 )
   {
      startGesture(newGesture);  // something kicked off a new gesture, we'll just do it once then go back to repeating GESTURE_SOMEBODY_HOME
      delayMotion = 0;
   }
   else if( done )
   { 

      // finished a thing, go to a neutral gesture
      Serial.println("parking");
      startGesture(GESTURE_PARK); 
      delayMotion = (int)random(10);       
     
   }
   // else just keep running the current motion
      
   loopMotion();
   
}


int eventGesture = GESTURE_WAVE;
int command;

int checkForEvents()
{
  // see if something happened that mean we ought to start a gesture, for now just use a timer

   while(Serial.available() > 0)
    {
        command = Serial.parseInt();
        Serial.println(command);
        if (Serial.read() == '\n') { //note need to set "newline" on console
          Serial.print("gesture found  ");
          Serial.println(command);
          eventGesture = command;
          return eventGesture;
        }else{
          Serial.println("no newline found");
        }
    }

/*
  long now = millis();

  if( now - eventStartMillis > 10000 )  // run through the gesture, one every 10 secs
  {
    eventStartMillis = now;

    eventGesture = eventGesture + 1;
    
    if( eventGesture > GESTURE_LAST )
      eventGesture = GESTURE_WANT_TO_SPEAK;
    
    
       #ifdef DO_LOGGING
         Serial.print ("next gesture ");
         Serial.println (eventGesture);
      #endif
      return eventGesture;
    

    
  }
*/
  return -1;
}



void loop() {

#ifdef ENABLE_SERIAL_COMMANDS
  processCommands();  //process any incoming serial commands.
#endif

#ifdef ENABLE_NUNCHUCK
  processNunchuck();
//  blinker.loop();
#endif

#ifdef ENABLE_TOUCHSCREEN
  processTouchscreen();
#endif

  loopGestures();
  
  updateServos();   //Servos come last, because they take the most time.
 // handleDemo(0,0);
}
