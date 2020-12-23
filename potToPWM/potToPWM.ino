
long tachCount=0;
void tachISR(){
  tachCount+=1;
}


// disable timer output on OC1B
void oc1bDis(){
  TCCR1A &= ~(_BV(COM1B1)|_BV(COM1B0));
}

// enable output on OC1B
void oc1bEn(){
  oc1bDis();
  //Compare output mode: non-inverting output on OC1B: COM1B=0b10
  TCCR1A |= _BV(COM1B1);
}

bool isOc1bEn(){
  return (TCCR1A & _BV(COM1B1)) || (TCCR1A & _BV(COM1B0));
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
  int potval=analogRead(A0);
  OCR1B=map(potval<50?0:(potval>1014?1023:potval),0,1023,0,639);
  if(OCR1B==0 && isOc1bEn()){
    // timer glitches high for a moment when OCR1B is 0
    digitalWrite(10,LOW);// OC1B is pin 10
    // digitalWrite automatically disables fast PWM (clears COM1B1)
    // https://garretlab.web.fc2.com/en/arduino/inside/hardware/arduino/avr/cores/arduino/wiring_digital.c/digitalWrite.html
    // https://garretlab.web.fc2.com/en/arduino/inside/hardware/arduino/avr/cores/arduino/wiring_digital.c/turnOffPWM.html
  }
  else if(OCR1B!=0 && !isOc1bEn()){
    oc1bEn();
  }
  if(millis()-lastTachTime>=1000){
    // for tachometer, 1k pull up and 100nf capacitor 
    // seems to clean up the signal enough for this to work
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
