#include<Windows.h>
#include<d3d9.h>
#include<xinput.h>
#include<d3dx9.h>
#include<iostream>

using namespace std;

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "xinput.lib")

const string AppTITLE = "XInput Test Program";
const wchar_t title[] = L"XInput Test Program";
const int SCREENW = 1024;
const int SCREENH = 768;

LPDIRECT3D9 d3d = NULL;
LPDIRECT3DDEVICE9 d3ddev = NULL;
//flag gameover
bool gameover = false;
//is windowed or not
bool isWindowed = true;

#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)

bool XInput_Init(int contNum = 0)
{
	XINPUT_CAPABILITIES caps;
	ZeroMemory(&caps, sizeof(XINPUT_CAPABILITIES));
	XInputGetCapabilities(contNum, XINPUT_FLAG_GAMEPAD, &caps);
	if (caps.Type != XINPUT_DEVTYPE_GAMEPAD) return false;

	return true;
}

void XInput_Vibrate(int contNum = 0, int left = 65535, int right = 65535)
{
	XINPUT_VIBRATION vibration;
	ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));
	vibration.wLeftMotorSpeed = left;
	vibration.wRightMotorSpeed = right;
	XInputSetState(contNum, &vibration);
}

void XInput_Update()
{
	XINPUT_STATE state;
	string message = "";

	for (int i = 0; i < 4; ++i)
	{
		ZeroMemory(&state, sizeof(XINPUT_STATE));
		message = "";

		DWORD result = XInputGetState(i, &state);

		if (result == 0)
		{
			if (state.Gamepad.bLeftTrigger)
				message = "Left Trigger";
			else if (state.Gamepad.bRightTrigger)
				message = "Right Trigger";
			else if (state.Gamepad.sThumbLX < -10000 || state.Gamepad.sThumbLX > 10000)
				message = "Left Thumb Stick";
			else if (state.Gamepad.sThumbRX < -10000 || state.Gamepad.sThumbRX > 10000)
				message = "Right Thumb Stick";
			else if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP)
				message = "DPAD UP";
			else if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN)
				message = "DPAD DOWN";
			else if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT)
				message = "DPAD LEFT";
			else if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT)
				message = "DPAD Right";
			else if (state.Gamepad.wButtons & XINPUT_GAMEPAD_START)
				message = "Start Button";
			else if (state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB)
				message = "Left Thumb";
			else if (state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB)
				message = "Right Thumb";
			else if (state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER)
				message = "Left Shoulder";
			else if (state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER)
				message = "Right Shoulder";
			else if (state.Gamepad.wButtons & XINPUT_GAMEPAD_A)
			{
				XInput_Vibrate(0, 65535, 65535);
				message = "A Button";
			}
			else if (state.Gamepad.wButtons & XINPUT_GAMEPAD_B)
			{
				XInput_Vibrate(0, 65535, 65535);
				message = "B Button";
			}
			else if (state.Gamepad.wButtons & XINPUT_GAMEPAD_X)
			{
				XInput_Vibrate(0, 65535, 65535);
				message = "X Button";
			}
			else if (state.Gamepad.wButtons & XINPUT_GAMEPAD_Y)
			{
				XInput_Vibrate(0, 65535, 65535);
				message = "Y Button";
			}
			else if (state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK)
				gameover = true;

			if (message.length() > 0)
			
				MessageBox(0, message.c_str(), "Controller", NULL);
		}
		else
		{
			// controller is not connected
		}
	}
}

bool Game_Init(HWND hwnd)
{
	d3d = Direct3DCreate9(D3D_SDK_VERSION);

	if (d3d == NULL)
	{
		MessageBox(hwnd, "Error initializing Direct3D", "Error", MB_OK);
		return false;
	}
	D3DDISPLAYMODE dm;
	d3d->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &dm);

	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));

	if (!isWindowed)
	{
		d3dpp.Windowed = FALSE;
		d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
		d3dpp.BackBufferFormat = dm.Format;
		d3dpp.BackBufferCount = 1;
		d3dpp.BackBufferWidth = dm.Width;
		d3dpp.BackBufferHeight = dm.Height;
		d3dpp.hDeviceWindow = hwnd;
	}
	else
	{
		d3dpp.Windowed = TRUE;
		d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
		d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
		d3dpp.BackBufferCount = 1;
		d3dpp.BackBufferWidth = SCREENW;
		d3dpp.BackBufferHeight = SCREENH;
		d3dpp.hDeviceWindow = hwnd;
	}

	d3d->CreateDevice(
		D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		hwnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp,
		&d3ddev);

	if (d3ddev == NULL)
	{
		MessageBox(hwnd, "Error creating Direct3D device", "Error", MB_OK);
		return FALSE;
	}

	XInput_Init();

	return true;
}

void Game_Run(HWND hwnd)
{
	if (d3ddev == NULL)
	{
		MessageBox(hwnd, "D3Ddevice is NULL", "D3Ddevice is NULL", NULL);
		return;
	}

	if (d3ddev->BeginScene())
	{
		d3ddev->EndScene();
		d3ddev->Present(NULL, NULL, NULL, NULL);
	}
	
	if (KEY_DOWN(VK_ESCAPE))
	{		
		PostMessage(hwnd, WM_DESTROY, 0, 0);
	}
	XInput_Update();
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
	wc.lpszClassName = AppTITLE.c_str();
	wc.lpszMenuName = NULL;

	if (!RegisterClassEx(&wc))
		return FALSE;

	HWND hwnd;
	if (!isWindowed)
	{
		hwnd = CreateWindow(
			AppTITLE.c_str(),
			AppTITLE.c_str(),
			WS_EX_TOPMOST | WS_POPUP,
			0,
			0,
			1440,
			900,
			(HWND)NULL,
			(HMENU)NULL,
			hInstance,
			(LPVOID)NULL
		);
	}
	else
	{
		hwnd = CreateWindow(
			AppTITLE.c_str(),
			AppTITLE.c_str(),
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
	}

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