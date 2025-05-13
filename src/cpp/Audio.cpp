#include "Audio.h"
#include "JS.h"

#include <cstdlib>
#include <ctime>
#include <string>

#ifndef __EMSCRIPTEN__
    #include <windows.h>
    #include <mmsystem.h>
    #pragma comment(lib, "winmm.lib")
#endif

void playAudio(const char* filepath) {
    #ifdef __EMSCRIPTEN__
    js::playSound(filepath);
    #else
    PlaySound(TEXT(filepath), NULL, SND_FILENAME | SND_ASYNC);
    #endif
}

#define SOUND_GO_0 "audio/whoosh-0.wav"
#define SOUND_GO_1 "audio/whoosh-1.wav"
#define SOUND_REVEAL "audio/reveal.wav"
#define SOUND_STUCK "audio/stuck.wav"
#define SOUND_FAIL "audio/fail.wav"

void Audio::initialize() {
    js::loadSound(SOUND_GO_0);
    js::loadSound(SOUND_GO_1);
    js::loadSound(SOUND_REVEAL);
    js::loadSound(SOUND_STUCK);
    js::loadSound(SOUND_FAIL);
}

void Audio::playGo() {
    static bool randInit = false;
    if (!randInit) {
        std::srand(static_cast<unsigned>(std::time(nullptr)));
        randInit = true;
    }

    const char* sounds[2] = {
        SOUND_GO_0,
        SOUND_GO_1,
    };

    int choice = std::rand() % (sizeof(sounds) / sizeof(const char*));
    const char* sound = sounds[choice];
    playAudio(sound);
}

void Audio::playReveal() {
    playAudio(SOUND_REVEAL);
}

void Audio::playStuck() {
    playAudio(SOUND_STUCK);
}

void Audio::playFail() {
    playAudio(SOUND_FAIL);
}