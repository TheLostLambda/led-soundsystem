#include <hardware/clocks.h>

#include "ws2805.h"
#include "ws2805.pio.h"

ws2805_t ws2805_init(uint pin) {
  PIO pio;
  uint sm;
  uint offset;

  // Claim a free PIO and state machine, then load our PIO program and return
  // its offset — checking for success
  bool success =
      pio_claim_free_sm_and_add_program(&ws2805_program, &pio, &sm, &offset);
  hard_assert(success);

  // Configure a single output GPIO at `pin`
  pio_gpio_init(pio, pin);
  pio_sm_set_consecutive_pindirs(pio, sm, pin, 1, true);

  // Update the default config by specifying the sideset pins, fusing the RX
  // FIFO into the TX FIFO (doubling its size), and enabling autopull for the
  // `out` shift register
  pio_sm_config c = ws2805_program_get_default_config(offset);
  sm_config_set_sideset_pins(&c, pin);
  sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_TX);
  // We need 40 bits for a single pixel, so autopull 2x20 bits
  sm_config_set_out_shift(&c, false, true, 20);

  // Set the clock divider to achieve the desired baud rate of `ws2805_RATE`
  uint cycles_per_bit = ws2805_T1 + ws2805_T2 + ws2805_T3;
  float div = (float)clock_get_hz(clk_sys) / (ws2805_RATE * cycles_per_bit);
  sm_config_set_clkdiv(&c, div);

  // Initialise and start the PIO state machine
  pio_sm_init(pio, sm, offset, &c);
  pio_sm_set_enabled(pio, sm, true);

  return (ws2805_t){.pio = pio, .sm = sm};
}

void put_pixel(ws2805_t ws2805, grbwc_t pixel) {
  // The most-significant-bit (MSB) of the final `uint32_t` must contain the
  // first bit of each 20-bit packet, so we shift our 40-bit number accordingly
  pio_sm_put_blocking(ws2805.pio, ws2805.sm, (uint32_t)(pixel >> 8));
  pio_sm_put_blocking(ws2805.pio, ws2805.sm, (uint32_t)(pixel << 12));
}

grbwc_t rgbwc(uint8_t r, uint8_t g, uint8_t b, uint8_t w, uint8_t c) {
  // WS2805 LED bytes are ordered: green, red, blue, warm-white, cool-white
  return ((uint64_t)(g) << 32) | ((uint64_t)(r) << 24) | ((uint64_t)(b) << 16) |
         ((uint64_t)(w) << 8) | (uint64_t)(c);
}
