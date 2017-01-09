#include<Windows.h>
#include<d3d9.h>
#include<time.h>
#include<d3dx9.h>
#include<iostream>
using namespace std;

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

const string APPTITLE = "Direcet3D_Windowed";
const wchar_t title[] = L"Create Surface Program";
const int SCREENW = 1024;
const int SCREENH = 769;

LPDIRECT3D9 d3d = NULL;
LPDIRECT3DDEVICE9 d3ddev = NULL;
LPDIRECT3DSURFACE9 backbuffer = NULL;
LPDIRECT3DSURFACE9 surface = NULL;
bool gameover = false;

#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)

bool Game_Init(HWND hwnd)
{
	d3d = Direct3DCreate9(D3D_SDK_VERSION);

	if (d3d == NULL)
	{
		MessageBox(hwnd, L"Error initializing Direct3D", L"Error", MB_OK);
		return false;
	}

	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));

	d3dpp.Windowed = true;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	d3dpp.BackBufferCount = 1;
	d3dpp.BackBufferWidth = SCREENW;
	d3dpp.BackBufferHeight = SCREENH;
	d3dpp.hDeviceWindow = hwnd;

	d3d->CreateDevice(
		D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		hwnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp,
		&d3ddev);

	if (d3ddev == NULL)
	{
		MessageBox(hwnd, L"Error creating Direct3D device", L"Error", MB_OK);
		return FALSE;
	}

	srand((unsigned int)time(NULL));
	d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 255, 255), 1.0f, 0);
	d3ddev->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backbuffer);

	HRESULT result = d3ddev->CreateOffscreenPlainSurface(
		100,
		100,
		D3DFMT_X8R8G8B8,
		D3DPOOL_DEFAULT,
		&surface,
		NULL
	);

	if (result != D3D_OK)	return false;
	return true;
}

void Game_Run(HWND hwnd)
{
	if (d3ddev == NULL)
	{
		MessageBox(hwnd, L"D3Ddevice is NULL", L"D3Ddevice is NULL", NULL);
		return;
	}

	if (d3ddev->BeginScene())
	{
		int r = rand() % 255;
		int g = rand() % 255;
		int b = rand() % 255;

		d3ddev->ColorFill(surface, NULL, D3DCOLOR_XRGB(r, g, b));

		RECT rect;
		rect.left = rand() % SCREENW;
		rect.right = rect.left + rand() % SCREENW / 2;
		rect.top = rand() % SCREENH;
		rect.bottom = rect.top + rand() % SCREENH / 2;

		d3ddev->StretchRect(surface, NULL, backbuffer, &rect, D3DTEXF_NONE);
		d3ddev->EndScene();
		d3ddev->Present(NULL, NULL, NULL, NULL);
	}
	

	if (KEY_DOWN(VK_ESCAPE))
	{
		MessageBox(hwnd, L"ESC", L"ESC", NULL);
		PostMessage(hwnd, WM_DESTROY, 0, 0);
	}

}

void Game_End(HWND hwnd)
{
	if (d3ddev)
	{
		d3ddev->Release();
		d3ddev = NULL;
	}
	if (d3d)
	{
		d3d->Release();
		d3d = NULL;
	}
	if (surface) surface->Release();
}

LRESULT WINAPI WinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		gameover = true;
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEX wc;
	MSG msg;

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.lpfnWndProc = (WNDPROC)WinProc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hIcon = NULL;
	wc.hIconSm = NULL;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszClassName = title;
	wc.lpszMenuName = NULL;

	if (!RegisterClassEx(&wc))
		return FALSE;

	HWND hwnd = CreateWindow(
		title,
		title,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		SCREENW,
		SCREENH,
		(HWND)NULL,
		(HMENU)NULL,
		hInstance,
		(LPVOID)NULL
	);

	if (hwnd == 0)	return 0;

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	if (!Game_Init(hwnd))	return 0;

	while (!gameover)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		Game_Run(hwnd);
	}

	Game_End(hwnd);

	return msg.wParam;
}