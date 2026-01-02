#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H

#include <SDL2/SDL.h>

#include <stdint.h>
#include <stdbool.h>

typedef struct AudioManager
{
  SDL_AudioDeviceID device;
  bool playSound;
  int frequency;
  SDL_AudioSpec desiredAudioSpec;
  SDL_AudioSpec realAudioSpec;
} AudioManager;

void audioCallback(void* userdata, uint8_t* stream, int len)
{
  AudioManager* pAudioManager = (AudioManager*)userdata;

  int16_t* out = (int16_t*)stream;
  int samples = len / sizeof(int16_t);

  static double phase = 0.0;
  double phaseStep = pAudioManager->frequency / (double)pAudioManager->desiredAudioSpec.freq;

  for (int i = 0; i < samples; ++i) {
    double v = (phase < 0.5 ? 1.0 : -1.0);
    v *= pAudioManager->playSound;
    out[i] = (int16_t)(v * 3000);            // set volume
    phase += phaseStep;
    if (phase >= 1.0) phase -= 1.0;
  }
}

void initAudioManager(AudioManager* audioManager)
{
  SDL_memset(&audioManager->desiredAudioSpec, 0, sizeof(audioManager->desiredAudioSpec));
  audioManager->desiredAudioSpec.freq = 48000;
  audioManager->desiredAudioSpec.format = AUDIO_S16;
  audioManager->desiredAudioSpec.channels = 1;
  audioManager->desiredAudioSpec.samples = 1024;
  audioManager->desiredAudioSpec.callback = audioCallback;
  audioManager->desiredAudioSpec.userdata = audioManager;

  audioManager->device = SDL_OpenAudioDevice(NULL, 0, &audioManager->desiredAudioSpec,
      &audioManager->realAudioSpec, 0);
  audioManager->playSound = false;
  audioManager->frequency = 440;
  SDL_PauseAudioDevice(audioManager->device, SDL_FALSE);
}

#endif
