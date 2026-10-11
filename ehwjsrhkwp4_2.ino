#include <Servo.h>

// Arduino pin assignment
#define PIN_LED   7   // LED active-low
#define PIN_TRIG  12  // sonar sensor TRIGGER
#define PIN_ECHO  13  // sonar sensor ECHO
#define PIN_SERVO 10  // servo motor

// configurable parameters for sonar
#define SND_VEL 346.0     // sound velocity at 24 celsius degree (unit: m/sec)
#define INTERVAL 25      // sampling interval (unit: msec)
#define PULSE_DURATION 10 // ultra-sound Pulse Duration (unit: usec)
#define _DIST_MIN 180.0   // minimum distance to be measured (unit: mm)
#define _DIST_MAX 360.0   // maximum distance to be measured (unit: mm)

#define TIMEOUT ((INTERVAL / 2) * 1000.0) // maximum echo waiting time (unit: usec)
#define SCALE (0.001 * 0.5 * SND_VEL) // coefficent to convert duration to distance

#define _EMA_ALPHA 0.3    

// Target Distance
#define _TARGET_LOW  180.0
#define _TARGET_HIGH 360.0
 
#define _DUTY_MIN 500   // servo full clockwise position (0 degree)
#define _DUTY_NEU 1500  // servo neutral position (90 degree)
#define _DUTY_MAX 2500  // servo full counterclockwise position (180 degree)

// global variables
float dist_ema, dist_prev = _DIST_MAX; 
unsigned long last_sampling_time;       

Servo myservo;
bool opened = false;

void setup() {
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_TRIG, OUTPUT);    
  pinMode(PIN_ECHO, INPUT);     
  digitalWrite(PIN_TRIG, 0);  

  myservo.attach(PIN_SERVO); 
  myservo.write(0); 
  Serial.begin(57600);
}

void loop() {
  float dist_raw, dist_filtered;
  
  if (millis() < last_sampling_time + INTERVAL)
    return;
  
  dist_raw = USS_measure(PIN_TRIG, PIN_ECHO);
  
  if ((dist_raw == 0.0) || (dist_raw > _DIST_MAX)) {
      dist_filtered = dist_prev;
      digitalWrite(PIN_LED, true);
  } else if (dist_raw < _DIST_MIN) {
      dist_filtered = dist_prev;
      digitalWrite(PIN_LED, true);
  } else {    
      dist_filtered = dist_raw;
      dist_prev = dist_raw;
      digitalWrite(PIN_LED, false);
  }

  dist_ema = (dist_filtered * _EMA_ALPHA) + (dist_ema * (1.0 - _EMA_ALPHA));
  
  
  if(dist_ema <= 200.0 && opened == false){
    sigmoid(true);
    opened = true;
  }
  else if(dist_ema > 200.0 && opened == true){
    sigmoid(false);
    opened = false;
  }
  
  
  Serial.print("Min:");    Serial.print(_DIST_MIN);
  Serial.print(",Low:");   Serial.print(_TARGET_LOW);
  Serial.print(",dist:");  Serial.print(dist_ema);
  Serial.print(",Servo:"); Serial.print(myservo.read());  
  Serial.print(",High:");  Serial.print(_TARGET_HIGH);
  Serial.print(",Max:");   Serial.print(_DIST_MAX);
  Serial.println("");
 
  last_sampling_time += INTERVAL;
}

float USS_measure(int TRIG, int ECHO) {
  digitalWrite(TRIG, 1);
  delayMicroseconds(PULSE_DURATION);
  digitalWrite(TRIG, 0);
  return pulseIn(ECHO, 1, TIMEOUT) * SCALE; 
}


void sigmoid(bool go){
  if(go){
    
    for(int progress = 0; progress <= 100; progress++) {
      
      float sigX = map(progress, 0, 100, -400, 400) / 100.0; 
      float sigval = 1.0 / (1.0 + exp(-1.0 * sigX));
      int angle = sigval * 90.0; 
      myservo.write(angle);
      delay(15); 
    }
  }
  else {
    
    for(int progress = 100; progress >= 0; progress--) {
      float sigX = map(progress, 0, 100, -400, 400) / 100.0;
      float sigval = 1.0 / (1.0 + exp(-1.0 * sigX));
      int angle = sigval * 90.0;
      myservo.write(angle);
      delay(15);
    }
  }
}

void cosfunc(bool go){
  if(go){
    
    for(int progress = 0; progress <= 100; progress++) {
      float rad = (progress / 100.0) * 3.14;
      float ratio = (1.0 - cos(rad)) / 2.0;
      int angle = ratio * 90.0;
      
      myservo.write(angle);
      delay(15); 
    }
  }
  else {
    
    for(int progress = 100; progress >= 0; progress--) {
      float rad = (progress / 100.0) * 3.14;
      float ratio = (1.0 - cos(rad)) / 2.0;
      int angle = ratio * 90.0;
      myservo.write(angle);
      delay(15); 
    }
  }
}
