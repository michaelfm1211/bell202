#include "wav.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define SAMPLE_RATE 8000

#define FREQ_0 2200
#define FREQ_1 1200

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "usage: %s input.bin\n", argv[0]);
    return 1;
  }

  FILE *input = fopen(argv[1], "rb");
  fseek(input, 0, SEEK_END);
  size_t input_size = ftell(input);
  fseek(input, 0, SEEK_SET);

  uint8_t *input_data = malloc(input_size);
  if (!fread(input_data, 1, input_size, input)) {
    perror("fread()");
    fclose(input);
    return 1;
  }

  size_t num_samples = input_size * 8 * (SAMPLE_RATE / 1200);
  fprintf(stderr, "num_samples: %ld\n", num_samples);
  if (!write_riff_wav_hdr(1, 1200 * (SAMPLE_RATE / 1200), num_samples)) {
    perror("write_riff_wav_hdr()");
    free(input_data);
    fclose(input);
    return 1;
  }

  int16_t *samples = malloc(sizeof(int16_t) * num_samples);
  size_t sample_i = 0;
  for (size_t i = 0; i < input_size; i++) {
    uint8_t byte = input_data[i];
    // iterate over every byte
    for (int j = 0; j < 8; j++) {
      // make some samples for this bit
      double freq = byte & 0x80 ? FREQ_1 : FREQ_0;
      size_t end_sample_i = sample_i + (SAMPLE_RATE / 1200);
      while (sample_i < end_sample_i) {
        double time = (double)sample_i / SAMPLE_RATE;
        /* fprintf(stderr, "sample_i: %ld; byte %ld, bit %d\n", sample_i, i, j); */
        /* samples[sample_i] = 127 * cos(2 * M_PI * freq * time) + 127; */
        samples[sample_i] = 32767 * cos(2 * M_PI * freq * time);
        sample_i++;
      }
      byte <<= 1;
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
