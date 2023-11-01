#include "wav.h"
#include <stdio.h>
#include <stdlib.h>

int write_riff_wav_hdr(uint16_t bytes_per_sample, uint32_t sample_rate,
                        size_t num_samples) {
  struct riff_wav_hdr hdr = {
      // RIFF header
      .riff_chunk_id = RIFF_CHUNK_ID,
      .chunk_size = 36 + num_samples * bytes_per_sample,
      .wave_format = WAVE_FORMAT,
      // format subchunk
      .fmt_subchunk_id = SUBCHUNK_ID_FMT,
      .fmt_subchunk_size = FMT_SUBCHUNK_PCM_SIZE,
      .audio_format = AUDIO_FORMAT_PCM,
      .num_channels = 1,
      .sample_rate = sample_rate,
      .byte_rate = sample_rate * bytes_per_sample,
      .block_align = bytes_per_sample,
      .bits_per_sample = 8 * bytes_per_sample,
      // data subchunk
      .data_subchunk_id = SUBCHUNK_ID_DATA,
      .data_subchunk_size = num_samples * bytes_per_sample,
  };
  return fwrite(&hdr, sizeof(struct riff_wav_hdr), 1, stdout);
}
