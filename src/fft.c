#include <complex.h>
#include <math.h>
#include <stddef.h>
#include <stdint.h>

void fft(complex float data[], uint8_t bits) {
  size_t length = 1 << bits;
  // FIXME: Obviously give things some more meaningful names...
  for (size_t level = 0; level < bits; ++level) {
    size_t le = 2 << (bits - level);
    size_t le2 = le / 2;
    complex float u = 1.0 + 0.0 * I;
    complex float s = cos(M_PI / le2) - sin(M_PI / le2) * I;
    for (size_t j = 0; j < le2; ++j) {
      for (size_t i = 0; i < length; i += le) {
        size_t ip = i + le2;
        complex float t = data[i] + data[ip];
        data[ip] = (data[i] - data[ip]) * u;
        data[i] = t;
      }
      u = u * s;
    }
  }
  size_t nd2 = length / 2;
  size_t nm1 = length - 1;
  // FIXME: Right type?
  size_t j = 1;
  for (size_t i = 0; i < nm1; ++i) {
    if (i > j) {
      complex float t = data[j];
      data[j] = data[i];
      data[i] = t;
    }
    size_t k = nd2;
    while (k < j) {
      j = j - k;
      k = k / 2;
    }
    j += k;
  }
}
