#pragma once
#include "stdafx.h"

// CDirect2DControl

class CDirect2DControl : public CStatic
{
	DECLARE_DYNAMIC(CDirect2DControl)
private:
	// Graphic tools
	Video* m_pVideo;
	IWICImagingFactory* m_pImageFactory;
	IWICFormatConverter* m_pFormatConverter;
	IWICBitmap* m_pWICBitmap;
	ID2D1Bitmap* m_pD2DBitmap;
	UINT m_startFrame;
	UINT m_currentFrame;
	chrono::high_resolution_clock::time_point m_startTime;
	atomic<BOOL> m_bPlaying;
	thread m_thread;

	// Audio tools
	IDirectSound8* m_pDirectSound;
	IDirectSoundBuffer* m_pPrimarySoundBuffer;
	IDirectSoundBuffer8* m_pSecondarySoundBuffer;

	// Histogram tools
	vector<DOUBLE> m_histogramData;

public:
	CDirect2DControl();
	virtual ~CDirect2DControl();
	UINT GetCurrentFrame() const { return m_currentFrame; }
	BOOL IsPlaying() const { return m_bPlaying || m_thread.joinable(); }
	DWORD GetCurrAudioPos() const { return m_currentFrame * 5880; }
	DWORD GetAudioPos(UINT nFrame) const { return nFrame * 5880; }

	// Audio and graphics
	BOOL InsertVideo(Video* pVideo);
	BOOL ChangeCurrentFrame(UINT n);
	void Play();
	void Stop();
	void Pause();

	// Histogram
	BOOL DrawHistogram(UINT nSize, DOUBLE* pValues);
	void RemoveBitmap();

private:
	BOOL DrawFrame(UINT n);
	void TerminateThread();
	void _animate();
	BOOL LoadAudio(LPCSTR lpcPath);

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnDraw2d(WPARAM wParam, LPARAM lParam);
};


class HistogramHelper
{
private:
	BYTE** m_pv;
	UINT m_width;
	UINT m_height;

public:
	HistogramHelper(BYTE** pv, UINT cbStride)
		: m_pv(pv), m_width(cbStride), m_height(100) {}
	void SetPixel(UINT row, UINT col, BYTE color) {
		(*m_pv)[row * m_width + col] = color;
	}
	void SetValue(UINT col, DOUBLE dValue) {
		UINT start = 0;
		start = m_height - (UINT)dValue;
		for (UINT i = start; i < m_height; ++i) {
			SetPixel(i, col, 0x00);
		}
	}
};


template <typename T>
inline void SafeRelease(T *&p)
{
	if (nullptr != p)
	{
		p->Release();
		p = nullptr;
	}
}


typedef struct WaveHeaderType
{
	char chunkId[4];
	unsigned long chunkSize;
	char format[4];
	char subChunkId[4];
	unsigned long subChunkSize;
	unsigned short audioFormat;
	unsigned short numChannels;
	unsigned long sampleRate;
	unsigned long bytesPerSecond;
	unsigned short blockAlign;
	unsigned short bitsPerSample;
	char dataChunkId[4];
	unsigned long dataSize;
} WAVEHEADERTYPE;