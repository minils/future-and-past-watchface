#include <pebble.h>

Window *window;
TextLayer *text_layer;

GBitmap *future_bitmap, *past_bitmap;
BitmapLayer *future_layer, *past_layer;

InverterLayer *inv_layer;

char buffer[] = "00:00";

/**
 *  Update the watchface display.
 */
void tick_handler(struct tm *tick_time, TimeUnits units_changed)
{
  // Format the buffer string using tick_time as the time source
  strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
  
  // Change the TextLayer text to show the new time
  text_layer_set_text(text_layer, buffer);
}

/**
 *  Load the window and it's elements.
 */
void window_load(Window *window)
{
  // load font
  ResHandle font_handle = resource_get_handle(RESOURCE_ID_FONT_DIGITAL_DREAM_30);
  
  text_layer = text_layer_create(GRect(8, 60, 125, 160));
  text_layer_set_background_color(text_layer, GColorClear);
  text_layer_set_text_color(text_layer, GColorBlack);
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  //text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_font(text_layer, fonts_load_custom_font(font_handle));
  
  layer_add_child(window_get_root_layer(window), (Layer*) text_layer);
  
  inv_layer = inverter_layer_create(GRect(0, 50, 144, 62));
  layer_add_child(window_get_root_layer(window), (Layer*) inv_layer);
  
  // load bitmaps into GBitmap structures
  future_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_FUTURE);
  past_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PAST);
  
  // create BitmapLayers to show GBitMaps and add to Window
  future_layer = bitmap_layer_create(GRect(0, 0, 144, 50));
  bitmap_layer_set_bitmap(future_layer, future_bitmap);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(future_layer));
  
  past_layer = bitmap_layer_create(GRect(0, 112, 144, 50));
  bitmap_layer_set_bitmap(past_layer, past_bitmap);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(past_layer));
  
  // get a time structure so that the face doesn't start blank
  struct tm *t;
  time_t temp;
  temp = time(NULL);
  t = localtime(&temp);
  
  // manually call the tick handler
  tick_handler(t, MINUTE_UNIT);
}

/**
 *  Unload the window.
 */
void window_unload(Window *window)
{
  text_layer_destroy(text_layer);
  inverter_layer_destroy(inv_layer);
  
  gbitmap_destroy(future_bitmap);
  gbitmap_destroy(past_bitmap);
  
  bitmap_layer_destroy(future_layer);
  bitmap_layer_destroy(past_layer);
}

/**
 *  Initialize the app.
 */
void init()
{
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  window_stack_push(window, true);
  tick_timer_service_subscribe(MINUTE_UNIT, (TickHandler) tick_handler);
}

/**
 *  Deinitialize the app to save memory.
 */
void deinit()
{
  tick_timer_service_unsubscribe();
  window_destroy(window);
}

/**
 *  Standard main.
 */
int main(void)
{
  init();
  app_event_loop();
  deinit();
}