#include "app_message.h"
#include "persist.h"
#include "math.h"
#include "c/layers/forecast_layer.h"
#include "c/layers/weather_status_layer.h"
#include "c/layers/loading_layer.h"
#include "c/windows/main_window.h"

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
    APP_LOG(APP_LOG_LEVEL_INFO, "Message received!");
    // Weather data
    Tuple *temp_trend_tuple = dict_find(iterator, MESSAGE_KEY_TEMP_TREND_INT16);
    Tuple *precip_trend_tuple = dict_find(iterator, MESSAGE_KEY_PRECIP_TREND_UINT8);
    Tuple *forecast_start_tuple = dict_find(iterator, MESSAGE_KEY_FORECAST_START);
    Tuple *num_entries_tuple = dict_find(iterator, MESSAGE_KEY_NUM_ENTRIES);
    Tuple *current_temp_tuple = dict_find(iterator, MESSAGE_KEY_CURRENT_TEMP);
    Tuple *city_tuple = dict_find(iterator, MESSAGE_KEY_CITY);
    Tuple *sun_events_tuple = dict_find(iterator, MESSAGE_KEY_SUN_EVENTS);

    // Clay config options
    Tuple *clay_celsius_tuple = dict_find(iterator, MESSAGE_KEY_CLAY_CELSIUS);
    Tuple *clay_time_lead_zero_tuple = dict_find(iterator, MESSAGE_KEY_CLAY_TIME_LEAD_ZERO);
    Tuple *clay_axis_12h_tuple = dict_find(iterator, MESSAGE_KEY_CLAY_AXIS_12H);
    Tuple *clay_start_mon_tuple = dict_find(iterator, MESSAGE_KEY_CLAY_START_MON);
    Tuple *clay_prev_week_tuple = dict_find(iterator, MESSAGE_KEY_CLAY_PREV_WEEK);
    Tuple *clay_color_today_tuple = dict_find(iterator, MESSAGE_KEY_CLAY_COLOR_TODAY);
    Tuple *clay_time_font_tuple = dict_find(iterator, MESSAGE_KEY_CLAY_TIME_FONT);
    Tuple *clay_show_qt_tuple = dict_find(iterator, MESSAGE_KEY_CLAY_SHOW_QT);
    Tuple *clay_show_bt_tuple = dict_find(iterator, MESSAGE_KEY_CLAY_SHOW_BT);
    Tuple *clay_show_bt_disconnect_tuple = dict_find(iterator, MESSAGE_KEY_CLAY_SHOW_BT_DISCONNECT);
    Tuple *clay_show_am_pm_tuple = dict_find(iterator, MESSAGE_KEY_CLAY_SHOW_AM_PM);

    if(temp_trend_tuple && temp_trend_tuple && forecast_start_tuple && num_entries_tuple && city_tuple && sun_events_tuple) {
        // Weather data received
        APP_LOG(APP_LOG_LEVEL_INFO, "All tuples received!");
        persist_set_forecast_start((time_t)forecast_start_tuple->value->int32);
        const int num_entries = ((int)num_entries_tuple->value->int32);
        persist_set_num_entries(num_entries);
        int16_t *temp_data = (int16_t*) temp_trend_tuple->value->data;
        persist_set_temp_trend(temp_data, num_entries);
        uint8_t *precip_data = (uint8_t*) precip_trend_tuple->value->data;
        persist_set_precip_trend(precip_data, num_entries);
        persist_set_city((char*)city_tuple->value->cstring);
        int lo, hi;
        min_max(temp_data, num_entries, &lo, &hi);
        persist_set_temp_lo(lo);
        persist_set_temp_hi(hi);
        persist_set_current_temp((int)current_temp_tuple->value->int32);
        uint8_t sun_event_start_type = (uint8_t) sun_events_tuple->value->uint8;
        time_t *sun_event_times = (time_t*) (sun_events_tuple->value->data + 1);
        persist_set_sun_event_start_type(sun_event_start_type);
        persist_set_sun_event_times(sun_event_times, 2);
        loading_layer_refresh();
        forecast_layer_refresh();
        weather_status_layer_refresh();
    }
    if (clay_celsius_tuple && clay_axis_12h_tuple && clay_start_mon_tuple && clay_prev_week_tuple && clay_color_today_tuple
        && clay_show_qt_tuple && clay_show_bt_tuple && clay_show_bt_disconnect_tuple && clay_show_am_pm_tuple) {
        // Clay config data received
        bool clay_celsius = (bool) (clay_celsius_tuple->value->int16);
        bool time_lead_zero = (bool) (clay_time_lead_zero_tuple->value->int16);
        bool axis_12h = (bool) (clay_axis_12h_tuple->value->int16);
        bool start_mon = (bool) (clay_start_mon_tuple->value->int16);
        bool prev_week = (bool) (clay_prev_week_tuple->value->int16);
        bool show_qt = (bool) (clay_show_qt_tuple->value->int16);
        bool show_bt = (bool) (clay_show_bt_tuple->value->int16);
        bool show_bt_disconnect = (bool) (clay_show_bt_disconnect_tuple->value->int16);
        bool show_am_pm = (bool) (clay_show_am_pm_tuple->value->int16);
        int16_t time_font = clay_time_font_tuple->value->int16;
        GColor color_today = GColorFromHEX(clay_color_today_tuple->value->int32);
        Config config = (Config) {
            .celsius = clay_celsius,
            .time_lead_zero = time_lead_zero,
            .axis_12h = axis_12h,
            .start_mon = start_mon,
            .prev_week = prev_week,
            .time_font = time_font,
            .color_today = color_today,
            .show_qt = show_qt,
            .show_bt = show_bt,
            .show_bt_disconnect = show_bt_disconnect,
            .show_am_pm = clay_show_am_pm_tuple
        };
        persist_set_config(config);
        main_window_refresh();
    }
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

void app_message_init() {
    // Register callbacks
    app_message_register_inbox_received(inbox_received_callback);
    app_message_register_inbox_dropped(inbox_dropped_callback);

    // Open AppMessage
    const int inbox_size = 256;
    const int outbox_size = 0;
    app_message_open(inbox_size, outbox_size);
}