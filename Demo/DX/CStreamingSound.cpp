#include "CStreamingSound.h"
#include "../GameConfig.h"

//-----------------------------------------------------------------------------
// Name: CStreamingSound::CStreamingSound()
// Desc: Setups up a buffer so data can be streamed from the wave file into 
//       a buffer.  This is very useful for large wav files that would take a 
//       while to load.  The buffer is initially filled with data, then 
//       as sound is played the notification events are signaled and more data
//       is written into the buffer by calling HandleWaveStreamNotification()
//-----------------------------------------------------------------------------
CStreamingSound::CStreamingSound(LPDIRECTSOUNDBUFFER pDSBuffer, DWORD dwDSBufferSize,
	CWaveFile* pWaveFile, DWORD dwNotifySize)
	: CSound(&pDSBuffer, dwDSBufferSize, 1, pWaveFile, 0)
{
	m_dwLastPlayPos = 0;
	m_dwPlayProgress = 0;
	m_dwNotifySize = dwNotifySize;
	m_dwNextWriteOffset = 0;
	m_bFillNextNotificationWithSilence = FALSE;
}




//-----------------------------------------------------------------------------
// Name: CStreamingSound::~CStreamingSound()
// Desc: Destroys the class
//-----------------------------------------------------------------------------
CStreamingSound::~CStreamingSound()
{
}



//-----------------------------------------------------------------------------
// Name: CStreamingSound::HandleWaveStreamNotification()
// Desc: Handle the notification that tells us to put more wav data in the 
//       circular buffer
//-----------------------------------------------------------------------------
HRESULT CStreamingSound::HandleWaveStreamNotification(BOOL bLoopedPlay)
{
	HRESULT hr;
	DWORD   dwCurrentPlayPos;
	DWORD   dwPlayDelta;
	DWORD   dwBytesWrittenToBuffer;
	VOID*   pDSLockedBuffer = NULL;
	VOID*   pDSLockedBuffer2 = NULL;
	DWORD   dwDSLockedBufferSize;
	DWORD   dwDSLockedBufferSize2;

	if (m_apDSBuffer == NULL || m_pWaveFile == NULL)
		return CO_E_NOTINITIALIZED;

	// Restore the buffer if it was lost
	BOOL bRestored;
	if (FAILED(hr = RestoreBuffer(m_apDSBuffer[0], &bRestored)))
		return DXTRACE_ERR(TEXT("RestoreBuffer"), hr);

	if (bRestored)
	{
		// The buffer was restored, so we need to fill it with new data
		if (FAILED(hr = FillBufferWithSound(m_apDSBuffer[0], FALSE)))
			return DXTRACE_ERR(TEXT("FillBufferWithSound"), hr);
		return S_OK;
	}

	// Lock the DirectSound buffer
	if (FAILED(hr = m_apDSBuffer[0]->Lock(m_dwNextWriteOffset, m_dwNotifySize,
		&pDSLockedBuffer, &dwDSLockedBufferSize,
		&pDSLockedBuffer2, &dwDSLockedBufferSize2, 0L)))
		return DXTRACE_ERR(TEXT("Lock"), hr);

	// m_dwDSBufferSize and m_dwNextWriteOffset are both multiples of m_dwNotifySize, 
	// it should the second buffer, so it should never be valid
	if (pDSLockedBuffer2 != NULL)
		return E_UNEXPECTED;

	if (!m_bFillNextNotificationWithSilence)
	{
		// Fill the DirectSound buffer with wav data
		if (FAILED(hr = m_pWaveFile->Read((BYTE*)pDSLockedBuffer,
			dwDSLockedBufferSize,
			&dwBytesWrittenToBuffer)))
			return DXTRACE_ERR(TEXT("Read"), hr);
	}
	else
	{
		// Fill the DirectSound buffer with silence
		FillMemory(pDSLockedBuffer, dwDSLockedBufferSize,
			(BYTE)(m_pWaveFile->m_pwfx->wBitsPerSample == 8 ? 128 : 0));
		dwBytesWrittenToBuffer = dwDSLockedBufferSize;
	}

	// If the number of bytes written is less than the 
	// amount we requested, we have a short file.
	if (dwBytesWrittenToBuffer < dwDSLockedBufferSize)
	{
		if (!bLoopedPlay)
		{
			// Fill in silence for the rest of the buffer.
			FillMemory((BYTE*)pDSLockedBuffer + dwBytesWrittenToBuffer,
				dwDSLockedBufferSize - dwBytesWrittenToBuffer,
				(BYTE)(m_pWaveFile->m_pwfx->wBitsPerSample == 8 ? 128 : 0));

			// Any future notifications should just fill the buffer with silence
			m_bFillNextNotificationWithSilence = TRUE;
		}
		else
		{
			// We are looping, so reset the file and fill the buffer with wav data
			DWORD dwReadSoFar = dwBytesWrittenToBuffer;    // From previous call above.
			while (dwReadSoFar < dwDSLockedBufferSize)
			{
				// This will keep reading in until the buffer is full (for very short files).
				if (FAILED(hr = m_pWaveFile->ResetFile()))
					return DXTRACE_ERR(TEXT("ResetFile"), hr);

				if (FAILED(hr = m_pWaveFile->Read((BYTE*)pDSLockedBuffer + dwReadSoFar,
					dwDSLockedBufferSize - dwReadSoFar,
					&dwBytesWrittenToBuffer)))
					return DXTRACE_ERR(TEXT("Read"), hr);

				dwReadSoFar += dwBytesWrittenToBuffer;
			}
		}
	}

	// Unlock the DirectSound buffer
	m_apDSBuffer[0]->Unlock(pDSLockedBuffer, dwDSLockedBufferSize, NULL, 0);

	// Figure out how much data has been played so far.  When we have played
	// past the end of the file, we will either need to start filling the
	// buffer with silence or starting reading from the beginning of the file, 
	// depending if the user wants to loop the sound
	if (FAILED(hr = m_apDSBuffer[0]->GetCurrentPosition(&dwCurrentPlayPos, NULL)))
		return DXTRACE_ERR(TEXT("GetCurrentPosition"), hr);

	// Check to see if the position counter looped
	if (dwCurrentPlayPos < m_dwLastPlayPos)
		dwPlayDelta = (m_dwDSBufferSize - m_dwLastPlayPos) + dwCurrentPlayPos;
	else
		dwPlayDelta = dwCurrentPlayPos - m_dwLastPlayPos;

	m_dwPlayProgress += dwPlayDelta;
	m_dwLastPlayPos = dwCurrentPlayPos;

	// If we are now filling the buffer with silence, then we have found the end so 
	// check to see if the entire sound has played, if it has then stop the buffer.
	if (m_bFillNextNotificationWithSilence)
	{
		// We don't want to cut off the sound before it's done playing.
		if (m_dwPlayProgress >= m_pWaveFile->GetSize())
		{
			m_apDSBuffer[0]->Stop();
		}
	}

	// Update where the buffer will lock (for next time)
	m_dwNextWriteOffset += dwDSLockedBufferSize;
	m_dwNextWriteOffset %= m_dwDSBufferSize; // Circular buffer

	return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CStreamingSound::Reset()
// Desc: Resets the sound so it will begin playing at the beginning
//-----------------------------------------------------------------------------
HRESULT CStreamingSound::Reset()
{
	HRESULT hr;

	if (m_apDSBuffer[0] == NULL || m_pWaveFile == NULL)
		return CO_E_NOTINITIALIZED;

	m_dwLastPlayPos = 0;
	m_dwPlayProgress = 0;
	m_dwNextWriteOffset = 0;
	m_bFillNextNotificationWithSilence = FALSE;

	// Restore the buffer if it was lost
	BOOL bRestored;
	if (FAILED(hr = RestoreBuffer(m_apDSBuffer[0], &bRestored)))
		return DXTRACE_ERR(TEXT("RestoreBuffer"), hr);

	if (bRestored)
	{
		// The buffer was restored, so we need to fill it with new data
		if (FAILED(hr = FillBufferWithSound(m_apDSBuffer[0], FALSE)))
			return DXTRACE_ERR(TEXT("FillBufferWithSound"), hr);
	}

	m_pWaveFile->ResetFile();

	return m_apDSBuffer[0]->SetCurrentPosition(0L);
}