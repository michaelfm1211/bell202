#include "wav.h"
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef M_PI
// from /usr/include/math.h because compliant C99 doesn't define M_PI
#define M_PI 3.14159265358979323846 /* pi */
#endif

#define SAMPLE_RATE 8000

#define FREQ_0 2200
#define FREQ_1 1200

void play_bit(bool bit, uint8_t *samples, size_t *sample_i) {
  double freq = bit ? FREQ_1 : FREQ_0;
  double time = (double)*sample_i / SAMPLE_RATE;
  size_t end_sample_i = *sample_i + (SAMPLE_RATE / 1200);
  while (*sample_i < end_sample_i) {
    time = (double)*sample_i / SAMPLE_RATE;
    samples[*sample_i] = 127 * cos(2 * M_PI * freq * time) + 127;
    *sample_i += 1;
  }
}

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "usage: %s input.bin\n", argv[0]);
    return 1;
  }

  FILE *input = fopen(argv[1], "rb");
  if (!input) {
    perror("fopen()");
    return 1;
  }
  fseek(input, 0, SEEK_END);
  size_t input_size = ftell(input);
  fseek(input, 0, SEEK_SET);

  // * (10 / 8) because 1 start bit, 8 data bits, 1 stop bit, then 2 sync bytes
  size_t output_size = ceilf((float)input_size * 10 / 8) + 2;
  uint8_t *input_data = malloc(output_size);
  if (!input_data) {
    perror("malloc()");
    return 1;
  }
  if (!fread(input_data, 1, input_size, input)) {
    perror("fread()");
    fclose(input);
    return 1;
  }

  size_t num_samples = output_size * 8 * (SAMPLE_RATE / 1200);
#ifdef DEBUG
  fprintf(stderr, "allocating room for %ld samples\n", num_samples);
#endif
  if (!write_riff_wav_hdr(1, 1200 * (SAMPLE_RATE / 1200), num_samples)) {
    perror("write_riff_wav_hdr()");
    free(input_data);
    fclose(input);
    return 1;
  }

  uint8_t *samples = malloc(sizeof(uint8_t) * num_samples);
  size_t sample_i = 0;
  // synchronization bits
  for (int i = 0; i < 16; i++) {
    play_bit(1, samples, &sample_i);
  }
  // convert input
  for (size_t i = 0; i < input_size; i++) {
    // start bit
    play_bit(0, samples, &sample_i);

    // iterate over every bit in the byte
    uint8_t byte = input_data[i];
    for (int j = 0; j < 8; j++) {
      // make some samples for this bit
      play_bit(byte & 0x1, samples, &sample_i);
      byte >>= 1;
    }

    // stop bit
    play_bit(1, samples, &sample_i);
  }
#ifdef DEBUG
  fprintf(stderr, "number of samples used: %ld\n", sample_i);
#endif

  if (!fwrite(samples, sizeof(uint8_t), num_samples, stdout)) {
    perror("fwrite()");
    free(samples);
    free(input_data);
    fclose(input);
    return 1;
  }

  free(samples);
  free(input_data);
  fclose(input);
  return 0;
}
