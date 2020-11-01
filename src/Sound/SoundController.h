#ifndef SOUNDCONTROLLER_H
#define SOUNDCONTROLLER_H

#include <iostream>
#include <thread>
#include <mutex>
#include <queue>

#include "../Semaphore.hpp"
#include "OpenAL/al.h"
#include "OpenAL/alc.h"
#include "CWaves.h"
#include "EventQueue.h"

#define NEW_FOLLOWER_FILE "media/new_follower.wav"
#define NEW_SUBSCRIBER_FILE "media/new_follower.wav"

class SoundController {
    enum SOUNDS {
        NEW_FOLLOWER,
        NEW_SUBSCRIBER
    };
    public:
        SoundController();
        static void playSound(SOUNDS sound);
        void processEvent(updateEvent* event);
        ~SoundController();
    private:
        ALboolean ALFWLoadWaveToBuffer(const char *szWaveFile, ALuint uiBufferID);
        static void soundPlayer(std::queue<SOUNDS>* soundQueue);
        std::thread* playerThread;
        void initOpenAL();
        void initSources();
        void initBuffers();
        std::queue<SOUNDS>* soundQueue;
        static std::mutex soundQueueMux;
        static Semaphore soundSem;
        int soundsActive = 0;
        ALCdevice *device;
        ALCcontext *context;
        static ALuint sources[1];
        static ALuint buffers[2];
        CWaves* g_pWaveLoader;
};

#endif