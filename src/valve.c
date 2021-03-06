#include "valve.h"

static int valveNumber = 0;
static int _allocValveNumber = 0;

// private pointers which store all valve configuration
static Valve_s *valves;
static AllValveSetCallback cb = NULL;
static bool pendingCbCall = false;

void allocValveNumber(int number) {
  _allocValveNumber = number;
  valves = malloc(sizeof(Valve_s) * number);
}

int getValveNumber() {
  return valveNumber;
}

bool isAllValveSet() {
  return valveNumber == _allocValveNumber;
}

Valve_s* addValve(char* name, int guid) {
  Valve_s* valve = valves + (valveNumber++);
  memcpy(valve->name, name, sizeof(valve->name));
  valve->guid = guid;
  
  // check wheither cb is set and have to be called
  if(isAllValveSet()) {
    if(cb) cb(valves);
    else pendingCbCall = true;
  }
  return valve;
}

Valve_s* getValveByIndex(int index) {
  if(index >= valveNumber)
    return NULL;
  return valves + index;
}

char* printValve(Valve_s* valve) {
  char* str = (char*)malloc(sizeof(char) * 100);
  snprintf(str, 100, "Name: %s | guid: %d", valve->name, valve->guid);
  return str;
}

void setValveSetCompleteCallback(AllValveSetCallback _cb) {
  cb = _cb;
  if(pendingCbCall) {
    cb(valves);
  }
}

void freeValve() {
  free(valves);
}