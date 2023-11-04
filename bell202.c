#include "wav.h"
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define SAMPLE_RATE 48000

#define FREQ_0 2200
#define FREQ_1 1200

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

  // * (10 / 8) because 1 start bit, 8 data bits, 1 stop bit
  size_t output_size = ceilf((float)input_size * 10 / 8);
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
  fprintf(stderr, "num_samples: %ld\n", num_samples);
  if (!write_riff_wav_hdr(2, 1200 * (SAMPLE_RATE / 1200), num_samples)) {
    perror("write_riff_wav_hdr()");
    free(input_data);
    fclose(input);
    return 1;
  }

  int16_t *samples = malloc(sizeof(int16_t) * num_samples);
  size_t sample_i = 0;
  for (size_t i = 0; i < input_size; i++) {
    // start bit
    double time = (double)sample_i / SAMPLE_RATE;
    size_t end_sample_i = sample_i + (SAMPLE_RATE / 1200);
    while (sample_i < end_sample_i) {
      time = (double)sample_i / SAMPLE_RATE;
      samples[sample_i] = 32767 * cos(2 * M_PI * FREQ_0 * time);
      sample_i++;
    }

    // iterate over every bit in the byte
    uint8_t byte = input_data[i];
    for (int j = 0; j < 8; j++) {
      // make some samples for this bit
      double freq = byte & 0x1 ? FREQ_1 : FREQ_0;
      /* double freq = byte & 0x1 ? FREQ_1 : FREQ_0; */
      end_sample_i = sample_i + (SAMPLE_RATE / 1200);
      while (sample_i < end_sample_i) {
        time = (double)sample_i / SAMPLE_RATE;
        samples[sample_i] = 32767 * cos(2 * M_PI * freq * time);
        sample_i++;
      }
      /* byte <<= 1; */
      byte >>= 1;
    }

    // stop bit
    end_sample_i = sample_i + (SAMPLE_RATE / 1200);
    while (sample_i < end_sample_i) {
      time = (double)sample_i / SAMPLE_RATE;
      samples[sample_i] = 32767 * cos(2 * M_PI * FREQ_1 * time);
      sample_i++;
    }
  }
  fprintf(stderr, "end sample_i: %ld\n", sample_i);

  if (!fwrite(samples, sizeof(int16_t), num_samples, stdout)) {
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
