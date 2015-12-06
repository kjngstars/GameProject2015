#pragma once

#include "CStreamingSound.h"

class CSoundManager
{
protected:
	LPDIRECTSOUND8 m_pDS;

public:
	CSoundManager();
	~CSoundManager();

	HRESULT Initialize(HWND hWnd, DWORD dwCoopLevel);
	inline  LPDIRECTSOUND8 GetDirectSound() { return m_pDS; }
	HRESULT SetPrimaryBufferFormat(DWORD dwPrimaryChannels, DWORD dwPrimaryFreq, DWORD dwPrimaryBitRate);
	HRESULT Get3DListenerInterface(LPDIRECTSOUND3DLISTENER* ppDSListener);

	HRESULT Create(CSound** ppSound, LPTSTR strWaveFileName, DWORD dwCreationFlags = 0, GUID guid3DAlgorithm = GUID_NULL, DWORD dwNumBuffers = 1);
	HRESULT CreateFromMemory(CSound** ppSound, BYTE* pbData, ULONG ulDataSize, LPWAVEFORMATEX pwfx, DWORD dwCreationFlags = 0, GUID guid3DAlgorithm = GUID_NULL, DWORD dwNumBuffers = 1);
	HRESULT CreateStreaming(CStreamingSound** ppStreamingSound, LPTSTR strWaveFileName, DWORD dwCreationFlags, GUID guid3DAlgorithm, DWORD dwNotifyCount, DWORD dwNotifySize, HANDLE hNotifyEvent);
};