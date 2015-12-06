#include "CSound.h"
#include "../GameConfig.h"

CSound::CSound(LPDIRECTSOUNDBUFFER* apDSBuffer, DWORD dwDSBufferSize,
	DWORD dwNumBuffers, CWaveFile* pWaveFile, DWORD dwCreationFlags)
{
	DWORD i;

	m_apDSBuffer = new LPDIRECTSOUNDBUFFER[dwNumBuffers];
	if (NULL != m_apDSBuffer)
	{
		for (i = 0; i < dwNumBuffers; i++)
			m_apDSBuffer[i] = apDSBuffer[i];

		m_dwDSBufferSize = dwDSBufferSize;
		m_dwNumBuffers = dwNumBuffers;
		m_pWaveFile = pWaveFile;
		m_dwCreationFlags = dwCreationFlags;

		FillBufferWithSound(m_apDSBuffer[0], FALSE);
	}
}




//-----------------------------------------------------------------------------
// Name: CSound::~CSound()
// Desc: Destroys the class
//-----------------------------------------------------------------------------
CSound::~CSound()
{
	for (DWORD i = 0; i < m_dwNumBuffers; i++)
	{
		SAFE_RELEASE(m_apDSBuffer[i]);
	}

	SAFE_DELETE_ARRAY(m_apDSBuffer);
	SAFE_DELETE(m_pWaveFile);
}




//-----------------------------------------------------------------------------
// Name: CSound::FillBufferWithSound()
// Desc: Fills a DirectSound buffer with a sound file 
//-----------------------------------------------------------------------------
HRESULT CSound::FillBufferWithSound(LPDIRECTSOUNDBUFFER pDSB, BOOL bRepeatWavIfBufferLarger)
{
	HRESULT hr;
	VOID*   pDSLockedBuffer = NULL; // Pointer to locked buffer memory
	DWORD   dwDSLockedBufferSize = 0;    // Size of the locked DirectSound buffer
	DWORD   dwWavDataRead = 0;    // Amount of data read from the wav file 

	if (pDSB == NULL)
		return CO_E_NOTINITIALIZED;

	// Make sure we have focus, and we didn't just switch in from
	// an app which had a DirectSound device
	if (FAILED(hr = RestoreBuffer(pDSB, NULL)))
		return DXTRACE_ERR(TEXT("RestoreBuffer"), hr);

	// Lock the buffer down
	if (FAILED(hr = pDSB->Lock(0, m_dwDSBufferSize,
		&pDSLockedBuffer, &dwDSLockedBufferSize,
		NULL, NULL, 0L)))
		return DXTRACE_ERR(TEXT("Lock"), hr);

	// Reset the wave file to the beginning 
	m_pWaveFile->ResetFile();

	if (FAILED(hr = m_pWaveFile->Read((BYTE*)pDSLockedBuffer,
		dwDSLockedBufferSize,
		&dwWavDataRead)))
		return DXTRACE_ERR(TEXT("Read"), hr);

	if (dwWavDataRead == 0)
	{
		// Wav is blank, so just fill with silence
		FillMemory((BYTE*)pDSLockedBuffer,
			dwDSLockedBufferSize,
			(BYTE)(m_pWaveFile->m_pwfx->wBitsPerSample == 8 ? 128 : 0));
	}
	else if (dwWavDataRead < dwDSLockedBufferSize)
	{
		// If the wav file was smaller than the DirectSound buffer, 
		// we need to fill the remainder of the buffer with data 
		if (bRepeatWavIfBufferLarger)
		{
			// Reset the file and fill the buffer with wav data
			DWORD dwReadSoFar = dwWavDataRead;    // From previous call above.
			while (dwReadSoFar < dwDSLockedBufferSize)
			{
				// This will keep reading in until the buffer is full 
				// for very short files
				if (FAILED(hr = m_pWaveFile->ResetFile()))
					return DXTRACE_ERR(TEXT("ResetFile"), hr);

				hr = m_pWaveFile->Read((BYTE*)pDSLockedBuffer + dwReadSoFar,
					dwDSLockedBufferSize - dwReadSoFar,
					&dwWavDataRead);
				if (FAILED(hr))
					return DXTRACE_ERR(TEXT("Read"), hr);

				dwReadSoFar += dwWavDataRead;
			}
		}
		else
		{
			// Don't repeat the wav file, just fill in silence 
			FillMemory((BYTE*)pDSLockedBuffer + dwWavDataRead,
				dwDSLockedBufferSize - dwWavDataRead,
				(BYTE)(m_pWaveFile->m_pwfx->wBitsPerSample == 8 ? 128 : 0));
		}
	}

	// Unlock the buffer, we don't need it anymore.
	pDSB->Unlock(pDSLockedBuffer, dwDSLockedBufferSize, NULL, 0);

	return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CSound::RestoreBuffer()
// Desc: Restores the lost buffer. *pbWasRestored returns TRUE if the buffer was 
//       restored.  It can also NULL if the information is not needed.
//-----------------------------------------------------------------------------
HRESULT CSound::RestoreBuffer(LPDIRECTSOUNDBUFFER pDSB, BOOL* pbWasRestored)
{
	HRESULT hr;

	if (pDSB == NULL)
		return CO_E_NOTINITIALIZED;
	if (pbWasRestored)
		*pbWasRestored = FALSE;

	DWORD dwStatus;
	if (FAILED(hr = pDSB->GetStatus(&dwStatus)))
		return DXTRACE_ERR(TEXT("GetStatus"), hr);

	if (dwStatus & DSBSTATUS_BUFFERLOST)
	{
		// Since the app could have just been activated, then
		// DirectSound may not be giving us control yet, so 
		// the restoring the buffer may fail.  
		// If it does, sleep until DirectSound gives us control.
		do
		{
			hr = pDSB->Restore();
			if (hr == DSERR_BUFFERLOST)
				Sleep(10);
		} while ((hr = pDSB->Restore()) == DSERR_BUFFERLOST);

		if (pbWasRestored != NULL)
			*pbWasRestored = TRUE;

		return S_OK;
	}
	else
	{
		return S_FALSE;
	}
}




//-----------------------------------------------------------------------------
// Name: CSound::GetFreeBuffer()
// Desc: Finding the first buffer that is not playing and return a pointer to 
//       it, or if all are playing return a pointer to a randomly selected buffer.
//-----------------------------------------------------------------------------
LPDIRECTSOUNDBUFFER CSound::GetFreeBuffer()
{
	if (m_apDSBuffer == NULL)
		return FALSE;
	DWORD i = 0;
	for (i = 0; i < m_dwNumBuffers; i++)
	{
		if (m_apDSBuffer[i])
		{
			DWORD dwStatus = 0;
			m_apDSBuffer[i]->GetStatus(&dwStatus);
			if ((dwStatus & DSBSTATUS_PLAYING) == 0)
				break;
		}
	}

	if (i != m_dwNumBuffers)
		return m_apDSBuffer[i];
	else
		return m_apDSBuffer[rand() % m_dwNumBuffers];
}




//-----------------------------------------------------------------------------
// Name: CSound::GetBuffer()
// Desc: 
//-----------------------------------------------------------------------------
LPDIRECTSOUNDBUFFER CSound::GetBuffer(DWORD dwIndex)
{
	if (m_apDSBuffer == NULL)
		return NULL;
	if (dwIndex >= m_dwNumBuffers)
		return NULL;

	return m_apDSBuffer[dwIndex];
}




//-----------------------------------------------------------------------------
// Name: CSound::Get3DBufferInterface()
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CSound::Get3DBufferInterface(DWORD dwIndex, LPDIRECTSOUND3DBUFFER* ppDS3DBuffer)
{
	if (m_apDSBuffer == NULL)
		return CO_E_NOTINITIALIZED;
	if (dwIndex >= m_dwNumBuffers)
		return E_INVALIDARG;

	*ppDS3DBuffer = NULL;

	return m_apDSBuffer[dwIndex]->QueryInterface(IID_IDirectSound3DBuffer,
		(VOID**)ppDS3DBuffer);
}


//-----------------------------------------------------------------------------
// Name: CSound::Play()
// Desc: Plays the sound using voice management flags.  Pass in DSBPLAY_LOOPING
//       in the dwFlags to loop the sound
//-----------------------------------------------------------------------------
HRESULT CSound::Play(DWORD dwPriority, DWORD dwFlags, LONG lVolume, LONG lFrequency, LONG lPan)
{
	HRESULT hr;
	BOOL    bRestored;

	if (m_apDSBuffer == NULL)
		return CO_E_NOTINITIALIZED;

	LPDIRECTSOUNDBUFFER pDSB = GetFreeBuffer();

	if (pDSB == NULL)
		return DXTRACE_ERR(TEXT("GetFreeBuffer"), E_FAIL);

	// Restore the buffer if it was lost
	if (FAILED(hr = RestoreBuffer(pDSB, &bRestored)))
		return DXTRACE_ERR(TEXT("RestoreBuffer"), hr);

	if (bRestored)
	{
		// The buffer was restored, so we need to fill it with new data
		if (FAILED(hr = FillBufferWithSound(pDSB, FALSE)))
			return DXTRACE_ERR(TEXT("FillBufferWithSound"), hr);
	}

	if (m_dwCreationFlags & DSBCAPS_CTRLVOLUME)
	{
		pDSB->SetVolume(lVolume);
	}

	if (lFrequency != -1 &&
		(m_dwCreationFlags & DSBCAPS_CTRLFREQUENCY))
	{
		pDSB->SetFrequency(lFrequency);
	}

	if (m_dwCreationFlags & DSBCAPS_CTRLPAN)
	{
		pDSB->SetPan(lPan);
	}

	return pDSB->Play(0, dwPriority, dwFlags);
}




//-----------------------------------------------------------------------------
// Name: CSound::Play3D()
// Desc: Plays the sound using voice management flags.  Pass in DSBPLAY_LOOPING
//       in the dwFlags to loop the sound
//-----------------------------------------------------------------------------
HRESULT CSound::Play3D(LPDS3DBUFFER p3DBuffer, DWORD dwPriority, DWORD dwFlags, LONG lFrequency)
{
	HRESULT hr;
	BOOL    bRestored;
	DWORD   dwBaseFrequency;

	if (m_apDSBuffer == NULL)
		return CO_E_NOTINITIALIZED;

	LPDIRECTSOUNDBUFFER pDSB = GetFreeBuffer();
	if (pDSB == NULL)
		return DXTRACE_ERR(TEXT("GetFreeBuffer"), E_FAIL);

	// Restore the buffer if it was lost
	if (FAILED(hr = RestoreBuffer(pDSB, &bRestored)))
		return DXTRACE_ERR(TEXT("RestoreBuffer"), hr);

	if (bRestored)
	{
		// The buffer was restored, so we need to fill it with new data
		if (FAILED(hr = FillBufferWithSound(pDSB, FALSE)))
			return DXTRACE_ERR(TEXT("FillBufferWithSound"), hr);
	}

	if (m_dwCreationFlags & DSBCAPS_CTRLFREQUENCY)
	{
		pDSB->GetFrequency(&dwBaseFrequency);
		pDSB->SetFrequency(dwBaseFrequency + lFrequency);
	}

	// QI for the 3D buffer 
	LPDIRECTSOUND3DBUFFER pDS3DBuffer;
	hr = pDSB->QueryInterface(IID_IDirectSound3DBuffer, (VOID**)&pDS3DBuffer);
	if (SUCCEEDED(hr))
	{
		hr = pDS3DBuffer->SetAllParameters(p3DBuffer, DS3D_IMMEDIATE);
		if (SUCCEEDED(hr))
		{
			hr = pDSB->Play(0, dwPriority, dwFlags);
		}

		pDS3DBuffer->Release();
	}

	return hr;
}



//-----------------------------------------------------------------------------
// Name: CSound::Stop()
// Desc: Stops the sound from playing
//-----------------------------------------------------------------------------
HRESULT CSound::Stop()
{
	if (m_apDSBuffer == NULL)
		return CO_E_NOTINITIALIZED;

	HRESULT hr = 0;

	for (DWORD i = 0; i < m_dwNumBuffers; i++)
		hr |= m_apDSBuffer[i]->Stop();

	return hr;
}




//-----------------------------------------------------------------------------
// Name: CSound::Reset()
// Desc: Reset all of the sound buffers
//-----------------------------------------------------------------------------
HRESULT CSound::Reset()
{
	if (m_apDSBuffer == NULL)
		return CO_E_NOTINITIALIZED;

	HRESULT hr = 0;

	for (DWORD i = 0; i < m_dwNumBuffers; i++)
		hr |= m_apDSBuffer[i]->SetCurrentPosition(0);

	return hr;
}




//-----------------------------------------------------------------------------
// Name: CSound::IsSoundPlaying()
// Desc: Checks to see if a buffer is playing and returns TRUE if it is.
//-----------------------------------------------------------------------------
BOOL CSound::IsSoundPlaying()
{
	BOOL bIsPlaying = FALSE;

	if (m_apDSBuffer == NULL)
		return FALSE;

	for (DWORD i = 0; i < m_dwNumBuffers; i++)
	{
		if (m_apDSBuffer[i])
		{
			DWORD dwStatus = 0;
			m_apDSBuffer[i]->GetStatus(&dwStatus);
			bIsPlaying |= ((dwStatus & DSBSTATUS_PLAYING) != 0);
		}
	}

	return bIsPlaying;
}