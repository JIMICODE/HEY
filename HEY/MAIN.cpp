#include<Windows.h>
#include<iostream>
#include<time.h>
#include<d3d12.h>

#ifndef UNICODE
#define UNICODE
#endif // !UNICODE

//const std::string ProgramTitle = "Hello Windows";
const wchar_t title[] = L"Game Loop";
HWND window;
HDC device;
bool gameover = false;
RECT rect;

int x = 0;
int y = 0;
int speedx = 1;
int speedy = 1;

void DrawBitmap(const wchar_t filename[], int x, int y, const RECT rect)
{
	HBITMAP image = (HBITMAP)LoadImage(0, filename, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	HBRUSH hbrush = (HBRUSH)GetStockObject(BLACK_BRUSH);
	BITMAP bm;
	GetObject(image, sizeof(BITMAP), &bm);

	if (x <= rect.left)
	{
		x = rect.left;
		speedx = -speedx;
	}
	if (y <= rect.top)
	{
		y = rect.top;
		speedy = -speedy;
	}
	if (x >= rect.right - bm.bmWidth)
	{
		x = rect.right - bm.bmWidth;
		speedx = -speedx;
	}
	if (y >= rect.bottom - bm.bmHeight)
	{
		y = rect.bottom - bm.bmHeight;
		speedy = -speedy;
	}

	HDC hdcImage = CreateCompatibleDC(device);
	SelectObject(hdcImage, image);
	FillRect(device, &rect, hbrush);
	BitBlt(device,
		x, y,
		bm.bmWidth, bm.bmHeight,
		hdcImage,
		0, 0,
		SRCCOPY);

	DeleteDC(hdcImage);
	DeleteObject((HBITMAP)image);
}

bool Game_Init()
{
	return 1;
}

void Game_Run()
{
	if (gameover == true) return;

	x += speedx;
	y += speedy;

	DrawBitmap(L"666/c.bmp", x, y, rect);
}

void Game_End()
{
	ReleaseDC(window, device);
}

LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WinProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = title;
	wc.hIconSm = NULL;

	return RegisterClassEx(&wc);
}

bool InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	window = CreateWindow(
		title,
		title,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		640, 480,
		NULL, NULL,
		hInstance,
		NULL);

	if (window == 0)	return 0;

	ShowWindow(window, nCmdShow);
	UpdateWindow(window);

	device = GetDC(window);

	return 1;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int nCmdShow)
{
	MyRegisterClass(hInstance);

	if (!InitInstance(hInstance, nCmdShow)) return 0;
	if (!Game_Init()) return 0;

	MSG msg;

	GetClientRect(window, &rect);

	while (!gameover)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		Game_Run();
	}

	Game_End();


	return msg.wParam;
}

