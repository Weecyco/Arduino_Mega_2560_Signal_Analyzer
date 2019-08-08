Signals::Signals()
{
  clipping = false;
  anlyStartms = 0;

  recLen = 0;
  len = 1024;
  p2Len = 10;
  sig = NULL;
  FT = NULL;

  refLvl = 127;
  freqCount = 2;
  approxExp = 0.53;

//  abrt = false;
  continuous = false;
//  sendOnVar = false;
  detail = 1;
  estimate = true;

  Results = NULL;
  resultLen = 150;
  resultCount = 0;
}

/*
 * Prints parameters
 */
void Signals::printParam()
{
  Serial.print("Sample Count: ");
  Serial.println(len);
  Serial.print("Reference Lvl: ");
  Serial.println(refLvl);
  Serial.print("Estimate?: ");
  Serial.println(estimate);
  Serial.print("Result Length: ");
  Serial.println(resultLen);
  
}

/*
 * Prints analog signal
 */
void Signals::printSig(bool timeDisplay)
{
//  if(checkSize())
  if(len == 0 || sig == NULL)
  {
    Serial.println("No Data");
    return;
  }
  for(int i = 0; i < len; i++)
  {
    if(timeDisplay)
    {
      Serial.print(i/SAMPLE_RATE*1000);
      Serial.print("ms: ");
    }
    Serial.println(sig[i]);
  }
}

/*
 * Prints Fourier transform
 */
void Signals::printFT(bool freqDisplay)
{
  if(len == 0 || FT == NULL)
  {
    Serial.println("No Data");
    return;
  }
  for(int i = 0; i < len/2; i++)
  {
    if(freqDisplay)
    {
      Serial.print(SAMPLE_RATE/len * i);
      Serial.print("Hz: ");
    }
    Serial.println(FT[i]);
  }
//  for(int i = 0; i < len/2; i++)
//  {
//    if(freqDisplay)
//    {
//      Serial.print(SAMPLE_RATE/len * i);
//      Serial.print("Hz: ");
//    }
//    Serial.println(FT[i+len/2]);
//  }
}

/*
 * Prints Results
 */
void Signals::printResult()
{
  for(int resCnt = 0; resCnt < resultLen; ++resCnt)
  {
    if(Results[(resCnt+resultCount+1)%resultLen].time_cs != 0)
    {
      Serial.print("At: ");
      Serial.print(Results[(resCnt+resultCount)%resultLen].time_cs/100.0);
      Serial.print("s  ");
      
      Serial.print("Freq's: ");
      for(int freqCnt = 0; freqCnt < freqCount; ++freqCnt)
      {
        Serial.print(Results[(resCnt+resultCount)%resultLen].freq[freqCnt]);
        Serial.print("(");
        Serial.print(Results[(resCnt+resultCount)%resultLen].mag[freqCnt]);
        Serial.print(")  ");
      }
      Serial.print("\n");
    }
  }
}

/*
 * records an input to sig array
 */
void Signals::rec(byte& input)
{
  if(Signal.recLen < Signal.len)
  {
    sig[recLen] = input;
    Signal.recLen++;
  }
}

/*
 * clears Results
 */
void Signals::clrResults()
{
  for(int i = 0; i < resultLen; ++i)
  {
    for(byte j = 0; j < 2; ++j)
    {
      Results[i].freq[j] = 0;
      Results[i].mag[j] = 0;
    }
    Results[i].time_cs = 0;
  }
  resultCount = 0;
}
