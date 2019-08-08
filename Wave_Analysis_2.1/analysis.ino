void Signals::startLoop()
{
  
  //allocates FT
  sig = (byte*)malloc(len * sizeof(byte));
  FT = (int8_t*)malloc(len * sizeof(int8_t));
  
  //clears signals
  for(int i = 0; i < len; i++)
  {
    sig[i] = 0;
    FT[i] = 0;
  }
  anlyStartms = millis();
  ADCSRA |= (1 << ADIE); //enable interrupts when measurement complete
  anlyLoop();
  ADCSRA &= ~(1 << ADIE); //disables interrupts while in setup

  free(sig);
  free(FT);
}


void Signals::anlyLoop()
{
  Serial.print("Analysis Loop >> ");
  
  bool ended = false;
  bool analyzed = true;
  bool trigger;
  while(!ended)
  {
    trigger = false;
    if (Signal.clipping){//if currently clipping
      Signal.clipping = 0;//
      digitalWrite(13,LOW);//turn off clipping led indicator (pin 13)
    }
    
    if(Serial.available() > 0)
    {
      //reads input
      String in = Serial.readString();
      in.remove(in.length() - 1);
      in.toUpperCase();
      Serial.println(in);
  
      //parses input
      
      //Handles commands
      if(in == "E")//ends analysis
      {
        ended = true;
      }
      else if(in == "PSIG")//prints current Signal
      {
        printSig(true);
      }
      else if(in == "PFT")//prints current FT
      {
        printFT(true);
      }
      else if(in == "PSIGO")//prints current Signal w/o guides
      {
        printSig(false);
      }
      else if(in == "PFTO")//prints current FT w/o guides
      {
        printFT(false);
      }
      else if(in == "T")//triggers analysis
      {
        trigger = true;
      }
      else if(in == "RT")//resets time
      {
        anlyStartms = millis();
        Serial.println("Time Reset");
      }
      else if(in == "RC")//resets time and toggles continuous
      {
        anlyStartms = millis();
        continuous = !continuous;
        Serial.print("Countinuous: ");
        Serial.println(continuous);
        Serial.println("Time Reset");
      }
      else if(!globalCommands(in, trigger))
      {
        Serial.println("Error: cmd/arg not recog.");
      }
      
      if(!ended)
        Serial.print("Analysis Loop >> ");
    }

    //Auto trigger detecting
    if((!trigger && (continuous || digitalRead(2))))
        trigger = true;

    //handles analysis at the end of a recording segment
    if(recLen == len)
    {
      //analyzes at the end of a recording
      if(!analyzed)
      {
        
        analyzed = true;
        analyze();
      }

      //triggers a recording
      if(trigger == true && analyzed == true)
      {
        analyzed = false;
        recLen = 0; //starts recording
        Results[resultCount].time_cs = (millis() - anlyStartms)/10; //record the time at the start of the recording
        trigger = false;
      }
    }
  }
}


void Signals::analyze()
{
  int8_t* FTi = (byte*)malloc(sizeof(int8_t*) * len);//move to an earlier point to speed up but costs more memery
  for(int i = 0; i < len; ++i)
  {
    FT[i] = sig[i] - refLvl;
    FTi[i] = 0;
  }

  fix_fft(FT, FTi, p2Len, 0);

  //finds magnitude and inputs it into FT
  for(int i = 0; i < len/2; ++i)
  {
    FT[i] = sqrt(pow(FT[i], 2) + pow(FTi[i], 2));
  }

  free(FTi);

  analyzeFT();
}

void Signals::analyzeFT()
{

  //finds the most prominant frequencies
  int *FTmax = new int[freqCount];
  for(int i = 0; i < freqCount; i++)
    FTmax[i] = -1;  

  findFTMax(FTmax);

  //Indexes through the most prominant frequencies and extracts information
  double maxRef = 0;//will store the reference fraction of the most prominant frequency
  for(int i = 0; i < freqCount; i++)
  {
    if(FTmax[i] >= 0)
    {
      double ref = 0;
      int baseFreq = SAMPLE_RATE/len * FTmax[i];
      Results[resultCount].mag[i] = FT[FTmax[i]];
      
      if(estimate)
      {
        int diff = FT[FTmax[i] + 1] - FT[FTmax[i] - 1];
        if(diff < 0)
        {
          ref = diff/(double)(FT[FTmax[i]] - FT[FTmax[i] + 1]);
          Results[resultCount].freq[i] = baseFreq - SAMPLE_RATE/len/2 * pow(abs(ref), approxExp);
        }
        else
        {
          ref = diff/(double)(FT[FTmax[i]] - FT[FTmax[i] - 1]);
          Results[resultCount].freq[i] = baseFreq + SAMPLE_RATE/len/2 * pow(ref, approxExp);
        }
      }
      else
        Results[resultCount].freq[i] = baseFreq;

      //displays detailed information
      if(detail > 1)
      {
        Serial.print(baseFreq);
        Serial.print("Hz (");
        Serial.print(FT[FTmax[i]]);
        Serial.println(")");
        Serial.print("Refined estimate:");
        Serial.print(Results[resultCount].freq[i]);
        Serial.print("Hz ~");
        Serial.print(ref * 100);
        Serial.print("%(preci: ");
        Serial.print((FT[FTmax[i]] - 0.5 * (FT[FTmax[i] + 1] + FT[FTmax[i] - 1])) / (double)FT[FTmax[i]]);
        Serial.println(")\n");
      }
      else if(detail == 1)
      {
        Serial.print(Results[resultCount].freq[i]);
        Serial.println("Hz");
      }
      
      if(i == 0)
        maxRef = ref;
    }
    else if(i == 0)
    {
      Serial.println("NO FREQ'S DETECTED");
    }
  }
  

//  //performs calibration if an expected frequency was given (not -1)
//  if(expectedFreq > 0 && FTmax[0] > 0)
//  {
//    Serial.print("Calibration approxExp = ");
//    if(abs(expectedFreq - SAMPLE_RATE/len * FTmax[0]) < SAMPLE_RATE/len/2)
//    {
//      approxExp = log(abs(expectedFreq - SAMPLE_RATE/len * FTmax[0])*2*len/SAMPLE_RATE)/log(abs(maxRef));
//      Serial.println(approxExp);
//    }
//    else
//      Serial.println("Invalid freq");
//  }
    
  
  delete [] FTmax;
  resultCount = (resultCount + 1)%resultLen;
}

void Signals::findFTMax(int FTmax[])
{
  for(int i = 0; i < len/2; i++)
  {
    if(i != 0)
    {
      byte curMaxInd = 0;
      if(i != 0 && FT[i - 1] <= FT[i] && i != len/2 - 1 && FT[i + 1] < FT[i])
        {
        while(FTmax[curMaxInd] >= 0 && FT[i] <= FT[FTmax[curMaxInd]] && curMaxInd < freqCount)
          curMaxInd++;
        if(curMaxInd != freqCount)
        {
          shift(FTmax + curMaxInd, curMaxInd, freqCount);
          FTmax[curMaxInd] = i;
        }
      }
    }
  }
}


void shift(int point[], byte n, byte& total)
{
  if(n == total - 1)
    return;

  shift(point + 1, n + 1, total);
  point[1] = point[0];
}
