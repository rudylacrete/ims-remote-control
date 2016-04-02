#include "app_communication.h"
#include "valve.h"

static bool s_js_ready;

bool comm_is_js_ready() {
  return s_js_ready;
}

static void inbox_received_callback(DictionaryIterator *iter, void *context) {
  Tuple *ready_tuple = dict_find(iter, AppKeyJSReady);
  Tuple *valveName = dict_find(iter, ValveName);
  Tuple *valveGuid = dict_find(iter, ValveGuid);
  Tuple * valveNumber = dict_find(iter, ValveNumber);
  if(ready_tuple) {
    // PebbleKit JS is ready! Safe to send messages
    s_js_ready = true;
    APP_LOG(APP_LOG_LEVEL_INFO, "Peeble JS ready");
  }
  if(valveNumber) {
    int vNumber = (int)valveNumber->value->int32;
    APP_LOG(APP_LOG_LEVEL_INFO, "Valve number: %d", vNumber);
  }
  if(valveName && valveGuid) {
    static char name[80];
    snprintf(name, sizeof(name), "%s", valveName->value->cstring);
    Valve_s* v = addValve(valveName->value->cstring, valveGuid->value->int32);
    APP_LOG(APP_LOG_LEVEL_INFO, "Name -> %s | guid -> %d", v->name, v->guid);
  }
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

void initAppCom() {
  // Register callbacks
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);
  
  // Open AppMessage
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
}