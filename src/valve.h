#pragma once
#include <pebble.h>

typedef struct Valve {
  char name[80];
  int guid;
} Valve_s;

// definition of callback to call when valve are all set
typedef void(* AllValveSetCallback)(Valve_s *valves);

void allocValveNumber(int);
int getValveNumber(void);
Valve_s* addValve(char*, int);
char* printValve(Valve_s*);
bool allValveSet(void);
void setValveSetCompleteCallback(AllValveSetCallback);