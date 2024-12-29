#include <hardware/adc.h>
#include <hardware/clocks.h>
#include <hardware/dma.h>
#include <pico/types.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "capture.h"

// Private State ===============================================================

// NOTE: The DMA controller's automatic address wrapping for ring-buffers works
// on *addresses* — not indicies — so this buffer must be aligned to a multiple
// of its size
alignas(FULL_RING_SIZE) static volatile uint8_t audio_buf[FULL_RING_SIZE] = {};
static volatile uint8_t buffers_ready = 0;
static uint8_t active_buffer = 0;
static int dma_chan = 0;

// Interrupt Handlers ==========================================================

void buffer_captured() {
  // Mark the next buffer as ready and check that the CPU is keeping up
  if (++buffers_ready > BUFFER_COUNT) {
    printf("WARN: CPU is not keeping up with audio capture!\n");
    buffers_ready = BUFFER_COUNT;
  }

  // Clear the interrupt request
  dma_channel_acknowledge_irq0(dma_chan);
}

// Public Functions ============================================================

void capture_init() {
  // Init GPIO for analogue use: hi-Z, no pulls, disable digital input buffer.
  adc_gpio_init(ADC_GPIO_START + LEFT_CHANNEL);
  adc_gpio_init(ADC_GPIO_START + RIGHT_CHANNEL);

  // Init the ADC and tell it to round-robin sample the left and right channels,
  // starting with the left one
  adc_init();
  adc_select_input(LEFT_CHANNEL);
  adc_set_round_robin(1 << LEFT_CHANNEL | 1 << RIGHT_CHANNEL);

  // Configure the ADC to sample each channel at approximately `SAMPLE_RATE` Hz
  // NOTE: Because of the round-robin sampling, the ADC actually needs to sample
  // at `CHANNELS * SAMPLE_RATE` Hz to keep up. The sampling period is `1 + div`
  // cycles on average, with a minimum of 96 cycles
  float clkdiv = (float)clock_get_hz(clk_adc) / (CHANNELS * SAMPLE_RATE) - 1;
  adc_set_clkdiv(clkdiv);

  // Configure the ADC FIFO for byte DMA
  adc_fifo_setup(
      true,  // Write each completed conversion to the sample FIFO
      true,  // Enable DMA data request (DREQ)
      1,     // DREQ (and IRQ) asserted when at least 1 sample present
      false, // We won't see the ERR bit because of 8 bit reads; disable
      true   // Truncate each sample to 8 bits for byte DMA
  );

  // Claim a DMA channel and get its default config
  dma_chan = dma_claim_unused_channel(true);
  dma_channel_config cfg = dma_channel_get_default_config(dma_chan);

  // Configure the channel to read bytes from constant address and write to
  // incrementing byte addresses in a ring buffer
  channel_config_set_transfer_data_size(&cfg, DMA_SIZE_8);
  channel_config_set_read_increment(&cfg, false);
  channel_config_set_write_increment(&cfg, true);
  channel_config_set_ring(&cfg, true, FULL_RING_SIZE_BITS);

  // Pace transfers based on the availability of ADC samples
  channel_config_set_dreq(&cfg, DREQ_ADC);

  // Write the new configuration, but don't trigger the channel yet
  dma_channel_set_config(dma_chan, &cfg, false);

  // Tell the DMA to raise IRQ line 0 when the channel finishes a block
  dma_channel_set_irq0_enabled(dma_chan, true);

  // Configure the processor to run block_ready() when DMA IRQ 0 is asserted
  irq_set_exclusive_handler(DMA_IRQ_0, buffer_captured);
  irq_set_enabled(DMA_IRQ_0, true);

  // Set the transfer count and enable `TRIGGER_SELF` in the `MODE` bits
  // TODO: Replace this with `dma_channel_set_trans_count()` after this issue is
  // closed: https://github.com/raspberrypi/pico-sdk/issues/2059
  dma_hw->ch[dma_chan].transfer_count =
      (CHANNELS * BUFFER_SIZE) | 1 << DMA_CH0_TRANS_COUNT_MODE_LSB;

  // Configure the channel's read and write addresses, then trigger the channel
  dma_channel_set_read_addr(dma_chan, &adc_hw->fifo, false);
  dma_channel_set_write_addr(dma_chan, audio_buf, true);
}

void capture(bool enabled) { adc_run(enabled); }

bool buffer_ready() { return (bool)buffers_ready; }

uint8_t get_sample(uint channel, size_t index) {
  size_t idx = (CHANNELS * (active_buffer * BUFFER_SIZE + index) + channel) %
               FULL_RING_SIZE;
  return audio_buf[idx];
}

void next_buffer() {
  active_buffer = (active_buffer + 1) % BUFFER_COUNT;
  --buffers_ready;
}
