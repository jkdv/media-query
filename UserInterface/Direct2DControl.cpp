
// Direct2DControl.cpp : implementation file
//

#include "stdafx.h"
#include "UserInterface.h"
#include "Direct2DControl.h"

using namespace D2D1;

// CDirect2DControl

IMPLEMENT_DYNAMIC(CDirect2DControl, CStatic)

CDirect2DControl::CDirect2DControl()
: m_pImageFactory(nullptr)
, m_pWICBitmap(nullptr)
, m_pFormatConverter(nullptr)
, m_pD2DBitmap(nullptr)
, m_pVideo(nullptr)
, m_startFrame(0)
, m_currentFrame(0)
, m_bPlaying(FALSE)
, m_pDirectSound(nullptr)
, m_pPrimarySoundBuffer(nullptr)
, m_pSecondarySoundBuffer(nullptr)
{
	// Enable Direct2D support for this control.
	EnableD2DSupport();

	// COM function
	CoInitialize(nullptr);
	CoCreateInstance(CLSID_WICImagingFactory, nullptr,
		CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_pImageFactory));

	// Create direct sound8 interface
	HRESULT hr;
	hr = DirectSoundCreate8(
		nullptr, 
		&m_pDirectSound, 
		nullptr);

	// Create primary buffer
	DSBUFFERDESC bufferDesc;
	bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	bufferDesc.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME;;
	bufferDesc.dwBufferBytes = 0;
	bufferDesc.dwReserved = 0;
	bufferDesc.lpwfxFormat = nullptr;
	bufferDesc.guid3DAlgorithm = GUID_NULL;

	hr = m_pDirectSound->CreateSoundBuffer(
		&bufferDesc,
		&m_pPrimarySoundBuffer,
		nullptr);

	// Set wave format
	WAVEFORMATEX waveFormat;
	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nSamplesPerSec = 44100;
	waveFormat.wBitsPerSample = 16;
	waveFormat.nChannels = 2;
	waveFormat.nBlockAlign =
		(waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
	waveFormat.nAvgBytesPerSec =
		waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	waveFormat.cbSize = 0;

	hr = m_pPrimarySoundBuffer->SetFormat(&waveFormat);
}


BOOL CDirect2DControl::InsertVideo(Video* pVideo)
{
	if (pVideo == nullptr)
		return FALSE;

	if (IsPlaying()) {
		Stop();
	}

	m_pVideo = pVideo;
	LoadAudio(m_pVideo->getAutioFilePath().c_str());
	return ChangeCurrentFrame(0);
}


BOOL CDirect2DControl::LoadAudio(LPCSTR lpcPath)
{
	FILE* fp;
	errno_t err = fopen_s(&fp, lpcPath, "rb");
	if (err != 0)
		return FALSE;

	// Read in the wave file header.
	WAVEHEADERTYPE waveFileHeader;
	size_t sz = fread(
		&waveFileHeader, 
		sizeof(waveFileHeader), 
		1, 
		fp);

	if (sz != 1)
		return FALSE;

	// Check that the chunk ID is the RIFF format.
	if ((waveFileHeader.chunkId[0] != 'R') || 
		(waveFileHeader.chunkId[1] != 'I') ||
		(waveFileHeader.chunkId[2] != 'F') || 
		(waveFileHeader.chunkId[3] != 'F')) {
		return FALSE;
	}

	// Check that the file format is the WAVE format.
	if ((waveFileHeader.format[0] != 'W') || 
		(waveFileHeader.format[1] != 'A') ||
		(waveFileHeader.format[2] != 'V') || 
		(waveFileHeader.format[3] != 'E')) {
		return FALSE;
	}

	// Check that the sub chunk ID is the fmt format.
	if ((waveFileHeader.subChunkId[0] != 'f') || 
		(waveFileHeader.subChunkId[1] != 'm') ||
		(waveFileHeader.subChunkId[2] != 't') || 
		(waveFileHeader.subChunkId[3] != ' ')) {
		return FALSE;
	}

	// Check that the audio format is WAVE_FORMAT_PCM.
	if (waveFileHeader.audioFormat != WAVE_FORMAT_PCM) {
		return FALSE;
	}

	// Check that the wave file was recorded in stereo format.
	if (waveFileHeader.numChannels != 2) {
		return FALSE;
	}

	// Check that the wave file was recorded at a sample rate of
	// 44.1 KHz.
	if (waveFileHeader.sampleRate != 44100) {
		return FALSE;
	}

	// Ensure that the wave file was recorded in 16 bit format.
	if (waveFileHeader.bitsPerSample != 16) {
		return FALSE;
	}

	// Check for the data chunk header.
	if ((waveFileHeader.dataChunkId[0] != 'd') || 
		(waveFileHeader.dataChunkId[1] != 'a') ||
		(waveFileHeader.dataChunkId[2] != 't') || 
		(waveFileHeader.dataChunkId[3] != 'a')) {
		return FALSE;
	}

	// Set the wave format of secondary buffer
	// that this wave file will be loaded onto.
	WAVEFORMATEX waveFormat;
	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nSamplesPerSec = 44100;
	waveFormat.wBitsPerSample = 16;
	waveFormat.nChannels = 2;
	waveFormat.nBlockAlign =
		(waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
	waveFormat.nAvgBytesPerSec = 
		waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	waveFormat.cbSize = 0;

	// Set the buffer description of the secondary sound buffer
	// that the wave file will be loaded onto.
	DSBUFFERDESC bufferDesc;
	bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	bufferDesc.dwFlags = DSBCAPS_CTRLVOLUME;
	bufferDesc.dwBufferBytes = waveFileHeader.dataSize;
	bufferDesc.dwReserved = 0;
	bufferDesc.lpwfxFormat = &waveFormat;
	bufferDesc.guid3DAlgorithm = GUID_NULL;

	// Create a temporary sound buffer with the specific buffer
	// settings.
	HRESULT hr;
	IDirectSoundBuffer* pTempBuffer;
	hr = m_pDirectSound->CreateSoundBuffer(
		&bufferDesc, 
		&pTempBuffer,
		nullptr);

	if (FAILED(hr))
		return FALSE;

	// Test the buffer format against the direct sound 8 interface
	// and create the secondary buffer.
	SafeRelease(m_pSecondarySoundBuffer);
	hr = pTempBuffer->QueryInterface(
		IID_IDirectSoundBuffer8,
		(LPVOID*)&m_pSecondarySoundBuffer);

	if (FAILED(hr))
		return FALSE;
	SafeRelease(pTempBuffer);

	// Move to the beginning of the wave data which starts
	// at the end of the data chunk header.
	fseek(fp, sizeof(WAVEHEADERTYPE), SEEK_SET);

	// Create a temporary buffer to hold the wave file data.
	BYTE* waveData = nullptr;
	waveData = new BYTE[waveFileHeader.dataSize];

	// Read in the wave file data into the newly created buffer.
	sz = fread(waveData, 1, waveFileHeader.dataSize, fp);
	if (sz != waveFileHeader.dataSize)
		return FALSE;

	// Close the file once done reading.
	fclose(fp);

	// Lock the secondary buffer to write wave data into it.
	LPVOID bufferPtr;
	DWORD bufferSize;
	hr = m_pSecondarySoundBuffer->Lock(
		0, 
		waveFileHeader.dataSize, 
		&bufferPtr, 
		&bufferSize, 
		nullptr, 0, 0);

	if (FAILED(hr))
		return FALSE;

	// Copy the wave data into the buffer.
	memcpy(bufferPtr, waveData, waveFileHeader.dataSize);

	// Release the wave data since it was copied into the
	// secondary buffer.
	delete[] waveData;

	// Unlock the secondary buffer after the data has been
	// written to it.
	hr = m_pSecondarySoundBuffer->Unlock(
		bufferPtr, 
		bufferSize, 
		nullptr, 0);

	if (FAILED(hr))
		return FALSE;

	// Set volume of the buffer to 100%.
	hr = m_pSecondarySoundBuffer->SetVolume(DSBVOLUME_MAX);

	return TRUE;
}


BOOL CDirect2DControl::ChangeCurrentFrame(UINT n)
{
	if (m_pVideo == nullptr)
		return FALSE;

	if ((int)n >= m_pVideo->size()) {
		Stop();
		return FALSE;
	}
	
	// Stop video and audio
	if (IsPlaying()) {
		Pause();
	}

	// Set video frame
	m_startFrame = n;
	DrawFrame(n);

	// Set position at the beginning of the sound buffer.
	m_pSecondarySoundBuffer->SetCurrentPosition(GetCurrAudioPos());

	return TRUE;
}


BOOL CDirect2DControl::DrawHistogram(UINT nSize, DOUBLE* pValues)
{
	UINT nHeight = 100;
	HRESULT hr;

	SafeRelease(m_pWICBitmap);
	hr = m_pImageFactory->CreateBitmap(
		nSize,
		nHeight,
		GUID_WICPixelFormat8bppGray,
		WICBitmapCacheOnDemand,
		&m_pWICBitmap);

	if (FAILED(hr))
		return FALSE;

	WICRect rect;
	rect.X = 0;
	rect.Y = 0;
	rect.Width = nSize;
	rect.Height = nHeight;
	IWICBitmapLock* pWICBitmapLock;

	hr = m_pWICBitmap->Lock(
		&rect,
		WICBitmapLockWrite,
		&pWICBitmapLock);

	if (FAILED(hr))
		return FALSE;

	// Get access the bitmap
	UINT cbBufferSize = 0;
	UINT cbStride = 0;
	BYTE* pv = nullptr;

	pWICBitmapLock->GetStride(&cbStride);
	pWICBitmapLock->GetDataPointer(&cbBufferSize, &pv);
	memset(pv, 0x80, cbBufferSize);

	// Write a histogram.
	HistogramHelper histogram(&pv, cbStride);
	for (UINT i = 0; i < nSize; ++i) {
		histogram.SetValue(i, pValues[i]);
	}

	// Done.
	SafeRelease(pWICBitmapLock);

	// Create format converter
	SafeRelease(m_pFormatConverter);
	m_pImageFactory->CreateFormatConverter(&m_pFormatConverter);

	hr = m_pFormatConverter->Initialize(
		m_pWICBitmap,
		GUID_WICPixelFormat32bppPBGRA,
		WICBitmapDitherTypeNone,
		nullptr,
		0.f,
		WICBitmapPaletteTypeCustom);

	if (FAILED(hr))
		return FALSE;
	Invalidate();
	return TRUE;
}


void CDirect2DControl::RemoveBitmap()
{
	SafeRelease(m_pWICBitmap);
	Invalidate();
}


BOOL CDirect2DControl::DrawFrame(UINT n)
{
	if (n >= (UINT)m_pVideo->size()) {
		// Notify parent window
		if (IsPlaying())
			m_bPlaying = FALSE;
		if (m_pSecondarySoundBuffer)
			m_pSecondarySoundBuffer->Stop();

		GetParent()->PostMessage(
			UWM_END_OF_FRAME,
			(WPARAM)m_currentFrame,
			(LPARAM)this->GetSafeHwnd());

		return FALSE;
	}

	if (IsPlaying() && m_currentFrame == n)
		return TRUE;

	m_currentFrame = n;
	Image image = m_pVideo->frameAt(m_currentFrame);

	// Convert zero frame image to WICBitmap
	SafeRelease(m_pWICBitmap);
	HRESULT hr;
	hr = m_pImageFactory->CreateBitmapFromMemory(
		image.getWidth(),
		image.getHeight(),
		GUID_WICPixelFormat24bppBGR,
		image.getWidth() * 3 * sizeof(uchar),
		image.getWidth() * image.getHeight() * 3 * sizeof(uchar),
		image.data().data,
		&m_pWICBitmap);

	if (FAILED(hr))
		return FALSE;

	SafeRelease(m_pFormatConverter);
	m_pImageFactory->CreateFormatConverter(&m_pFormatConverter);

	hr = m_pFormatConverter->Initialize(
		m_pWICBitmap,
		GUID_WICPixelFormat32bppPBGRA,
		WICBitmapDitherTypeNone,
		nullptr,
		0.f,
		WICBitmapPaletteTypeCustom);

	if (FAILED(hr))
		return FALSE;
	Invalidate();

	return TRUE;
}


void CDirect2DControl::Play()
{
	if (IsPlaying()) {
		TerminateThread();
	}
	if (m_pSecondarySoundBuffer)
		m_pSecondarySoundBuffer->Stop();

	// Play video
	m_bPlaying = TRUE;
	m_startTime = chrono::high_resolution_clock::now();
	m_thread = thread(&CDirect2DControl::_animate, this);

	// Set priority
	HRESULT hr;
	hr = m_pDirectSound->SetCooperativeLevel(
		GetSafeHwnd(),
		DSSCL_NORMAL);

	// Play the contents of the secondary sound buffer.
	hr = m_pSecondarySoundBuffer->SetVolume(DSBVOLUME_MAX);
	hr = m_pSecondarySoundBuffer->Play(0, 0, 0);
	if (FAILED(hr))
		return;
}


void CDirect2DControl::Stop()
{
	if (IsPlaying()) {
		TerminateThread();
	}
	if (m_pSecondarySoundBuffer)
		m_pSecondarySoundBuffer->Stop();
	ChangeCurrentFrame(0);
}


void CDirect2DControl::Pause()
{
	if (IsPlaying()) {
		TerminateThread();
	}
	if (m_pSecondarySoundBuffer)
		m_pSecondarySoundBuffer->Stop();
}


CDirect2DControl::~CDirect2DControl()
{
	SafeRelease(m_pD2DBitmap);
	SafeRelease(m_pWICBitmap);
	SafeRelease(m_pFormatConverter);
	SafeRelease(m_pImageFactory);
	SafeRelease(m_pSecondarySoundBuffer);
	SafeRelease(m_pPrimarySoundBuffer);
	SafeRelease(m_pDirectSound);
	TerminateThread();
}


void CDirect2DControl::_animate()
{
	m_startFrame = m_currentFrame;
	while (m_bPlaying) {
		auto now = chrono::high_resolution_clock::now();
		long long msec =
			chrono::duration_cast<chrono::milliseconds>(now - m_startTime).count();
		DrawFrame(m_startFrame + (UINT)(msec / (1000 / 30.f)));
		Sleep(8);
	}
}

void CDirect2DControl::TerminateThread()
{
	if (m_bPlaying) {
		m_bPlaying = FALSE;
	}
	if (m_thread.joinable()) {
		m_thread.join();
	}
}


BEGIN_MESSAGE_MAP(CDirect2DControl, CStatic)
	ON_REGISTERED_MESSAGE(AFX_WM_DRAW2D, &CDirect2DControl::OnDraw2d)
END_MESSAGE_MAP()


// CDirect2DControl message handlers

afx_msg LRESULT CDirect2DControl::OnDraw2d(WPARAM wParam, LPARAM lParam)
{
	CHwndRenderTarget* pHwndRT = (CHwndRenderTarget*)lParam;
	ID2D1RenderTarget* pRT = pHwndRT->GetRenderTarget();

	// Clear window background
	HRESULT hr;
	pRT->Clear(ColorF(ColorF::Gray));

	if (m_pWICBitmap) {
		SafeRelease(m_pD2DBitmap);
		hr = pRT->CreateBitmapFromWicBitmap(
			m_pFormatConverter,
			nullptr,
			&m_pD2DBitmap);

		D2D_SIZE_F d2dSize = pRT->GetSize();
		D2D_RECT_F d2dRect = RectF(
			(FLOAT)0,
			(FLOAT)0,
			(FLOAT)d2dSize.width,
			(FLOAT)d2dSize.height);

		pRT->DrawBitmap(m_pD2DBitmap, &d2dRect);
	}

	// Notify parent window
	GetParent()->SendMessage(
		UWM_UPDATED_FRAME,
		(WPARAM)m_currentFrame,
		(LPARAM)this->GetSafeHwnd());

	return TRUE;
}
