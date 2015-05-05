// Sound.cpp

#include "Sound.h"
using namespace std;

Sound::Sound()
{
	m_data = nullptr;
    m_DirectSound = 0;
	m_primaryBuffer = 0;
	m_secondaryBuffer1 = 0;
}
Sound::Sound(const string& path)
{
	m_data = nullptr;
	m_filename = path + ".wav";
	m_path = path;
	m_DirectSound = 0;
	m_primaryBuffer = 0;
	m_secondaryBuffer1 = 0;
}
Sound::Sound(const string& path, const int numFrames)
{
	m_filename = path + ".wav";
	m_numFrames = numFrames;
	m_path = path;
	//m_data = new unsigned int[m_numFrames];
	m_data = nullptr;

	m_DirectSound = 0;
	m_primaryBuffer = 0;
	m_secondaryBuffer1 = 0;

}
Sound::Sound(const Sound& rc)
{
	m_filename = rc.m_filename;
	m_path = rc.m_path;
	m_numFrames = rc.m_numFrames;
	m_data = new unsigned int[m_numFrames];
	memcpy(m_data, rc.m_data, sizeof(unsigned int) * m_numFrames);

	//m_data = rc.m_data;
	//for (i = 0; i < m_numFrames; i++)
	//m_data[i] = rc.m_data[i];

	//m_DirectSound = rc.m_DirectSound;
	//m_primaryBuffer = rc.m_primaryBuffer;
	//m_secondaryBuffer1 = rc.m_secondaryBuffer1;
	

}
Sound::~Sound()
{
	//if (m_data != nullptr)
		//delete[] m_data;
	//m_data = nullptr;
}
bool Sound::Initialize(/*HWND hwnd*/)
{
	bool result;

	//Initialize direct sound and the primary sound buffer

	result = InitializeDirectSound(/*hwnd*/);
	if (!result)
	{
		return false;
	}

	//Load a wave audio file onto a secondary buffer
	result = LoadWaveFile(&m_secondaryBuffer1);
	if (!result)
	{
		return false;
	}

	/* just in case

	//Play the wave file
	result = PlayWaveFile();
	if (!result)
	{
	return false;
	}
	*/

	return true;
}
void Sound::Shutdown()
{
	//Release the secondary buffer
	ShutdownWaveFile(&m_secondaryBuffer1);

	//Shutdown the Direct Sound API
	ShutdownDirectSound();

	return;
}
void Sound::setNumFrames(const int numFrames)
{
	m_numFrames = numFrames;
}
void Sound::setFileName(const string& path)
{
	m_filename = path + ".wav";
	m_path = path;
}
unsigned int* Sound::data()
{
	return m_data;
}
int Sound::getNumFrames()
{
	return m_numFrames;
}
string& Sound::getPath()
{
	return m_path;
}
bool Sound::InitializeDirectSound(/*HWND hwnd*/)
{
	HRESULT result;
	//DSBUFFERDESC bufferDesc;
	//WAVEFORMATEX waveFormat;

	//Initialize the DirectSound interface pointer for the default sound device
	result = DirectSoundCreate8(NULL, &m_DirectSound, NULL);
	if (FAILED(result))
	{
		return false;
	}







	/* just in case

	//Set the cooperative level to priority so the format of the primary sound buffer can be modified.
	result = m_DirectSound->SetCooperativeLevel(hwnd, DSSCL_PRIORITY);
	if (FAILED(result))
	{
	return false;
	}

	//Setup the primary buffer description
	bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	bufferDesc.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME;
	bufferDesc.dwBufferBytes = 0;
	bufferDesc.dwReserved = 0;
	bufferDesc.lpwfxFormat = NULL;
	bufferDesc.guid3DAlgorithm = GUID_NULL;

	//Get control of the primary sound buffer on the default sound device.
	result = m_DirectSound->CreateSoundBuffer(&bufferDesc, &m_primaryBuffer, NULL);
	if (FAILED(result))
	{
	return false;
	}

	// Setup the format of the primary sound buffer.
	//In this case it is a .WAV file recorded at 44100 samples per second in 16 bits stereo
	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nSamplesPerSec = 44100;
	waveFormat.wBitsPerSample = 16;
	waveFormat.nChannels = 2;
	waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	waveFormat.cbSize = 0;

	//Set the primary buffer to be the wave format specified
	result = m_primaryBuffer->SetFormat(&waveFormat);
	if (FAILED(result))
	{
	return false;
	}
	*/


	return true;

}
void Sound::ShutdownDirectSound()
{
	//Release the primary sound buffer pointer
	if (m_primaryBuffer)
	{
		m_primaryBuffer->Release();
		m_primaryBuffer = 0;
	}

	//Release the DirectSound interface pointer
	if (m_DirectSound)
	{
		m_DirectSound->Release();
		m_DirectSound = 0;
	}

	return;
}

bool Sound::LoadWaveFile(IDirectSoundBuffer8 ** secondaryBuffer)
{
	int error, i, j;
	FILE* filePtr;
	size_t count;
	WaveHeaderType waveFileHeader;
	WAVEFORMATEX waveFormat;
	DSBUFFERDESC bufferDesc;
	HRESULT result;
	IDirectSoundBuffer* tempBuffer;
	unsigned char* waveData;
	unsigned int* wavesampleData;
	unsigned char* bufferPtr;
	unsigned long BufferSize;
	unsigned long tempSize;//the number of samples of one slot in waveData for downsampling

	//open the wave file in binary
	error = fopen_s(&filePtr, m_filename.c_str(), "rb");
	if (error != 0)
	{
		return false;
	}

	//Read in the wave file header
	count = fread(&waveFileHeader, sizeof(waveFileHeader), 1, filePtr);
	if (count != 1)
	{
		return false;
	}

	//Check that the chunk ID is RIFF format
	if ((waveFileHeader.chunkId[0] != 'R') || (waveFileHeader.chunkId[1] != 'I') || (waveFileHeader.chunkId[2] != 'F') || (waveFileHeader.chunkId[3] != 'F'))
	{
		return false;
	}

	//check that the file format is the WAVE format
	if ((waveFileHeader.format[0] != 'W') || (waveFileHeader.format[1] != 'A') || (waveFileHeader.format[2] != 'V') || (waveFileHeader.format[3] != 'E'))
	{
		return false;
	}

	//check that the sub Chunk ID is the fmt format
	if ((waveFileHeader.subChunkId[0] != 'f') || (waveFileHeader.subChunkId[1] != 'm') || (waveFileHeader.subChunkId[2] != 't') || (waveFileHeader.subChunkId[3] != ' '))
	{
		return false;
	}

	//Check that the audio format is WAVE_FORMAT_PCM
	if (waveFileHeader.audioFormat != WAVE_FORMAT_PCM)
	{
		return false;
	}

	//check that the wave file was recorded in stereo format
	if (waveFileHeader.numChannels != 2)
	{
		false;
	}

	//Check that the wave file was recorded at a sample rate of 44100 Hz(44.1 KHz)
	if (waveFileHeader.sampleRate != 44100)
	{
		return false;
	}

	//Ensure that the wave file was recorded in 16 bit format.
	if (waveFileHeader.bitsPerSample != 16)
	{
		return false;
	}

	//Check for the data chunk header
	if ((waveFileHeader.dataChunkId[0] != 'd') || (waveFileHeader.dataChunkId[1] != 'a') || (waveFileHeader.dataChunkId[2] != 't') || (waveFileHeader.dataChunkId[3] != 'a'))
	{
		return false;
	}

	//Set the wave format of secondary buffer that this wave file will be loaded into
	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nSamplesPerSec = 44100;
	waveFormat.wBitsPerSample = 16;
	waveFormat.nChannels = 2;
	waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8)*waveFormat.nChannels;
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	waveFormat.cbSize = 0;

	//Set the buffer description of the secondary sound buffer that the wave file will be loaded into.
	bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	bufferDesc.dwFlags = DSBCAPS_CTRLVOLUME;
	bufferDesc.dwBufferBytes = waveFileHeader.dataSize;
	bufferDesc.dwReserved = 0;
	bufferDesc.lpwfxFormat = &waveFormat;
	bufferDesc.guid3DAlgorithm = GUID_NULL;

	//Create a temporary sound buffer with the specific buffer settings
	result = m_DirectSound->CreateSoundBuffer(&bufferDesc, &tempBuffer, NULL);
	if (FAILED(result))
	{
		return false;
	}

	//Test the buffer format against the DirectSound 8 interface and create the secondary buffer
	result = tempBuffer->QueryInterface(IID_IDirectSoundBuffer8, (void**)&*secondaryBuffer);
	if (FAILED(result))
	{
		return false;
	}

	//Release the temporary buffer
	tempBuffer->Release();
	tempBuffer = 0;

	//Move to the beginning of the wave data which starts at the end of the data chunk header.
	fseek(filePtr, sizeof(WaveHeaderType), SEEK_SET);

	//Creat a temporary buffer to hold the wave file data
	waveData = new unsigned char[waveFileHeader.dataSize];
	if (!waveData)
	{
		return false;
	}

	//Read in the wave file data into the newly created buffer
	count = fread(waveData, 1, waveFileHeader.dataSize, filePtr);
	if (count != waveFileHeader.dataSize)
	{
		return false;
	}
	
	int sz = waveFileHeader.dataSize;
	wavesampleData = new unsigned int[sz / 4];
    
	// 4 bytes/sample
	for (i = 0, j = 0; i < sz; i = i + 4, j++)
	{
		wavesampleData[j] = waveData[i] + waveData[i + 1] + waveData[i + 2] + waveData[i + 3];
	}


	m_data = new unsigned int[m_numFrames];

	//downsampling the waveData for later use to m_data
	tempSize = unsigned long(waveFileHeader.dataSize / 4 / m_numFrames);
	for (i = 0, j = 0; i < m_numFrames; i++, j = j + tempSize)
	{
		m_data[i] = wavesampleData[j];

	}
	//cout << endl<<endl;
	//cout << "i  is " << i <<endl<< "j is " << j<<endl;

	//Close the file once done reading
	error = fclose(filePtr);
	if (error != 0)
	{
		return false;
	}

	//Lock the secondary buffer to write wave data into it.
	result = (*secondaryBuffer)->Lock(0, waveFileHeader.dataSize, (void**)&bufferPtr, (DWORD*)&BufferSize, NULL, 0, 0);
	if (FAILED(result))
	{
		return false;
	}

	//Copy the wave data into the buffer
	memcpy(bufferPtr, waveData, waveFileHeader.dataSize);

	//Unlock the secondaray buffer after the data has been written to it
	result = (*secondaryBuffer)->Unlock((void*)bufferPtr, BufferSize, NULL, 0);
	if (FAILED(result))
	{
		return false;
	}
	//Release the wave data since it was copied into the secondary buffer.
	delete[] waveData;
	waveData = 0;

	return true;

}

void Sound::ShutdownWaveFile(IDirectSoundBuffer8 ** secondaryBuffer)
{
	//Release the secondary sound buffer
	if (*secondaryBuffer)
	{
		(*secondaryBuffer)->Release();
		*secondaryBuffer = 0;
	}

	return;
}

/*
bool Sound::PlayWaveFile()
{

HRESULT result;

//Set position at the beginning of the sound buffer
result = m_secondaryBuffer1->SetCurrentPosition(0);
if (FAILED(result))
{
return false;
}

//Set volume of the buffer to 100%
result = m_secondaryBuffer1->SetVolume(DSBVOLUME_MAX);
if (FAILED(result))
{
return false;
}

//Play the contents of the secondary sound buffer
result = m_secondaryBuffer1->Play(0, 0, 0);
if (FAILED(result))
{
return false;
}


return true;
}

*/