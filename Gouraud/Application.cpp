#include "Application.h"
#include "Chronometer.h"

LRESULT Application::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	{
		switch (message)
		{
		case WM_CLOSE:
			PostQuitMessage(0);
			return 0;
		case WM_KEYUP:
			processReleasedKey(wParam);
			break;
		default:
			break;
		}
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
}

LPDIRECT3DSURFACE9 Application::createSurface(LPDIRECT3DDEVICE9 dev)
{
	IDirect3DDevice9_Clear(d3dev, 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 170), 1.0f, 0);

	if (SUCCEEDED(IDirect3DDevice9_GetBackBuffer(d3dev, 0, 0, D3DBACKBUFFER_TYPE_MONO, &backbuffer))) {
		IDirect3DSurface9_LockRect(surf, &lockedRect, NULL, D3DLOCK_DISCARD);
		data = (char*)lockedRect.pBits;
		if (drawingFuntionPtr) {
			tictac = !tictac;
			(*drawingFuntionPtr)(data, PIXMAP_WIDTH, PIXMAP_HEIGHT, lockedRect.Pitch, tictac);
		}
		IDirect3DSurface9_UnlockRect(surf);
		IDirect3DDevice9_StretchRect(d3dev, surf, NULL, backbuffer, NULL, D3DTEXF_LINEAR);
		d3dev->Present(nullptr, nullptr, nullptr, nullptr);
	}
	return surf;
}

void Application::renderOnSurface(LPDIRECT3DSURFACE9& surf, LPDIRECT3DDEVICE9& d3dev, D3DPRESENT_PARAMETERS& d3dpp, HWND hWnd, DWORD& endFrame, DWORD& frameTimer)
{

	/* Render our scene */
	if (surf) {
		surf = createSurface(d3dev);
	}
}

void Application::processReleasedKey(WPARAM key)
{
	if (key == VK_ESCAPE) {
		PostQuitMessage(0);
	}
}

int Application::createWindow(HINSTANCE hInstance, std::string name, int w, int h)
{
	this->hInstance = hInstance;
	PIXMAP_WIDTH = w;
	PIXMAP_HEIGHT = h;

	WNDCLASS wc = { 0 };
	wc.lpfnWndProc = Application::WndProc;
	wc.hInstance = hInstance;
	wc.hbrBackground = (HBRUSH)(COLOR_BACKGROUND);
	wc.lpszClassName = L"minwindowsapp";
	if (!RegisterClass(&wc))
		return 1;

	std::wstring widestr = std::wstring(name.begin(), name.end());

	winRect.left = 0;
	winRect.top = 0;
	winRect.right = PIXMAP_WIDTH;
	winRect.bottom = PIXMAP_HEIGHT;
	AdjustWindowRect(&winRect, WS_OVERLAPPEDWINDOW, FALSE);

	// window creation
	if (!(hWnd = CreateWindow(wc.lpszClassName,
		widestr.c_str(),
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		0, 0,
		winRect.right - winRect.left, winRect.bottom - winRect.top,
		0, 0, hInstance, NULL))) {
		return 2;
	}

	// dx9 surface creation
	d9 = Direct3DCreate9(D3D_SDK_VERSION);
	if (waitRefresh) {
		configParametersWaitRR(&d3dpp, hWnd, PIXMAP_WIDTH, PIXMAP_HEIGHT);
	}
	else {
		configParametersNoRR(&d3dpp, hWnd, PIXMAP_WIDTH, PIXMAP_HEIGHT);
	}
	if (FAILED(IDirect3D9_CreateDevice(d9, D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &d3dev))) {
		IDirect3D9_Release(d9);
		return 1;
	}

	if (SUCCEEDED(IDirect3DDevice9_GetBackBuffer(d3dev, 0, 0, D3DBACKBUFFER_TYPE_MONO, &backbuffer))) {
		IDirect3DDevice9_Clear(d3dev, 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 170), 1.0f, 0);
		IDirect3DDevice9_StretchRect(d3dev, surf, NULL, backbuffer, NULL, D3DTEXF_LINEAR);
		IDirect3DSurface9_Release(backbuffer);
	}

	if (FAILED(IDirect3DDevice9_CreateOffscreenPlainSurface(d3dev, PIXMAP_WIDTH, PIXMAP_HEIGHT, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &surf, NULL))) {
		return NULL;
	}

	return 0;
}

void Application::waitRR(bool yesOrNo)
{
	waitRefresh = yesOrNo;
}

void Application::stopLoop()
{
	endLoop = true;
}

void Application::messagesLoop(DRAW_CALL_BACK_PTR ptr)
{
	this->drawingFuntionPtr = ptr;

	DWORD frameTimer = (DWORD)GetTickCount64();
	MSG msg;
	while (!endLoop) {
		DWORD endFrame;
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT) {
				break;
			}
		}
		if (msg.message == WM_QUIT) {
			break;
		}
		Chronometer::startLap();
		renderOnSurface(surf, d3dev, d3dpp, hWnd, endFrame, frameTimer);
		Chronometer::endLap("renderOnSurface");
	}

	// freeing
	if (surf) {
		IDirect3DSurface9_Release(surf);
	}
	IDirect3DDevice9_Release(d3dev);
	IDirect3D9_Release(d9);
	endLoop = true;
}

