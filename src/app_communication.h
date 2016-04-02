#pragma once
#include <pebble.h>

// AppMessage keys
typedef enum {
  AppKeyJSReady = 0,
  ValveNumber,
  ValveName,
  ValveGuid,
  /* other keys */

} AppKeys;

bool comm_is_js_ready(void);
void initAppCom(void);