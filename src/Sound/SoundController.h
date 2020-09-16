#ifndef SOUNDCONTROLLER_H
#define SOUNDCONTROLLER_H

#include <iostream>
#include <thread>
#include <vector>

#include "OpenAL/al.h"
#include "OpenAL/alc.h"
#include "CWaves.h"
#include "EventQueue.h"

#define NEW_FOLLOWER_FILE "media/new_follower.wav"

class SoundController {
    enum SOUNDS {
        NEW_FOLLOWER,
        NEW_SUBSCRIBER
    };
    public:
        SoundController();
        static void playSound(SOUNDS sound, int i);
        void processEvent(updateEvent* event);
        ~SoundController();
        void cleanupSoundThreads();
    private:
        ALboolean ALFWLoadWaveToBuffer(const char *szWaveFile, ALuint uiBufferID);
        void initOpenAL();
        void initSources();
        void initBuffers();
        static std::vector<std::thread*> soundThreads;
        static std::vector<bool> soundThreadsActive;
        int soundsActive = 0;
        ALCdevice *device;
        ALCcontext *context;
        static ALuint sources[1];
        static ALuint buffers[1];
        CWaves* g_pWaveLoader;
};

#endif