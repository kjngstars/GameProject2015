#pragma once

#include "CSoundManager.h"

//primary DirectSound object
extern CSoundManager *dsound;

//function prototypes
int Init_DirectSound(HWND);
CSound *LoadSound(char *);
void PlaySound(CSound *);
void LoopSound(CSound *);
void StopSound(CSound *);
