#include "SoundController.h"

ALuint SoundController::sources[1];
ALuint SoundController::buffers[2];
std::mutex SoundController::soundQueueMux;
Semaphore SoundController::soundSem;

SoundController::SoundController(){
    initOpenAL();
    initSources();
    initBuffers();
    soundQueue = new std::queue<SOUNDS>();
    playerThread = new std::thread(soundPlayer, soundQueue);
}

void SoundController::initOpenAL(){
    device = alcOpenDevice(NULL);
    if(!device)
        std::cout << "No Sound device was found!" << std::endl;
    context = alcCreateContext(device, NULL);
    if(!alcMakeContextCurrent(context)){
        std::cout << "Failed to make sound context current" << std::endl;
        ALCenum error = alGetError();
        if(error != AL_NO_ERROR)
            std::cout << error << std::endl;
    }
}

void SoundController::initSources(){
    alGenSources(1, sources);
    alSourcef(sources[0], AL_PITCH, 1);
    alSourcef(sources[0], AL_GAIN, 1);
    alSource3f(sources[0], AL_POSITION, 0, 0, 0);
    alSource3f(sources[0], AL_VELOCITY, 0, 0, 0);
    alSourcei(sources[0], AL_LOOPING, AL_FALSE);
}

void SoundController::initBuffers(){
    alGenBuffers(2, buffers);
    g_pWaveLoader = new CWaves();

    ALFWLoadWaveToBuffer(NEW_FOLLOWER_FILE, buffers[0]);
    ALFWLoadWaveToBuffer(NEW_SUBSCRIBER_FILE, buffers[1]);
}

SoundController::~SoundController(){
    alDeleteSources(1, sources);
    alDeleteBuffers(2, buffers);
    device = alcGetContextsDevice(context);
    alcMakeContextCurrent(NULL);
    alcDestroyContext(context);
    alcCloseDevice(device);
}

void SoundController::processEvent(updateEvent* event){
    if(event->action != updateEvent::ACTION::NONE){
        if(event->action == updateEvent::ACTION::NEW_FOLLOWER){
            soundQueueMux.lock();
            soundQueue->push(SOUNDS::NEW_FOLLOWER);
            soundQueueMux.unlock();
            soundSem.release();
        }
        else if(event->action == updateEvent::ACTION::NEW_SUBSCRIBER || event->action == updateEvent::ACTION::GIFTED_SUBSCRIBER){
            soundQueueMux.lock();
            soundQueue->push(SOUNDS::NEW_SUBSCRIBER);
            soundQueueMux.unlock();
            soundSem.release();
        }
    }
}

void SoundController::soundPlayer(std::queue<SOUNDS>* soundQueue){
    while(1){
        soundSem.aqquire();
        soundQueueMux.lock();
        SOUNDS sound = soundQueue->front();
        soundQueue->pop();
        soundQueueMux.unlock();
        playSound(sound);
    }
}


void SoundController::playSound(SOUNDS sound){
    alSourcei(sources[0], AL_BUFFER, buffers[sound]);
    alSourcePlay(sources[0]);
    ALint source_state;
    alGetSourcei(sources[0], AL_SOURCE_STATE, &source_state);
    while (source_state == AL_PLAYING) {
        alGetSourcei(sources[0], AL_SOURCE_STATE, &source_state);
    }
}

ALboolean SoundController::ALFWLoadWaveToBuffer(const char *szWaveFile, ALuint uiBufferID)
{
	WAVEID			WaveID;
	ALint			iDataSize, iFrequency;
	ALenum			eBufferFormat;
	ALchar			*pData;
	ALboolean		bReturn;

	bReturn = AL_FALSE;
	if (g_pWaveLoader)
	{
		if (SUCCEEDED(g_pWaveLoader->LoadWaveFile(szWaveFile, &WaveID)))
		{
			if ((SUCCEEDED(g_pWaveLoader->GetWaveSize(WaveID, (unsigned long*)&iDataSize))) &&
				(SUCCEEDED(g_pWaveLoader->GetWaveData(WaveID, (void**)&pData))) &&
				(SUCCEEDED(g_pWaveLoader->GetWaveFrequency(WaveID, (unsigned long*)&iFrequency))) &&
				(SUCCEEDED(g_pWaveLoader->GetWaveALBufferFormat(WaveID, &alGetEnumValue, (unsigned long*)&eBufferFormat))))
			{
				alGetError();
				alBufferData(uiBufferID, eBufferFormat, pData, iDataSize, iFrequency);
				if (alGetError() == AL_NO_ERROR)
					bReturn = AL_TRUE;

				g_pWaveLoader->DeleteWaveFile(WaveID);
			}
		}
	}

	return bReturn;
}