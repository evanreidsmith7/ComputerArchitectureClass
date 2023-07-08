/******************************
 * Submitted by: enter your first and last name and net ID
 * CS 3339 - Spring 2023, Texas State University
 * Project 5 Data Cache
 * Copyright 2023, Lee B. Hinkle, all rights reserved
 * Based on prior work by Martin Burtscher and Molly O'Neil
 * Redistribution in source or binary form, with or without modification,
 * is *not* permitted. Use in source or binary form, with or without
 * modification, is only permitted for academic use in CS 3339 at
 * Texas State University.
 ******************************/
#include <iostream>
#include <cstdlib>
#include <iomanip>
#include "CacheStats.h"
using namespace std;

CacheStats::CacheStats() {
  cout << "Cache Config: ";
  if(!CACHE_EN) {
    cout << "cache disabled" << endl;
  } else {
    cout << (SETS * WAYS * BLOCKSIZE) << " B (";
    cout << BLOCKSIZE << " bytes/block, " << SETS << " sets, " << WAYS << " ways)" << endl;
    cout << "  Latencies: Lookup = " << LOOKUP_LATENCY << " cycles, ";
    cout << "Read = " << READ_LATENCY << " cycles, ";
    cout << "Write = " << WRITE_LATENCY << " cycles" << endl;
  }

  loads = 0;
  stores = 0;
  load_misses = 0;
  store_misses = 0;
  writebacks = 0;
  for (int i = 0; i < SETS; i++)
  {
    rob[i] = 0;
    for (int j = 0; j < WAYS; j++)
    {
      valid[i][j] = 0;
      modify[i][j] = 0;
      tag[i][j] = 0;
    }
  }

  /* TODO: your code to initialize your datastructures here */
}
int CacheStats::nextRob(int currentRob)
{
  int newRob;
  if (currentRob == 3)
  {
    newRob = 0;
  } 
  else
  {
    newRob = currentRob+1;
  }
  return newRob;
}
/*
iterate over all the cache sets, and for each set, 
iterate over all the blocks in the set. 
If a block is marked as dirty, then a wb++.
*/
void CacheStats::cachedrain()
{
  for (int set = 0; set < SETS; set++)
  {
    for (int way = 0; way < WAYS; way++)
    {
      if (modify[set][way]) //theres dirty data in the cache!
      {
        writebacks++;
      }
    }    
  }
}
int CacheStats::access(uint32_t addr, ACCESS_TYPE type) {
  if(!CACHE_EN) { // full latency if the cache is disabled
    return (type == LOAD) ? READ_LATENCY : WRITE_LATENCY;
  }
  uint32_t incomingIndex = (0b00000000000000000000000011100000 & addr) >> 5;
  uint32_t incomingTag =   (0b11111111111111111111111100000000 & addr) >> 8;
  //int way = rob[incomingIndex];
  int way = 0;
  bool validFound = false;
  bool tagFound = false;

  switch(type)
  {
    case LOAD:
      loads++;
    break;
    case STORE:
      stores++;
    break;
  }
  for (way = 0; way < 4; way++)
  {
    if (valid[incomingIndex][way] == 1 && tag[incomingIndex][way] == incomingTag) //HIT
    {
      if (type == LOAD)
      {
        return LOOKUP_LATENCY;
      }
      else //its a store
      {
        modify[incomingIndex][way] = 1;
        return LOOKUP_LATENCY;
      }
    }
  }
  //MISS*************************************************************************


  valid[incomingIndex][rob[incomingIndex]] = 1;
  tag[incomingIndex][rob[incomingIndex]] = incomingTag;


  
  if(type == LOAD && !modify[incomingIndex][rob[incomingIndex]])
  { 
    load_misses++;   
    rob[incomingIndex] = nextRob(rob[incomingIndex]);
    return READ_LATENCY;   
  }
  if (type == STORE &&  !modify[incomingIndex][rob[incomingIndex]])
  {
    modify[incomingIndex][rob[incomingIndex]] = 1;
    store_misses++;
    rob[incomingIndex] = nextRob(rob[incomingIndex]);
    return READ_LATENCY;  
  }
  if (type == LOAD && modify[incomingIndex][rob[incomingIndex]])
  {
    modify[incomingIndex][rob[incomingIndex]] = 0;
    load_misses++;
    writebacks++;
    rob[incomingIndex] = nextRob(rob[incomingIndex]);    
    return READ_LATENCY+WRITE_LATENCY; 

  }
  if (type == STORE &&  modify[incomingIndex][rob[incomingIndex]])
  {
    store_misses++;
    writebacks++;
    rob[incomingIndex] = nextRob(rob[incomingIndex]);     
    return READ_LATENCY+WRITE_LATENCY;
  }


}



void CacheStats::printFinalStats() {
  /* TODO: your code here "drain" the cache of writebacks */

  int accesses = loads + stores;
  int misses = load_misses + store_misses;
  cout << "Accesses: " << accesses << endl;
  cout << "  Loads: " << loads << endl;
  cout << "  Stores: " << stores << endl;
  cout << "Misses: " << misses << endl;
  cout << "  Load misses: " << load_misses << endl;
  cout << "  Store misses: " << store_misses << endl;
  cout << "Writebacks: " << writebacks << endl;
  cout << "Hit Ratio: " << fixed << setprecision(1) << 100.0 * (accesses - misses) / accesses << "%";
}
