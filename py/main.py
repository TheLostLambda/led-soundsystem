import array, time, machine
from machine import Pin
import rp2

# Configure the number of WS2812 LEDs.
NUM_LEDS = 50


@rp2.asm_pio(
    sideset_init=rp2.PIO.OUT_LOW,
    out_shiftdir=rp2.PIO.SHIFT_LEFT,
    autopull=True,
    pull_thresh=8,
)
def ws2812():
    T1 = 2
    T2 = 5
    T3 = 3
    label("bitloop")
    out(x, 1).side(0)[T3 - 1]
    jmp(not_x, "do_zero").side(1)[T1 - 1]
    jmp("bitloop").side(1)[T2 - 1]
    label("do_zero")
    jmp("bitloop").side(0)[T2 - 1]


sm = rp2.StateMachine(0, ws2812, freq=8_000_000, sideset_base=Pin(16))


# FIXME: Wrap this up in an object, so `sm` isn't a global
def put_pixels(barr):
    global sm
    sm.put(barr, 24)


def rgbwc(r, g, b, w=0, c=0):
    return bytearray([g, r, b, w, c])


# def put_rgbwc(r, g, b, w=0, c=0):
#     put_pixel(rgbwc(r, g, b, w, c))


sm.active(1)
power = 0
adc = machine.ADC(0)
while True:
    power = (power + 1) % 2
    # power = (power + 1) % NUM_LEDS
    # power = adc.read_u16() / 65535 * 3.3 * NUM_LEDS
    # if new_power > power:
    #     power = new_power
    # else:
    #     power = 0.02 * new_power + (1 - 0.02) * power
    # sm.active(0)
    pixels = bytearray(
        [
            byte
            for led in range(NUM_LEDS)
            for byte in (rgbwc(25, 0, 0) if power == 1 else rgbwc(0, 0, 0))
        ]
    )
    sm.put(pixels, 24)
