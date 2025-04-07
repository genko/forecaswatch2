#include "time_layer.h"
#include "c/appendix/config.h"

static Layer *s_canvas_layer;

static void canvas_update_proc(Layer *layer, GContext *ctx) {
    time_t temp = time(NULL);
    struct tm *tick_time = localtime(&temp);

    int8_t hour = tick_time->tm_hour > 12 ? tick_time->tm_hour - 12 :  tick_time->tm_hour;
    int8_t min = tick_time->tm_min;

    int32_t angle_start = 0;
    int32_t angle_end = 0;

    uint16_t inset_thickness = 10; 

    // Draw frame
    graphics_context_set_stroke_color(ctx, GColorRed);
    graphics_context_set_stroke_width(ctx, 5);

    // Draw chain stay
    GPoint start = GPoint(15+20, 50);
    GPoint end = GPoint(65, 50);
    graphics_draw_line(ctx, start, end);

    // Draw seatpost stay
    start = GPoint(15+20, 50);
    end = GPoint(15+20+20, 30);
    graphics_draw_line(ctx, start, end);

    // Draw seatpost
    start = GPoint(15+20+12, 15);
    end = GPoint(65, 50);
    graphics_draw_line(ctx, start, end);

    // Draw saddle
    graphics_context_set_stroke_color(ctx, GColorWhite);
    start = GPoint(15+20+7, 15);
    end = GPoint(57, 15);
    graphics_draw_line(ctx, start, end);

    // Draw top tube
    graphics_context_set_stroke_color(ctx, GColorRed);
    start = GPoint(15+20+14, 23);
    end = GPoint(85, 23);
    graphics_draw_line(ctx, start, end);

    // Draw down tube
    graphics_context_set_stroke_color(ctx, GColorRed);
    start = GPoint(65, 50);
    end = GPoint(85, 23);
    graphics_draw_line(ctx, start, end);

    // Draw fork
    graphics_context_set_stroke_color(ctx, GColorRed);
    start = GPoint(85, 23);
    end = GPoint(76+20, 30+20);
    graphics_draw_line(ctx, start, end);

    // Draw handlebar
    angle_start = DEG_TO_TRIGANGLE(0);
    angle_end = DEG_TO_TRIGANGLE(180);
    GRect rect_bounds = GRect(84, 12, 15, 15);

    graphics_context_set_fill_color(ctx, GColorRed);
    graphics_fill_radial(ctx, rect_bounds, GOvalScaleModeFitCircle, 4,
                                                        angle_start, angle_end);

    //Tires
    graphics_context_set_fill_color(ctx, GColorWhite);

    angle_start = DEG_TO_TRIGANGLE((hour*30)-355);
    angle_end = DEG_TO_TRIGANGLE((hour*30));
    rect_bounds = GRect(15, 30, 40, 40);
    graphics_fill_radial(ctx, rect_bounds, GOvalScaleModeFitCircle, inset_thickness,
                                                        angle_start, angle_end);

    angle_start = DEG_TO_TRIGANGLE((min*6)-355);
    angle_end = DEG_TO_TRIGANGLE((min*6));
    rect_bounds = GRect(76, 30, 40, 40);
    graphics_fill_radial(ctx, rect_bounds, GOvalScaleModeFitCircle, inset_thickness,
                                                        angle_start, angle_end);
}

void time_layer_create(Layer* parent_layer, GRect frame) {
    s_canvas_layer = layer_create(frame);
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