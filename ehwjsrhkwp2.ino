int duration;
int rates;
int repeater;
int led = 7;
int dutyCount=0;
int DutyPerDuration;

void setup() {
  
  pinMode(led,OUTPUT);
  
}

void loop() {
  
  set_period(100); 
  
  if(duration >5000){
    
    dutyCount = 0;
    DutyPerDuration = 100/(500000/duration);
    
    while(dutyCount <=100){
      set_duty(dutyCount);
      
      digitalWrite(led, false);
      delayMicroseconds(rates*(duration/100));
      
      digitalWrite(led, true);
      delayMicroseconds((100-rates)*(duration/100));
      
      dutyCount+=DutyPerDuration;
    }
    
    dutyCount = 100;
    
    do{
      set_duty(dutyCount);
      
      digitalWrite(led, false);
      delayMicroseconds(rates*(duration/100));
      
      digitalWrite(led, true);
      delayMicroseconds((100-rates)*(duration/100));
      
      dutyCount-=DutyPerDuration;
    } while(dutyCount >0);
    
  }
  
  else if(duration <= 5000){
    
    dutyCount = 0;
    repeater = 5000/duration ;
    
    while(dutyCount <=100)
    {
      set_duty(dutyCount);
      for(int i=0;i<repeater;i++){
        digitalWrite(led, false);
        delayMicroseconds(rates*(duration/100));

        digitalWrite(led, true);
        delayMicroseconds((100-rates)*(duration/100));
      }
      dutyCount+=1;
    }
    
    dutyCount = 100;
    while(dutyCount>0)
    {
      set_duty(dutyCount);
      for(int i=0;i<repeater;i++){
        digitalWrite(led, false);
        delayMicroseconds(rates*(duration/100));

        digitalWrite(led, true);
        delayMicroseconds((100-rates)*(duration/100));
      }
      dutyCount-=1;
    }
  }
  
}

void set_period(int period)
{
  duration = period;
}

void set_duty(int duty)
{
  rates= duty;
}
