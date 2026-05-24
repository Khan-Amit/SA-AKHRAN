#include <stdio.h>
#include <stdbool.h>

// Link your external trigger operations from src/state_controller.c
extern void ui_trigger_on_click_run(void);
extern void ui_trigger_on_click_report(void);
extern void ui_trigger_on_click_stop(void);

// Evaluates standard touch coordinates based on your visual button placement
void process_dashboard_screen_tap(int touch_x, int touch_y, int screen_width, int screen_height) {
    // Standardize coordinates into percentages to accommodate varying phone aspect ratios
    float horizontal_ratio = (float)touch_x / screen_width;
    float vertical_ratio = (float)touch_y / screen_height;

    // 1. RUN BUTTON GEOMETRY (Roughly top third area of your dashboard buttons)
    if (horizontal_ratio >= 0.20f && horizontal_ratio <= 0.80f &&
        vertical_ratio >= 0.40f && vertical_ratio <= 0.48f) {
        printf("[UI Router] RUN button pressed at position (%d, %d)\n", touch_x, touch_y);
        ui_trigger_on_click_run();
        return;
    }

    // 2. REPORT BUTTON GEOMETRY (Middle horizontal segment)
    if (horizontal_ratio >= 0.20f && horizontal_ratio <= 0.80f &&
        vertical_ratio >= 0.52f && vertical_ratio <= 0.60f) {
        printf("[UI Router] REPORT button pressed at position (%d, %d)\n", touch_x, touch_y);
        ui_trigger_on_click_report();
        return;
    }

    // 3. STOP BUTTON GEOMETRY (Bottom horizontal segment)
    if (horizontal_ratio >= 0.20f && horizontal_ratio <= 0.80f &&
        vertical_ratio >= 0.64f && vertical_ratio <= 0.72f) {
        printf("[UI Router] STOP button pressed at position (%d, %d)\n", touch_x, touch_y);
        ui_trigger_on_click_stop();
        return;
    }
}
