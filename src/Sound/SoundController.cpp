#include "SoundController.h"

ALuint SoundController::sources[1];
ALuint SoundController::buffers[1];
std::vector<std::thread*> SoundController::soundThreads = std::vector<std::thread*>();
std::vector<bool> SoundController::soundThreadsActive = std::vector<bool>();

SoundController::SoundController(){
    initOpenAL();
    initSources();
    initBuffers();
}

void SoundController::initOpenAL(){
    device = alcOpenDevice(NULL);
    if(!device){
            std::cout << "No Sound device was found!" << std::endl;
    }
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
    alGenBuffers(1, buffers);
    g_pWaveLoader = new CWaves();

    ALFWLoadWaveToBuffer(NEW_FOLLOWER_FILE, buffers[0]);
}

SoundController::~SoundController(){
    alDeleteSources(1, sources);
    alDeleteBuffers(1, buffers);
    device = alcGetContextsDevice(context);
    alcMakeContextCurrent(NULL);
    alcDestroyContext(context);
    alcCloseDevice(device);
    cleanupSoundThreads();
}

void SoundController::processEvent(updateEvent* event){
    if(event->action != updateEvent::ACTION::NONE){
        if(event->action == updateEvent::ACTION::NEW_FOLLOWER){
            soundThreadsActive.push_back(true);
            soundThreads.push_back(new std::thread(playSound, NEW_FOLLOWER, soundsActive));
            soundsActive++;
        }
    }
}

void SoundController::cleanupSoundThreads(){
    for(int i=0;i<soundThreadsActive.size();i++){
        if(!soundThreadsActive[i]){
            delete soundThreads[i];
            soundThreads.erase(soundThreads.begin() + i);
            soundThreadsActive.erase(soundThreadsActive.begin() + i);
        }
    }
}

void SoundController::playSound(SOUNDS sound, int i){
    alSourcei(sources[0], AL_BUFFER, buffers[sound]);
    alSourcePlay(sources[0]);
    ALint source_state;
    alGetSourcei(sources[0], AL_SOURCE_STATE, &source_state);
    while (source_state == AL_PLAYING) {
            alGetSourcei(sources[0], AL_SOURCE_STATE, &source_state);
    }
    soundThreadsActive[i] = false;
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