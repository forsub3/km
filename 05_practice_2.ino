int i = 0;
void setup() {
  // put your setup code here, to run once:
  pinMode(7,OUTPUT);
  i = 0;
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(7,true);
  delay(1000);
  
  while(i<5){
  digitalWrite(7,false);
  delay(100);
  digitalWrite(7,true);
  delay(100);
  i++;
  }
  while(1){}
  
  
}
