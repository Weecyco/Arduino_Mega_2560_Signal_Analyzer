#include <fix_fft.h>

#define SAMPLE_RATE 38461.53846
#define RESULT_LIMIT 750
#define RESULT_WARNING 370
#define RECORD_LIMIT 10
//Simple Frequency Analysis Program 2.1

//Audio in with 38.5kHz sampling rate, interrupts, and clipping indicator
//by Amanda Ghassaei
//https://www.instructables.com/id/Arduino-Audio-Input/
//Sept 2012

/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
*/
struct Result
{
  unsigned int freq[2] = {0, 0};
  byte mag[2] = {0, 0};
  unsigned int time_cs = 0;
};

class Signals
{
public:
  bool clipping; //true if clipping
  unsigned long anlyStartms; //records the analysis start time in ms
  
  unsigned int recLen; //holds the signal front
  unsigned int len; //length of the signal
  byte p2Len; //holds the length of the signal as a power of 2
  byte* sig; //holds the audio signal
  int8_t* FT; //holds the fourier transform

//parameters:
  byte refLvl;
  byte freqCount;
  float approxExp;
  
//  bool abrt; //returns to setup
  bool continuous; //holds whether or not the sending of data is continous or by prompt
//  bool sendOnVar; //sends when signal varies significantly (ignored if continous is true)
  byte detail; //whether or not detailed information is displayed
  bool estimate; //whether or not to estimate the value based on its surounding frequencies

//result parameters
  Result* Results;
  unsigned int resultLen;
  unsigned int resultCount;
  
  Signals(); //constructor

  //prints parameters
  void printParam();
  //prints arrays
  void printSig(bool);
  void printFT(bool);
  void printResult();
  
  //clears results
  void clrResults();
  
  //records a single value to sig
  void rec(byte& input);

  void startLoop();

  bool globalCommands(const String&, bool);
  
private:
  void anlyLoop();
  void analyze();
  void analyzeFT();
  void findFTMax(int FTmax[]);
};



Signals Signal;

void setup() {
  pinMode(13,OUTPUT);//led indicator pin
  pinMode(2,INPUT);
  
  cli();//disable interrupts
  
  //set up continuous sampling of analog pin 0
  
  //clear ADCSRA and ADCSRB registers
  ADCSRA = 0;
  ADCSRB = 0;
  
  ADMUX |= (1 << REFS0); //set reference voltage
  ADMUX |= (1 << ADLAR); //left align the ADC value- so we can read highest 8 bits from ADCH register only
  
  ADCSRA |= (1 << ADPS2) | (1 << ADPS0); //set ADC clock with 32 prescaler- 16mHz/32=500kHz
  ADCSRA |= (1 << ADATE); //enabble auto trigger
  //ADCSRA |= (1 << ADIE); //enable interrupts when measurement complete
  ADCSRA &= ~(1 << ADIE); //disables interrupts while in setup
  ADCSRA |= (1 << ADEN); //enable ADC
  ADCSRA |= (1 << ADSC); //start ADC measurements
  
  sei();//enable interrupts

  //if you want to add other things to setup(), do it here

  Signal.recLen = Signal.len;

//  recordTimer = DEFAULT_SNIP_SIZE;
//  recordLength = DEFAULT_SNIP_SIZE;
//  state = Start;
  
  Serial.begin(9600);


  
  //startup message
  Serial.println("Signal Anly. tool, Ver 2.1");
  Serial.print(" >> ");

  //allocates the default size for results
  Signal.Results = new Result[Signal.resultLen];
}

void loop() {

  //Setup
  if(Serial.available() > 0)
  {
    //reads input
    String in = Serial.readString();
    in.remove(in.length() - 1);
    in.toUpperCase();
    Serial.println(in);

    //parses input
    
    //Handles commands
    if(in[0] == 'S')
    {
      if(in.length() > 1)
        switchAnalog((in.substring(1)).toInt());
      Serial.print("Analyzing: A");
      Serial.println((ADMUX & B111) | (((ADCSRB & (1<<MUX5))>>MUX5)<<3));

      Serial.println("With Parameters:");
      Signal.printParam();
      Signal.startLoop();
    }
    else if(in == "help")
    {
      Serial.println("See manual");
    }
    else if(in[0] == 'L')//sets and shows recording length
    {
      if(in.length() > 1)
      {
        Signal.p2Len = (in.substring(1)).toInt();
        if(Signal.p2Len < 1)
          Signal.p2Len = 1;
        else if(Signal.p2Len > RECORD_LIMIT)
          Signal.p2Len = RECORD_LIMIT;
          
        Signal.recLen = round(pow(2, Signal.p2Len));
        Signal.len = Signal.recLen;
      }
      Serial.print("Segment Length: ");
      Serial.println(Signal.len);
    }
    else if(in[0] == 'R' && in.length() > 1 && in[1] == 'L')//sets and shows result count length
    {
      if(in.length() > 2)
      {
        if(Signal.Results != NULL)
          delete [] Signal.Results;
        Signal.resultLen = (in.substring(2)).toInt();
        if(Signal.resultLen > RESULT_LIMIT)
          Signal.resultLen = RESULT_LIMIT;
        if(Signal.resultLen > RESULT_WARNING)
          Serial.println("Warning: High result count may crash the program at high record lengths");
        else if(Signal.resultLen < 1)
          Signal.resultLen = 1;

        if(Signal.resultLen == 0)
          Signal.Results = NULL;
        else
          Signal.Results = new Result[Signal.resultLen];
      }
      Serial.print("Result Length: ");
      Serial.println(Signal.resultLen);
    }
    else if(!Signal.globalCommands(in, false))
    {
      Serial.println("Error: cmd/arg not recog.");
    }
    Serial.print(" >> ");
  }
}


void switchAnalog(byte val)
{
  //ensures valid input
  if(val > 15 || val < 0)
  {
    Serial.println("E:0<=val<=15");
    return;
  }
  //clears register (ADMUX[3:0] and MUX5 in ADCSRB)
  ADMUX &= ~B111;
  ADCSRB &= ~(1<<MUX5);
  //sets val
  ADMUX |= (val & B0111);
  ADCSRB |= (((val & B1000)>>3)<<MUX5);
}
