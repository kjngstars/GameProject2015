#include <time.h>
#include "CGraphics.h"
#include"CGame.h"

bool gameover = false;
CGame game;
CDXInput inputDevice;

LRESULT WINAPI WinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_ACTIVATE:
		(LOWORD(wParam) == WA_INACTIVE) ?
			game.isPaused = true : game.isPaused = false;
		return 0;

	case WM_SIZE:
		if (wParam == SIZE_MINIMIZED)
			game.isPaused = true;
		return 0;

	case WM_SYSKEYDOWN:				//alt
		if (wParam == VK_F4 && (HIWORD(lParam) & KF_ALTDOWN))
		{
			gameover = true;
			PostQuitMessage(0);
			return 0;
		}

		if (wParam == VK_RETURN && (HIWORD(lParam) & KF_ALTDOWN))
			game.ToggleFullScreen();
		return 0;

	case WM_DESTROY:
		gameover = true;
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	HANDLE hMutex = OpenMutex(MUTEX_ALL_ACCESS, 0, APP_TITLE);
	if (!hMutex)
		CreateMutex(0, 0, APP_TITLE);
	else
		return 0;

	//initialize window settings
	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = (WNDPROC)WinProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = APP_TITLE;
	wc.hIconSm = NULL;

	if (!RegisterClassEx(&wc))
	{
		MessageBox(0, "RegisterClass FAILED", 0, 0);
		PostQuitMessage(0);
	}

	//create a new window
	HWND window = CreateWindow(
		APP_TITLE, APP_TITLE,
		WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX,
		CW_USEDEFAULT, CW_USEDEFAULT,
		SCREEN_WIDTH, SCREEN_HEIGHT, NULL, NULL, hInstance, NULL);

	if (window == 0)
	{
		MessageBox(0, "CreateWindow FAILED", 0, 0);
		PostQuitMessage(0);
	}

	new CGraphics(hInstance, window);

	//display the window
	ShowWindow(window, SW_SHOW);
	UpdateWindow(window);

	game.GameInit();
	inputDevice.Initialize(
		CGraphics::GetInstancePtr()->GetHInst(),
		CGraphics::GetInstancePtr()->GetWND());

	// main message loop
	MSG message;

	float time_per_frame = 1000.0f / FPS;

	LARGE_INTEGER frequency;
	LARGE_INTEGER start;
	LARGE_INTEGER stop;
	LARGE_INTEGER elapsedTime;

	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&start);

	float dt = 0.0f;

	while (!gameover)
	{
		if (PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&message);
			DispatchMessage(&message);
		}

		if (!game.IsLostDevice())
		{
			QueryPerformanceCounter(&stop);
			elapsedTime.QuadPart = stop.QuadPart - start.QuadPart;

			dt += (float)elapsedTime.QuadPart * 1000.0f /
				(float)frequency.QuadPart;
			inputDevice.Update();

			if (dt >= time_per_frame)
			{
				game.Update(dt, &inputDevice);
				game.Render();

				dt = 0.0f;
				inputDevice.Change();
			}

			start = stop;
		}
	}

	//shutdown
	delete CGraphics::GetInstancePtr();

	return message.wParam;
}