ISR(ADC_vect) {//when new ADC value ready
  byte incomingAudio = ADCH;//store 8 bit value from analog pin 0
  if (incomingAudio == 0 || incomingAudio == 255){//if clipping
    digitalWrite(13,HIGH);//set pin 13 high
    Signal.clipping = 1;//currently clipping
  }

  Signal.rec(incomingAudio);
}
