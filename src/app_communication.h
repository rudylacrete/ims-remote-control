#pragma once
#include <pebble.h>
#include "valve.h"

// AppMessage keys
typedef enum {
  AppKeyJSReady = 0,
  ValveNumber,
  ValveName,
  ValveGuid,
  ValveCmdRequest,
  /* other keys */

} AppKeys;

bool comm_is_js_ready(void);
void initAppCom(void);
void sendCmdRequest(int, ValveCmdCode);