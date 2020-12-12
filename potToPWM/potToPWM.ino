
long tachCount=0;
void tachISR(){
  tachCount+=1;
}

unsigned long lastTachTime;
void setup() {
  // put your setup code here, to run once:
  pinMode(10,OUTPUT);//OC1B
  //frequency no prescale
  //max counter 640
  //Waveform Generation Mode: fast PWM, TOP=ICR1: WGM1=0b1110
  TCCR1A = _BV(WGM11);
  TCCR1B = _BV(WGM12) | _BV(WGM13);
  //Compare output mode: non-inverting output on OC1B: COM1B=0b10
  TCCR1A |= _BV(COM1B1);
  //clock source: system clock, no prescale: CS1=0b001
  TCCR1B |= _BV(CS10);
  //set counter max
  ICR1=639;

  Serial.begin(9600);
  
  //tachometer
  pinMode(3,INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(3),tachISR,RISING);

  lastTachTime=millis();
}


void loop() {
  // put your main code here, to run repeatedly:
  OCR1B=map(analogRead(A0),0,1024,0,639);
  if(millis()-lastTachTime>=1000){
    noInterrupts();
    unsigned long timePassed=millis()-lastTachTime;
    unsigned long rpm = 60000*tachCount/timePassed/2;
    Serial.print(timePassed);
    Serial.print(" ");
    Serial.println(rpm);
    tachCount=0;
    lastTachTime=millis();
    interrupts();
  }
  delay(5);
  
}
