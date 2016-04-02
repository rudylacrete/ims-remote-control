#include <pebble.h>
#include "app_communication.h"
#include "valve.h"

#define NUM_MENU_SECTIONS 1

static SimpleMenuLayer *s_simple_menu_layer;
static SimpleMenuSection s_menu_sections[NUM_MENU_SECTIONS];
static SimpleMenuItem *s_first_menu_items;
static ActionMenuLevel *s_action_menu;

static Window *s_window;
static TextLayer *s_text_layer;

static Valve_s* selectedValve;

static void action_menu_callback(ActionMenu *action_menu, const ActionMenuItem *action, void *context) {
  ValveCmdCode i = (ValveCmdCode)action_menu_item_get_action_data(action);
  APP_LOG(APP_LOG_LEVEL_INFO, "Action menu triggered: %d", i);
  sendCmdRequest(selectedValve->guid, i);
}

static void initActionMenu() {
  s_action_menu = action_menu_level_create(16);
  action_menu_level_add_action(s_action_menu, "Open", action_menu_callback, (void*)CmdOpen);
  action_menu_level_add_action(s_action_menu, "Close", action_menu_callback, (void*)CmdClose);
}

static void menu_select_callback(int index, void *ctx) {
  Valve_s* v = getValveByIndex(index);
  //SimpleMenuItem* i = &s_first_menu_items[index];
  //i->subtitle = "Openning ....";
  //layer_mark_dirty(simple_menu_layer_get_layer(s_simple_menu_layer));
  selectedValve = v;
  ActionMenuConfig config = (ActionMenuConfig){
    .root_level = s_action_menu
  };
  action_menu_open(&config);
}

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
      .subtitle = guid_str,
      .callback = menu_select_callback,
    };
  }

  s_menu_sections[0] = (SimpleMenuSection) {
    .num_items = getValveNumber(),
    .items = s_first_menu_items,
  };

  Layer *window_layer = window_get_root_layer(s_window);
  GRect bounds = layer_get_frame(window_layer);

  s_simple_menu_layer = simple_menu_layer_create(bounds, s_window, s_menu_sections, NUM_MENU_SECTIONS, NULL);
  
  initActionMenu();
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
  action_menu_hierarchy_destroy(s_action_menu, NULL, NULL);
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
