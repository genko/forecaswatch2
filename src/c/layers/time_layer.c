#include "time_layer.h"
#include "c/appendix/config.h"

// MT = Margin Top
#define MT_TIME 14
#define MT_AM_PM 7


static Layer *s_canvas_layer;
static GRect rect_bounds;

static void canvas_update_proc(Layer *layer, GContext *ctx) {
    time_t temp = time(NULL);
    struct tm *tick_time = localtime(&temp);

    int8_t hour = tick_time->tm_hour > 12 ? tick_time->tm_hour - 12 :  tick_time->tm_hour;
    int8_t min = tick_time->tm_min;

    int32_t angle_start = DEG_TO_TRIGANGLE((hour*30)-355);
    int32_t angle_end = DEG_TO_TRIGANGLE((hour*30));

    uint16_t inset_thickness = 10; 

    // Set the fill color
    graphics_context_set_fill_color(ctx, GColorWhite);

    GRect rect_bounds = GRect(10, 10, 40, 40);

    graphics_fill_radial(ctx, rect_bounds, GOvalScaleModeFitCircle, inset_thickness,
                                                        angle_start, angle_end);

    angle_start = DEG_TO_TRIGANGLE((min*6)-355);
    angle_end = DEG_TO_TRIGANGLE((min*6));
    rect_bounds = GRect(80, 10, 40, 40);

    graphics_fill_radial(ctx, rect_bounds, GOvalScaleModeFitCircle, inset_thickness,
                                                        angle_start, angle_end);
}

void time_layer_create(Layer* parent_layer, GRect frame) {
    s_canvas_layer = layer_create(frame);
    rect_bounds = frame;
    layer_set_update_proc(s_canvas_layer, canvas_update_proc);
    layer_add_child(parent_layer, s_canvas_layer);
}

void time_layer_tick() {
    layer_mark_dirty(s_canvas_layer);
}

void time_layer_refresh() {
    time_layer_tick();  // Update main time text and layer positions
}

void time_layer_destroy() {
    layer_destroy(s_canvas_layer);
}