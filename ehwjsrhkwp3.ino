// Arduino pin assignment
#define PIN_LED  7
#define PIN_TRIG 12
#define PIN_ECHO 13

// configurable parameters
#define SND_VEL 346.0     // sound velocity at 24 celsius degree (unit: m/sec)
#define INTERVAL 25       // sampling interval (unit: msec)
#define PULSE_DURATION 10 // ultra-sound Pulse Duration (unit: usec)
#define _DIST_MIN 100     // minimum distance to be measured (unit: mm)
#define _DIST_MAX 300     // maximum distance to be measured (unit: mm)

#define TIMEOUT ((INTERVAL / 2) * 1000.0) // maximum echo waiting time (unit: usec)
#define SCALE (0.001 * 0.5 * SND_VEL)     // coefficent to convert duration to distance

#define _EMA_ALPHA 0.5    // EMA weight of new sample (range: 0 to 1)
                          // Setting EMA to 1 effectively disables EMA filter.
#define Sample_N 30 // set N var


// global variables
unsigned long last_sampling_time;   // unit: msec
float dist_prev;        // Distance last-measured
float dist_ema;                     // EMA distance
float ema_temp;

float dist_median;

float med_arr[Sample_N];
float sorted_arr[Sample_N];
float copied_arr[Sample_N];
int arrCounter = 0;
float Biggest = 0;
int varForIndex;

void setup() {
  // initialize GPIO pins
  pinMode(PIN_LED,OUTPUT);
  pinMode(PIN_TRIG,OUTPUT);
  pinMode(PIN_ECHO,INPUT);
  digitalWrite(PIN_TRIG, LOW);

  // initialize serial port
  Serial.begin(57600);
}

void loop() {
  float dist_raw, dist_filtered;
  Biggest = 0;
  // wait until next sampling time. 
  // millis() returns the number of milliseconds since the program started. 
  // will overflow after 50 days.
  if (millis() < last_sampling_time + INTERVAL)
    return;

  // get a distance reading from the USS
  dist_raw = USS_measure(PIN_TRIG,PIN_ECHO);
  if(arrCounter < Sample_N){
    med_arr[arrCounter] = dist_raw;
    arrCounter+=1;
  }else if(arrCounter == Sample_N){
    arrCounter = 0;
    med_arr[arrCounter] = dist_raw;
    arrCounter+=1;
  }

  for(int i=0;i<Sample_N;i++){
    copied_arr[i] = med_arr[i];
  }
  
  for(int l = 0;l<Sample_N;l++)
  {
    Biggest = 0;
    for(int i = 0;i<Sample_N;i++){
      if(Biggest < copied_arr[i]){
        Biggest = copied_arr[i];
        varForIndex = i;
      }
    }
    sorted_arr[l] = Biggest;
    copied_arr[varForIndex] = 0;
  }

  dist_median = sorted_arr[round(Sample_N/2)];

  ema_temp = dist_ema;
  dist_ema = (ema_temp*_EMA_ALPHA)+(dist_raw*_EMA_ALPHA);
  
  // output the distance to the serial port
  Serial.print("Min:");   Serial.print(_DIST_MIN);
  Serial.print(",raw:"); Serial.print(dist_raw );
  Serial.print(",ema:");  Serial.print(dist_ema );
  Serial.print(",median:");  Serial.print(dist_median );
  Serial.print(",Max:");  Serial.print(_DIST_MAX);
  Serial.println("");
  
  // do something here
  if ((dist_raw < _DIST_MIN) || (dist_raw > _DIST_MAX))
    digitalWrite(PIN_LED, 1);       // LED OFF
  else
    digitalWrite(PIN_LED, 0);       // LED ON

  // update last sampling time
  last_sampling_time += INTERVAL;
}

// get a distance reading from USS. return value is in millimeter.
float USS_measure(int TRIG, int ECHO)
{
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(PULSE_DURATION);
  digitalWrite(TRIG, LOW);
  
  return pulseIn(ECHO, HIGH, TIMEOUT) * SCALE; // unit: mm

  // Pulse duration to distance conversion example (target distance = 17.3m)
  // - pulseIn(ECHO, HIGH, timeout) returns microseconds (음파의 왕복 시간)
  // - 편도 거리 = (pulseIn() / 1,000,000) * SND_VEL / 2 (미터 단위)
  //   mm 단위로 하려면 * 1,000이 필요 ==>  SCALE = 0.001 * 0.5 * SND_VEL
  //
  // - 예, pusseIn()이 100,000 이면 (= 0.1초, 왕복 거리 34.6m)
  //        = 100,000 micro*sec * 0.001 milli/micro * 0.5 * 346 meter/sec
  //        = 100,000 * 0.001 * 0.5 * 346
  //        = 17,300 mm  ==> 17.3m
}
