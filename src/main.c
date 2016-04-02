#include <pebble.h>
#include "app_communication.h"
#include "valve.h"

#define NUM_MENU_SECTIONS 1
#define NUM_FIRST_MENU_ITEMS 1

static SimpleMenuLayer *s_simple_menu_layer;
static SimpleMenuSection s_menu_sections[NUM_MENU_SECTIONS];
static SimpleMenuItem *s_first_menu_items;


static Window *s_window;
static TextLayer *s_text_layer;

static void allValveSetCallback(Valve_s* valves) {
  static char text[100];
  snprintf(text, sizeof(text), "Fetch done (%d valves)", getValveNumber());
  text_layer_set_text(s_text_layer, text);
  
  s_first_menu_items = (SimpleMenuItem*)malloc(sizeof(SimpleMenuItem) * getValveNumber());
  for(int i=0; i < getValveNumber(); i++){
    char* guid_str = malloc(20);
    snprintf(guid_str, 20, "GUID %d", valves[i].guid);
    s_first_menu_items[i] = (SimpleMenuItem) {
      .title = valves[i].name,
      .subtitle = guid_str
    };
  }

  s_menu_sections[0] = (SimpleMenuSection) {
    .num_items = getValveNumber(),
    .items = s_first_menu_items,
  };

  Layer *window_layer = window_get_root_layer(s_window);
  GRect bounds = layer_get_frame(window_layer);

  s_simple_menu_layer = simple_menu_layer_create(bounds, s_window, s_menu_sections, NUM_MENU_SECTIONS, NULL);

  layer_add_child(window_layer, simple_menu_layer_get_layer(s_simple_menu_layer));
}

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
	
  // Create a text layer and set the text
	s_text_layer = text_layer_create(bounds);
	text_layer_set_text(s_text_layer, "Fetching valves ...");
  
  // Set the font and text alignment
	text_layer_set_font(s_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
	text_layer_set_text_alignment(s_text_layer, GTextAlignmentCenter);

	// Add the text layer to the window
	layer_add_child(window_layer, text_layer_get_layer(s_text_layer));
  
  // Enable text flow and paging on the text layer, with a slight inset of 10, for round screens
  text_layer_enable_screen_text_flow_and_paging(s_text_layer, 10);
  
  setValveSetCompleteCallback(allValveSetCallback);
}

static void main_window_unload(Window *window) {
  text_layer_destroy(s_text_layer);
  simple_menu_layer_destroy(s_simple_menu_layer);
}

static void init(void) {
	// Create a window and get information about the window
	s_window = window_create();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload,
  });
  
	// Push the window, setting the window animation to 'true'
	window_stack_push(s_window, true);
	
	// App Logging!
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Just pushed a window!");
  
  initAppCom();
}

static void deinit(void) {	
	// Destroy the window
	window_destroy(s_window);
  freeValve();
}

int main(void) {
	init();
	app_event_loop();
	deinit();
}
