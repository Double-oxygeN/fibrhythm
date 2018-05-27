#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <sndfile.h> /* -lsndfile */

#define SAMPLERATE 44100
#define CHANNELS 1
#define DB0 (1.0 * 0x7f000000)
#define BASEFREQ 82.407
#define BPS (336.0 / 60.0)

double sineWave(double freq, int offset) {
  return sin(freq / SAMPLERATE * 2 * offset * M_PI);
}

double fmWave(double freq, int offset) {
  return sin(freq / SAMPLERATE * 2 * offset * M_PI + M_PI / 2 * sin(freq / SAMPLERATE * 2 * offset * M_PI));
}

double fmWave2(double freq, int offset) {
  return sin(freq / SAMPLERATE * 2 * offset * M_PI + M_PI / 4 * sin(2.0 * freq / SAMPLERATE * 2 * offset * M_PI));
}

bool addWaveFromRhythmFile(int *buffer, char const *filename, int count, int offset, double freq, double amp, double (*waveform)(double, int)) {
  FILE *fp = fopen(filename, "r");
  int i, j, c;
  int const beatInterval = SAMPLERATE / BPS;
  double r = 1.0, progress;

  if (fp == NULL) {
    fprintf(stderr, "Cannot open file %s!\n", filename);
    return false;
  }

  for (i = offset; i < count; i++) {
    if ((c = fgetc(fp)) == EOF) break;
    if (c != '0') {
      for (j = i * beatInterval; j < (i + 1) * beatInterval; j++) {
        progress = (double)(((i + 1) * beatInterval) - j) / (double)beatInterval;
        if (progress < 1.0 / 12.0)
          r = progress * 12;
        else if (progress > 2.0 / 3.0)
          r = (1 - progress) * 3;
        else
          r = 1.0;
        buffer[j] += (int)(amp * r * (*waveform)(freq, j));
      }
    }
  }

  fclose(fp);

  return true;
}

int main(int argc, char const *argv[]) {
  int count = 1;
  int sampleCount = SAMPLERATE;

  SNDFILE *file;
  SF_INFO sfinfo;
  int *buffer;
  int k;
  double amp;

  if (argc < 3 || argc % 2 == 1) {
    fprintf(stderr, "Usage: %s count filename offset [filename offset ...]\n", argv[0]);
    return 3;
  }
  count = atoi(argv[1]);
  sampleCount = SAMPLERATE * (count / BPS);

  // initialize buffer
  buffer = (int *)malloc(sampleCount * CHANNELS * sizeof(int));
  if (buffer == NULL) {
    fprintf(stderr, "Memory allocation error!\n");
    return errno;
  }

  // set sound file info
  memset(&sfinfo, 0, sizeof(sfinfo));
  sfinfo.samplerate = SAMPLERATE;
  sfinfo.frames = sampleCount;
  sfinfo.channels = CHANNELS;
  sfinfo.format = (SF_FORMAT_WAV | SF_FORMAT_PCM_24);

  // open wav file
  file = sf_open("nanika.wav", SFM_WRITE, &sfinfo);
  if (file == NULL) {
    fprintf(stderr, "Not able to open sound file!\n");
    return errno;
  }

  // create buffer
  amp = DB0 / (argc / 2 - 1);
  for (k = 2; k < argc; k += 2) {
    addWaveFromRhythmFile(buffer, argv[k], count, atoi(argv[k+1]), BASEFREQ * (k / 2 + 1), amp, fmWave2);
  }

  // drain buffer to the file
  if (sf_write_int(file, buffer, CHANNELS * sampleCount) != CHANNELS * sampleCount) {
    fprintf(stderr, "%s\n", sf_strerror(file));
  }

  // free memory
  sf_close(file);
  free(buffer);

  return 0;
}
