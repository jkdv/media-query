//Sound.h

#ifndef _SOUND_H_
#define _SOUND_H_


//linking
#pragma comment(lib,"dsound.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"winmm.lib")

//include
#include <Windows.h>
#include <mmsystem.h>
#include <dsound.h>
#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class Sound{


private:

	string m_path;//the file's absolute path
	string m_filename;//file name including its path
	int m_numFrames;//the number of frames for downsampling
	unsigned int* m_data;//downsampled sound data (600 samples(if DB), 150 samples(if query)), not the original sound data

	IDirectSound8* m_DirectSound;//DirectSound object
	IDirectSoundBuffer* m_primaryBuffer;//primary buffer
	IDirectSoundBuffer8 * m_secondaryBuffer1;//the secondary buffer

	//structure for Header of Wave file
	struct WaveHeaderType
	{
		//RIFF chunk
		char chunkId[4];//the name of the following chunk: 'RIFF'(fixed)
		unsigned long chunkSize;// the size of the rest of file: the file's total size - 8 bytes
		char format[4];//the file's format: 'WAVE'(fixed)
		//fmt chunk
		char subChunkId[4];//the name of the following chunk : 'fmt '(fixed)
		unsigned long subChunkSize;//remaining chunk's size: 16 or 18
		unsigned short audioFormat;//this audio's format: 1(if PCM)
		unsigned short numChannels;//the number of channel of this sound file: 1(if mono), 2(if stereo)
		unsigned long sampleRate;//sampling frequency(Hz)
		unsigned long bytesPerSecond;//bytes per second
		unsigned short blockAlign;//the size of sample frame: the size of a sample*1(if mono), the size of a sample*2(if stereo)
		unsigned short bitsPerSample;//the size of a sample(bits/sample)
		//data chunk
		char dataChunkId[4];//the name of the following chunk: 'data'(fixed)
		unsigned long dataSize;//the size of the actual sound data(the size of the file - the size of header)
	};

public:
	Sound();
	Sound(const string& path);
	Sound(const string& path, int numFrames);
	Sound(const Sound&);
	~Sound();
	bool Initialize(/*HWND hwnd*/);//Initialization of DirectSound object,loading wave file,downsampling, and playing the file
	void Shutdown();// release wavefile and DirectSound object 

	void setNumFrames(int numFrames);
	void setFileName(const string& path);
	unsigned int* data();
	int getNumFrames();
	string& getPath();

private:
	bool InitializeDirectSound(/*HWND hwnd*/);
	void ShutdownDirectSound();

	bool LoadWaveFile(IDirectSoundBuffer8 **);
	void ShutdownWaveFile(IDirectSoundBuffer8 **);

	//bool PlayWaveFile();

	//void downsampling(int numFrames);
};

#endif