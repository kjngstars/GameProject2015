#pragma once
 
#include <dsound.h>
#include "CWaveFile.h"

class CSound
{
protected:
	LPDIRECTSOUNDBUFFER* m_apDSBuffer;
	DWORD                m_dwDSBufferSize;
	CWaveFile*           m_pWaveFile;
	DWORD                m_dwNumBuffers;
	DWORD                m_dwCreationFlags;

	HRESULT RestoreBuffer(LPDIRECTSOUNDBUFFER pDSB, BOOL* pbWasRestored);

public:
	CSound(LPDIRECTSOUNDBUFFER* apDSBuffer, DWORD dwDSBufferSize, DWORD dwNumBuffers, CWaveFile* pWaveFile, DWORD dwCreationFlags);
	virtual ~CSound();

	HRESULT Get3DBufferInterface(DWORD dwIndex, LPDIRECTSOUND3DBUFFER* ppDS3DBuffer);
	HRESULT FillBufferWithSound(LPDIRECTSOUNDBUFFER pDSB, BOOL bRepeatWavIfBufferLarger);
	LPDIRECTSOUNDBUFFER GetFreeBuffer();
	LPDIRECTSOUNDBUFFER GetBuffer(DWORD dwIndex);

	HRESULT Play(DWORD dwPriority = 0, DWORD dwFlags = 0, LONG lVolume = 0, LONG lFrequency = -1, LONG lPan = 0);
	HRESULT Play3D(LPDS3DBUFFER p3DBuffer, DWORD dwPriority = 0, DWORD dwFlags = 0, LONG lFrequency = 0);
	HRESULT Stop();
	HRESULT Reset();
	BOOL    IsSoundPlaying();
};