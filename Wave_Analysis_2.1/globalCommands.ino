bool Signals::globalCommands(String& in)
{
  if(in == "C")//togggles continuous
  {
    continuous = !continuous;
    Serial.print("Countinuous: ");
    Serial.println(continuous);
    return true;
  }
  else if(in == "D")//toggles details
  {
    detail = !detail;
    Serial.print("Detail: ");
    Serial.println(detail);
    return true;
  }
  else if(in == "PR")//prints results
  {
    printResult();
    return true;
  }
  else if(in == "EST")//estimates the result or not
  {
    estimate = !estimate;
    Serial.print("Estimate: ");
    Serial.println(estimate);
    return true;
  }
  else if(in.length() > 2 && in.substring(0,3) == "REF")//sets reference level
  {
    if(in.length() > 3)
    {
      String temp = in.substring(3);
      if(temp == "D")//detects to set a reference level
        refLvl = ADCH;
      else
        refLvl = (temp.toInt());
    }
  
    Serial.print("Ref. Lvl.: ");
    Serial.println(refLvl);
    return true;
  }
  else if(in[0] == 'A')//sets analog input
  {
    if(in.length() > 1)
       switchAnalog((in.substring(1)).toInt());
    Serial.print("Analog Input: A");
    Serial.println((ADMUX & B111) | (((ADCSRB & (1<<MUX5))>>MUX5)<<3));
    return true;
  }
  else if(in == "CLRR")//clears results
  {
    clrResults();
    Serial.println("Results Cleared");
    return true;
  }
  
  return false;
}