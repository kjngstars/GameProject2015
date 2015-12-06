#include "CSoundManager.h"
#include "../GameConfig.h"

#define WAVEFILE_READ 1
#define WAVEFILE_WRITE 2

//-----------------------------------------------------------------------------
// Name: CSoundManager::CSoundManager()
// Desc: Constructs the class
//-----------------------------------------------------------------------------
CSoundManager::CSoundManager()
{
	m_pDS = NULL;
}




//-----------------------------------------------------------------------------
// Name: CSoundManager::~CSoundManager()
// Desc: Destroys the class
//-----------------------------------------------------------------------------
CSoundManager::~CSoundManager()
{
	SAFE_RELEASE(m_pDS);
}




//-----------------------------------------------------------------------------
// Name: CSoundManager::Initialize()
// Desc: Initializes the IDirectSound object and also sets the primary buffer
//       format.  This function must be called before any others.
//-----------------------------------------------------------------------------
HRESULT CSoundManager::Initialize(HWND  hWnd,
	DWORD dwCoopLevel)
{
	HRESULT             hr;

	SAFE_RELEASE(m_pDS);

	// Create IDirectSound using the primary sound device
	if (FAILED(hr = DirectSoundCreate8(NULL, &m_pDS, NULL)))
		return DXTRACE_ERR(TEXT("DirectSoundCreate8"), hr);

	// Set DirectSound coop level 
	if (FAILED(hr = m_pDS->SetCooperativeLevel(hWnd, dwCoopLevel)))
		return DXTRACE_ERR(TEXT("SetCooperativeLevel"), hr);

	return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CSoundManager::SetPrimaryBufferFormat()
// Desc: Set primary buffer to a specified format 
//       !WARNING! - Setting the primary buffer format and then using this
//                   same dsound object for DirectMusic messes up DirectMusic! 
//       For example, to set the primary buffer format to 22kHz stereo, 16-bit
//       then:   dwPrimaryChannels = 2
//               dwPrimaryFreq     = 22050, 
//               dwPrimaryBitRate  = 16
//-----------------------------------------------------------------------------
HRESULT CSoundManager::SetPrimaryBufferFormat(DWORD dwPrimaryChannels,
	DWORD dwPrimaryFreq,
	DWORD dwPrimaryBitRate)
{
	HRESULT             hr;
	LPDIRECTSOUNDBUFFER pDSBPrimary = NULL;

	if (m_pDS == NULL)
		return CO_E_NOTINITIALIZED;

	// Get the primary buffer 
	DSBUFFERDESC dsbd;
	ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
	dsbd.dwSize = sizeof(DSBUFFERDESC);
	dsbd.dwFlags = DSBCAPS_PRIMARYBUFFER;
	dsbd.dwBufferBytes = 0;
	dsbd.lpwfxFormat = NULL;

	if (FAILED(hr = m_pDS->CreateSoundBuffer(&dsbd, &pDSBPrimary, NULL)))
		return DXTRACE_ERR(TEXT("CreateSoundBuffer"), hr);

	WAVEFORMATEX wfx;
	ZeroMemory(&wfx, sizeof(WAVEFORMATEX));
	wfx.wFormatTag = (WORD)WAVE_FORMAT_PCM;
	wfx.nChannels = (WORD)dwPrimaryChannels;
	wfx.nSamplesPerSec = (DWORD)dwPrimaryFreq;
	wfx.wBitsPerSample = (WORD)dwPrimaryBitRate;
	wfx.nBlockAlign = (WORD)(wfx.wBitsPerSample / 8 * wfx.nChannels);
	wfx.nAvgBytesPerSec = (DWORD)(wfx.nSamplesPerSec * wfx.nBlockAlign);

	if (FAILED(hr = pDSBPrimary->SetFormat(&wfx)))
		return DXTRACE_ERR(TEXT("SetFormat"), hr);

	SAFE_RELEASE(pDSBPrimary);

	return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CSoundManager::Get3DListenerInterface()
// Desc: Returns the 3D listener interface associated with primary buffer.
//-----------------------------------------------------------------------------
HRESULT CSoundManager::Get3DListenerInterface(LPDIRECTSOUND3DLISTENER* ppDSListener)
{
	HRESULT             hr;
	DSBUFFERDESC        dsbdesc;
	LPDIRECTSOUNDBUFFER pDSBPrimary = NULL;

	if (ppDSListener == NULL)
		return E_INVALIDARG;
	if (m_pDS == NULL)
		return CO_E_NOTINITIALIZED;

	*ppDSListener = NULL;

	// Obtain primary buffer, asking it for 3D control
	ZeroMemory(&dsbdesc, sizeof(DSBUFFERDESC));
	dsbdesc.dwSize = sizeof(DSBUFFERDESC);
	dsbdesc.dwFlags = DSBCAPS_CTRL3D | DSBCAPS_PRIMARYBUFFER;
	if (FAILED(hr = m_pDS->CreateSoundBuffer(&dsbdesc, &pDSBPrimary, NULL)))
		return DXTRACE_ERR(TEXT("CreateSoundBuffer"), hr);

	if (FAILED(hr = pDSBPrimary->QueryInterface(IID_IDirectSound3DListener,
		(VOID**)ppDSListener)))
	{
		SAFE_RELEASE(pDSBPrimary);
		return DXTRACE_ERR(TEXT("QueryInterface"), hr);
	}

	// Release the primary buffer, since it is not need anymore
	SAFE_RELEASE(pDSBPrimary);

	return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CSoundManager::Create()
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CSoundManager::Create(CSound** ppSound,
	LPTSTR strWaveFileName,
	DWORD dwCreationFlags,
	GUID guid3DAlgorithm,
	DWORD dwNumBuffers)
{
	HRESULT hr;
	HRESULT hrRet = S_OK;
	DWORD   i;
	LPDIRECTSOUNDBUFFER* apDSBuffer = NULL;
	DWORD                dwDSBufferSize = NULL;
	CWaveFile*           pWaveFile = NULL;

	if (m_pDS == NULL)
		return CO_E_NOTINITIALIZED;
	if (strWaveFileName == NULL || ppSound == NULL || dwNumBuffers < 1)
		return E_INVALIDARG;

	apDSBuffer = new LPDIRECTSOUNDBUFFER[dwNumBuffers];
	if (apDSBuffer == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto LFail;
	}

	pWaveFile = new CWaveFile();
	if (pWaveFile == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto LFail;
	}

	pWaveFile->Open(strWaveFileName, NULL, WAVEFILE_READ);

	if (pWaveFile->GetSize() == 0)
	{
		// Wave is blank, so don't create it.
		hr = E_FAIL;
		goto LFail;
	}

	// Make the DirectSound buffer the same size as the wav file
	dwDSBufferSize = pWaveFile->GetSize();

	// Create the direct sound buffer, and only request the flags needed
	// since each requires some overhead and limits if the buffer can 
	// be hardware accelerated
	DSBUFFERDESC dsbd;
	ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
	dsbd.dwSize = sizeof(DSBUFFERDESC);
	dsbd.dwFlags = dwCreationFlags;
	dsbd.dwBufferBytes = dwDSBufferSize;
	dsbd.guid3DAlgorithm = guid3DAlgorithm;
	dsbd.lpwfxFormat = pWaveFile->m_pwfx;

	// DirectSound is only guarenteed to play PCM data.  Other
	// formats may or may not work depending the sound card driver.
	hr = m_pDS->CreateSoundBuffer(&dsbd, &apDSBuffer[0], NULL);

	// Be sure to return this error code if it occurs so the
	// callers knows this happened.
	if (hr == DS_NO_VIRTUALIZATION)
		hrRet = DS_NO_VIRTUALIZATION;

	if (FAILED(hr))
	{
		// DSERR_BUFFERTOOSMALL will be returned if the buffer is
		// less than DSBSIZE_FX_MIN and the buffer is created
		// with DSBCAPS_CTRLFX.

		// It might also fail if hardware buffer mixing was requested
		// on a device that doesn't support it.
		DXTRACE_ERR(TEXT("CreateSoundBuffer"), hr);

		goto LFail;
	}

	// Default to use DuplicateSoundBuffer() when created extra buffers since always 
	// create a buffer that uses the same memory however DuplicateSoundBuffer() will fail if 
	// DSBCAPS_CTRLFX is used, so use CreateSoundBuffer() instead in this case.
	if ((dwCreationFlags & DSBCAPS_CTRLFX) == 0)
	{
		for (i = 1; i < dwNumBuffers; i++)
		{
			if (FAILED(hr = m_pDS->DuplicateSoundBuffer(apDSBuffer[0], &apDSBuffer[i])))
			{
				DXTRACE_ERR(TEXT("DuplicateSoundBuffer"), hr);
				goto LFail;
			}
		}
	}
	else
	{
		for (i = 1; i < dwNumBuffers; i++)
		{
			hr = m_pDS->CreateSoundBuffer(&dsbd, &apDSBuffer[i], NULL);
			if (FAILED(hr))
			{
				DXTRACE_ERR(TEXT("CreateSoundBuffer"), hr);
				goto LFail;
			}
		}
	}

	// Create the sound
	*ppSound = new CSound(apDSBuffer, dwDSBufferSize, dwNumBuffers, pWaveFile, dwCreationFlags);

	SAFE_DELETE_ARRAY(apDSBuffer);
	return hrRet;

LFail:
	// Cleanup
	SAFE_DELETE(pWaveFile);
	SAFE_DELETE_ARRAY(apDSBuffer);
	return hr;
}









//-----------------------------------------------------------------------------
// Name: CSoundManager::CreateFromMemory()
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CSoundManager::CreateFromMemory(CSound** ppSound,
	BYTE* pbData,
	ULONG  ulDataSize,
	LPWAVEFORMATEX pwfx,
	DWORD dwCreationFlags,
	GUID guid3DAlgorithm,
	DWORD dwNumBuffers)
{
	HRESULT hr;
	DWORD   i;
	LPDIRECTSOUNDBUFFER* apDSBuffer = NULL;
	DWORD                dwDSBufferSize = NULL;
	CWaveFile*           pWaveFile = NULL;

	if (m_pDS == NULL)
		return CO_E_NOTINITIALIZED;
	if (pbData == NULL || ppSound == NULL || dwNumBuffers < 1)
		return E_INVALIDARG;

	apDSBuffer = new LPDIRECTSOUNDBUFFER[dwNumBuffers];
	if (apDSBuffer == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto LFail;
	}

	pWaveFile = new CWaveFile();
	if (pWaveFile == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto LFail;
	}

	pWaveFile->OpenFromMemory(pbData, ulDataSize, pwfx, WAVEFILE_READ);


	// Make the DirectSound buffer the same size as the wav file
	dwDSBufferSize = ulDataSize;

	// Create the direct sound buffer, and only request the flags needed
	// since each requires some overhead and limits if the buffer can 
	// be hardware accelerated
	DSBUFFERDESC dsbd;
	ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
	dsbd.dwSize = sizeof(DSBUFFERDESC);
	dsbd.dwFlags = dwCreationFlags;
	dsbd.dwBufferBytes = dwDSBufferSize;
	dsbd.guid3DAlgorithm = guid3DAlgorithm;
	dsbd.lpwfxFormat = pwfx;

	if (FAILED(hr = m_pDS->CreateSoundBuffer(&dsbd, &apDSBuffer[0], NULL)))
	{
		DXTRACE_ERR(TEXT("CreateSoundBuffer"), hr);
		goto LFail;
	}

	// Default to use DuplicateSoundBuffer() when created extra buffers since always 
	// create a buffer that uses the same memory however DuplicateSoundBuffer() will fail if 
	// DSBCAPS_CTRLFX is used, so use CreateSoundBuffer() instead in this case.
	if ((dwCreationFlags & DSBCAPS_CTRLFX) == 0)
	{
		for (i = 1; i < dwNumBuffers; i++)
		{
			if (FAILED(hr = m_pDS->DuplicateSoundBuffer(apDSBuffer[0], &apDSBuffer[i])))
			{
				DXTRACE_ERR(TEXT("DuplicateSoundBuffer"), hr);
				goto LFail;
			}
		}
	}
	else
	{
		for (i = 1; i < dwNumBuffers; i++)
		{
			hr = m_pDS->CreateSoundBuffer(&dsbd, &apDSBuffer[i], NULL);
			if (FAILED(hr))
			{
				DXTRACE_ERR(TEXT("CreateSoundBuffer"), hr);
				goto LFail;
			}
		}
	}

	// Create the sound
	*ppSound = new CSound(apDSBuffer, dwDSBufferSize, dwNumBuffers, pWaveFile, dwCreationFlags);

	SAFE_DELETE_ARRAY(apDSBuffer);
	return S_OK;

LFail:
	// Cleanup

	SAFE_DELETE_ARRAY(apDSBuffer);
	return hr;
}





//-----------------------------------------------------------------------------
// Name: CSoundManager::CreateStreaming()
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CSoundManager::CreateStreaming(CStreamingSound** ppStreamingSound,
	LPTSTR strWaveFileName,
	DWORD dwCreationFlags,
	GUID guid3DAlgorithm,
	DWORD dwNotifyCount,
	DWORD dwNotifySize,
	HANDLE hNotifyEvent)
{
	HRESULT hr;

	if (m_pDS == NULL)
		return CO_E_NOTINITIALIZED;
	if (strWaveFileName == NULL || ppStreamingSound == NULL || hNotifyEvent == NULL)
		return E_INVALIDARG;

	LPDIRECTSOUNDBUFFER pDSBuffer = NULL;
	DWORD               dwDSBufferSize = NULL;
	CWaveFile*          pWaveFile = NULL;
	DSBPOSITIONNOTIFY*  aPosNotify = NULL;
	LPDIRECTSOUNDNOTIFY pDSNotify = NULL;

	pWaveFile = new CWaveFile();
	if (pWaveFile == NULL)
		return E_OUTOFMEMORY;
	pWaveFile->Open(strWaveFileName, NULL, WAVEFILE_READ);

	// Figure out how big the DSound buffer should be 
	dwDSBufferSize = dwNotifySize * dwNotifyCount;

	// Set up the direct sound buffer.  Request the NOTIFY flag, so
	// that we are notified as the sound buffer plays.  Note, that using this flag
	// may limit the amount of hardware acceleration that can occur. 
	DSBUFFERDESC dsbd;
	ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
	dsbd.dwSize = sizeof(DSBUFFERDESC);
	dsbd.dwFlags = dwCreationFlags |
		DSBCAPS_CTRLPOSITIONNOTIFY |
		DSBCAPS_GETCURRENTPOSITION2;
	dsbd.dwBufferBytes = dwDSBufferSize;
	dsbd.guid3DAlgorithm = guid3DAlgorithm;
	dsbd.lpwfxFormat = pWaveFile->m_pwfx;

	if (FAILED(hr = m_pDS->CreateSoundBuffer(&dsbd, &pDSBuffer, NULL)))
	{
		// If wave format isn't then it will return 
		// either DSERR_BADFORMAT or E_INVALIDARG
		if (hr == DSERR_BADFORMAT || hr == E_INVALIDARG)
			return DXTRACE_ERR(TEXT("CreateSoundBuffer"), hr);

		return DXTRACE_ERR(TEXT("CreateSoundBuffer"), hr);
	}

	// Create the notification events, so that we know when to fill
	// the buffer as the sound plays. 
	if (FAILED(hr = pDSBuffer->QueryInterface(IID_IDirectSoundNotify,
		(VOID**)&pDSNotify)))
	{
		SAFE_DELETE_ARRAY(aPosNotify);
		return DXTRACE_ERR(TEXT("QueryInterface"), hr);
	}

	aPosNotify = new DSBPOSITIONNOTIFY[dwNotifyCount];
	if (aPosNotify == NULL)
		return E_OUTOFMEMORY;

	for (DWORD i = 0; i < dwNotifyCount; i++)
	{
		aPosNotify[i].dwOffset = (dwNotifySize * i) + dwNotifySize - 1;
		aPosNotify[i].hEventNotify = hNotifyEvent;
	}

	// Tell DirectSound when to notify us. The notification will come in the from 
	// of signaled events that are handled in WinMain()
	if (FAILED(hr = pDSNotify->SetNotificationPositions(dwNotifyCount,
		aPosNotify)))
	{
		SAFE_RELEASE(pDSNotify);
		SAFE_DELETE_ARRAY(aPosNotify);
		return DXTRACE_ERR(TEXT("SetNotificationPositions"), hr);
	}

	SAFE_RELEASE(pDSNotify);
	SAFE_DELETE_ARRAY(aPosNotify);

	// Create the sound
	*ppStreamingSound = new CStreamingSound(pDSBuffer, dwDSBufferSize, pWaveFile, dwNotifySize);

	return S_OK;
}
