#include <stdio.h>
#include "pico/stdlib.h"

#define BUTTON_PIN 0
#define ENCODER_CCW 1
#define ENCODER_CW 2

typedef struct {
    bool last_state;
    bool last_processed_state;
    absolute_time_t stable_after;
} db_btn_t;

void gpio_active_low(uint pin) {
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_IN);
    gpio_pull_up(pin);
}

// Perform initialisation
int init(void) {
    // STDIO
    stdio_init_all();

    // Inputs
    uint inputs[] = {BUTTON_PIN, ENCODER_CW, ENCODER_CCW};
    for (int i = 0; i < sizeof(inputs) / sizeof(inputs[0]); ++i) {
        gpio_active_low(inputs[i]);
    }
    return PICO_OK;
}

int main() {
    int rc = init();
    hard_assert(rc == PICO_OK);

    int counter = 0;
    bool last_button = gpio_get(BUTTON_PIN);
    bool last = gpio_get(ENCODER_CW);
    bool last_processed = last;
    bool print_flag = true;
    absolute_time_t debounce = nil_time;
    absolute_time_t button_debounce = nil_time;
    while (true) {
        // Press the encoder button
        bool current_button = gpio_get(BUTTON_PIN);
        if (!current_button && last_button && get_absolute_time() > button_debounce) {
            counter = 0;
            print_flag = true;
            button_debounce = make_timeout_time_ms(2);
            last_button = current_button;
        }

        // Encoder stuff
        bool current = gpio_get(ENCODER_CW);
        if (current != last)
            debounce = make_timeout_time_ms(1);
        if (current != last_processed && get_absolute_time() > debounce) {
            if (!current) {
                if (!gpio_get(ENCODER_CCW))
                    --counter;
                else
                    ++counter;
                print_flag = true;
            }
            last_processed = current;
        }
        last = current;

        if (print_flag) {
            printf("Counter: %d\n", counter);
            print_flag = false;
        }
            
    }
}
//funky burger