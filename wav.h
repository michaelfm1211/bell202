#pragma once

#include <stddef.h>
#include <stdint.h>

#define RIFF_CHUNK_ID 0x46464952
#define WAVE_FORMAT 0x45564157

#define SUBCHUNK_ID_FMT 0x20746D66
#define FMT_SUBCHUNK_PCM_SIZE 16
#define AUDIO_FORMAT_PCM 1

#define SUBCHUNK_ID_DATA 0x61746164

// Mostly from http://soundfile.sapp.org/doc/WaveFormat/
struct __attribute__((packed)) riff_wav_hdr {
  uint32_t riff_chunk_id; // Always RIFF_CHUNK_ID
  uint32_t chunk_size;    // Size of entire chunk (headers + data) after this.
  uint32_t wave_format;   // Always WAVE_FMT

  uint32_t fmt_subchunk_id;   // Always SUBCHUNK_ID_FMT
  uint32_t fmt_subchunk_size; // Always FMT_SUBCHUNK_PCM_SIZE
  uint16_t audio_format;      // We use AUDIO_FORMAT_PCM
  uint16_t num_channels;      // We use 1 (mono)
  uint32_t sample_rate;       // Samples per second
  uint32_t byte_rate;         // Sample_rate * num_channels * bits_per_sample/8
  uint16_t block_align; // Bytes in one sample: num_channels * bits_per_sample/8
  uint16_t bits_per_sample; // Audio precision in bits (multiple of 8).

  uint32_t data_subchunk_id;   // Always SUBCHUNK_ID_DATA
  uint32_t data_subchunk_size; // num_samples * num_channels * bits_per_sample/8
};

int write_riff_wav_hdr(uint16_t bytes_per_sample, uint32_t sample_rate,
                       size_t num_samples);
