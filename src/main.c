#include <pico/stdio.h>
#include <pico/time.h>
#include <stdio.h>

int main() {
  stdio_init_all();
  while (true) {
    printf("Hola, Mom!\n");
    sleep_ms(1000);
  }
}
