/******************************
 * Copyright 2022, Lee B. Hinkle, all rights reserved
 * Based on prior work by Martin Burtscher and Molly O'Neil
 * Redistribution in source or binary form, with or without modification,
 * is *not* permitted. Use in source or binary form, with or without
 * modification, is only permitted for academic use in CS 3339 at
 * Texas State University.
 ******************************/
#ifndef __STATS_H
#define __STATS_H
#include <iostream>
#include <iomanip>
#include "Debug.h"
using namespace std;

enum PIPESTAGE { IF1 = 0, IF2 = 1, ID = 2, EXE1 = 3, EXE2 = 4, MEM1 = 5, 
                 MEM2 = 6, WB = 7, PIPESTAGES = 8 };

class Stats {
  private:
    long long cycles;
    int flushes;
    int bubbles;

    int memops;
    int branches;
    int taken;
    int stalls;
    
    int resultReg[PIPESTAGES]; //register numbers
    int resultStage[PIPESTAGES]; //pipestages that registers are valid

    int rawEXE1;
    int rawEXE2;
    int rawMEM1;
    int rawMEM2;

  public:
    Stats();

    void clock();

    void flush(int count);

    void registerSrc(int r, PIPESTAGE needed);
    void registerDest(int r, PIPESTAGE valid);

    void stall(int numCycles); //needs fixed in cpp
    void countMemOp() { memops++; }
    void countBranch() { branches++; }
    void countTaken() { taken++; }
    void countRawEXE1(){ rawEXE1++;}
    void countRawEXE2(){ rawEXE2++;}
    void countRawMEM1(){ rawMEM1++;}
    void countRawMEM2(){ rawMEM2++;}
    

    void showPipe();
    string stringNeeded(PIPESTAGE needed);

    // getters
    long long getCycles() { return cycles; }
    int getFlushes() { return flushes; }
    int getBubbles() { return bubbles; }
    int getMemOps() { return memops; }
    int getBranches() { return branches; }
    int getTaken() { return taken; }
    int getStalls() { return stalls;}

    int getRawHazardsOnExe1() {return rawEXE1;}
    int getRawHazardsOnExe2() {return rawEXE2;}
    int getRawHazardsOnMem1() {return rawMEM1;}
    int getRawHazardsOnMem2() {return rawMEM2;}
    int getRawHazards(){return rawEXE1+rawEXE2+rawMEM1+rawMEM2;}

  private:
    void bubble();
};

#endif
