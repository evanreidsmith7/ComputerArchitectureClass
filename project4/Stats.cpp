/******************************
 * Submitted by: Evan Smith ers131@txstate.edu
 * CS 3339 - Spring 2022, Texas State University
 * Project 3 Pipelining
 * Copyright 2022, Lee B. Hinkle, all rights reserved
 * Based on prior work by Martin Burtscher and Molly O'Neil
 * Redistribution in source or binary form, with or without modification,
 * is *not* permitted. Use in source or binary form, with or without
 * modification, is only permitted for academic use in CS 3339 at
 * Texas State University.
 ******************************/
#include "Stats.h"

Stats::Stats()
{
  cycles = PIPESTAGES - 1; // pipeline startup cost
  flushes = 0;
  bubbles = 0;

  memops = 0;
  branches = 0;
  taken = 0;
  
  rawEXE1 = 0;
  rawEXE2 = 0;
  rawMEM1 = 0;
  rawMEM2 = 0;

  for (int i = IF1; i < PIPESTAGES; i++)
  {
    resultReg[i] = -1;
  }
  for (int i = 0; i < PIPESTAGES; i++)
  {
    resultStage[i] = 0;
  }
}

void Stats::clock()
{
  D(cout << "clock()" << endl << endl;);
  cycles++;

  // advance all stages in pipeline
  for (int i = WB; i > IF1; i--)
  {
    resultReg[i] = resultReg[i - 1];
    resultStage[i] = resultStage[i - 1];
  }
  // inject a NOP in pipestage IF1
  resultReg[IF1] = -1;
  resultStage[IF1] = 0;
}



/*
  if (r == 0)
    return;
  for(int i = EXE1; i < WB; i++)
  {
    if (r==resultStage[i])
    {
      int ready = WB - 1;

      while(ready > 0)
      {
        bubble();
        ready--;
      }
      break;
    }
  }
*/
string Stats::stringNeeded(PIPESTAGE need)
{//this is for debugging
  string stage;
  switch(need)
  {
    case 0: stage = "0";
    break;
    case 1: stage = "F2";
    break;
    case 2: stage = "ID";
    break;
    case 3: stage = "E1";
    break;
    case 4: stage = "E2";
    break;
    case 5: stage = "M1";
    break;
    case 6: stage = "M2";
    break;
    case 7: stage = "WB";
    break;
    default: stage = "0";
  }
  return stage;
}

void Stats::registerSrc(int r, PIPESTAGE needed)
{ // r == the register being read DO check exe1 to mem2 reg 0 does not matter
  D(string stage = stringNeeded(needed));
  D(cout << dec << endl
         << "stats.Regsrc(" << r << ", " << stage << ")" << endl);
  D(showPipe());

  if (r != 0)
  {
    for (int i = EXE1; i < WB; i++) 
    {
      if (resultReg[i] == r) //
      {
        D(cout << i << " " << resultStage[i] << endl;)
        switch (i)
        {
          case EXE1: countRawEXE1();
          break;
          case EXE2: countRawEXE2();
          break;
          case MEM1: countRawMEM1();
          break;
          case MEM2: countRawMEM2();
          break;
        }
        int num = (resultStage[i]-i)-(needed - ID);
        D(cout << num << endl;)
        for (int i = 0; i < num; i++)
        {
          bubble();
        }
        break;
      }
    }
  }
}

void Stats::registerDest(int r, PIPESTAGE valid)
{ // r == the register to be written to
  // stage == stage that r's result is ready
  resultReg[ID] = r;
  resultStage[ID] = valid;
  D(cout << dec << endl
         << "stats.RegDest(" << r << ", " << valid << ")" << endl);
  D(showPipe());
}


/*
update flush count and call clock() for each count
the parameter count is ID-IF1
loop count times{
  adv pipeline (CLOCK())
  flush++;
}
*/
void Stats::flush(int count)
{ // count == how many ops to flush
  for (int i = 0; i < count; i++)
  {
    clock();
    flushes++;
    D(cout << endl
           << "flush" << endl);
  }
}

/*
if1 if2 id "frozen"
advance ex forward
copy clock code but it only goes to ex1
*/
void Stats::bubble()
{
  D(cout << endl
         << "bub" << endl);

  cycles++;
  bubbles++;
  // advance all stages from exe1 to wb
  for (int i = WB; i > ID; i--)
  {
    resultReg[i] = resultReg[i - 1];
    resultStage[i] = resultStage[i - 1];
  }
  // inject a NOP in pipestage EXE1
  resultReg[EXE1] = -1;
  resultStage[EXE1] = 0;
  D(showPipe());
}
void Stats::showPipe()
{
  // this method is to assist testing and debug, please do not delete or edit
  // you are welcome to use it but remove any debug outputs before you submit
  cout << "              IF1  IF2 *ID* EXE1 EXE2 MEM1 MEM2 WB         #C      #B      #F" << endl;
  cout << "  resultReg ";
  for (int i = 0; i < PIPESTAGES; i++)
  {
    cout << "  " << dec << setw(2) << resultReg[i] << " ";
  }
  //resultstageshow
  cout << endl;
  cout << "  resuStage ";
  for (int i = 0; i < PIPESTAGES; i++)
  {
    string stage = stringNeeded((PIPESTAGE)resultStage[i]);
    cout << "  " << dec << setw(2) << stage << " ";
  }
  cout << "   " << setw(7) << cycles << " " << setw(7) << bubbles << " " << setw(7) << flushes;
  cout << endl;
}

