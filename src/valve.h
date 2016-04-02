#pragma once
#include <pebble.h>

typedef struct Valve {
  char name[80];
  int guid;
} Valve_s;

typedef enum {
  CmdOpen = 0,
  CmdClose,
} ValveCmdCode;

// definition of callback to call when valve are all set
typedef void(* AllValveSetCallback)(Valve_s*);

void allocValveNumber(int);
int getValveNumber(void);
Valve_s* addValve(char*, int);
char* printValve(Valve_s*);
bool isAllValveSet(void);
void setValveSetCompleteCallback(AllValveSetCallback);
void freeValve(void);
Valve_s* getValveByIndex(int);