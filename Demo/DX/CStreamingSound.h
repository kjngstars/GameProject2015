#pragma once

#include "CSound.h"

class CStreamingSound : public CSound
{
protected:
	DWORD m_dwLastPlayPos;
	DWORD m_dwPlayProgress;
	DWORD m_dwNotifySize;
	DWORD m_dwNextWriteOffset;
	BOOL  m_bFillNextNotificationWithSilence;

public:
	CStreamingSound(LPDIRECTSOUNDBUFFER pDSBuffer, DWORD dwDSBufferSize, CWaveFile* pWaveFile, DWORD dwNotifySize);
	~CStreamingSound();

	HRESULT HandleWaveStreamNotification(BOOL bLoopedPlay);
	HRESULT Reset();
};