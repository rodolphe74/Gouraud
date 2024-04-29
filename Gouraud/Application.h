#pragma once
#include <Windows.h>
#include <string>
#include <d3d9.h>

typedef void (*DRAW_CALL_BACK_PTR)(char *, int w, int h, int pitch, bool tictac);

class Application
{
private:
	HINSTANCE hInstance;
	D3DLOCKED_RECT lockedRect;
	HWND hWnd;
	int x, y;
	char* data;
	int PIXMAP_WIDTH, PIXMAP_HEIGHT;
	bool endLoop = false;
	bool waitRefresh = false;
	int useChrono = 0;
	LPDIRECT3D9 d9 = Direct3DCreate9(D3D_SDK_VERSION);
	LPDIRECT3DDEVICE9 d3dev;
	D3DPRESENT_PARAMETERS d3dpp;
	LPDIRECT3DSURFACE9 surf;
	LPDIRECT3DSURFACE9 backbuffer;
	RECT winRect;

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static void processReleasedKey(WPARAM key);
	LPDIRECT3DSURFACE9 createSurface(LPDIRECT3DDEVICE9 dev);
	void renderOnSurface(LPDIRECT3DSURFACE9& surf, LPDIRECT3DDEVICE9& d3dev, D3DPRESENT_PARAMETERS& d3dpp, HWND hWnd, DWORD& endFrame, DWORD& frameTimer);
	DRAW_CALL_BACK_PTR drawingFuntionPtr = 0;
	bool tictac = false;

	
	static void
		configParametersNoRR(D3DPRESENT_PARAMETERS* d3dpp, HWND hWnd, int w, int h)
	{
		ZeroMemory(d3dpp, sizeof(D3DPRESENT_PARAMETERS));
		d3dpp->Windowed = TRUE;
		d3dpp->SwapEffect = D3DSWAPEFFECT_DISCARD;
		d3dpp->hDeviceWindow = hWnd;
		d3dpp->BackBufferFormat = D3DFMT_UNKNOWN;
		d3dpp->BackBufferWidth = w;
		d3dpp->BackBufferHeight = h;
		d3dpp->EnableAutoDepthStencil = TRUE;
		d3dpp->AutoDepthStencilFormat = D3DFMT_D16;
		d3dpp->PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	}

	static void
		configParametersWaitRR(D3DPRESENT_PARAMETERS* d3dpp, HWND hWnd, int w, int h)
	{
		ZeroMemory(d3dpp, sizeof(D3DPRESENT_PARAMETERS));
		d3dpp->Windowed = TRUE;
		d3dpp->SwapEffect = D3DSWAPEFFECT_DISCARD;
		d3dpp->hDeviceWindow = hWnd;
		d3dpp->BackBufferFormat = D3DFMT_UNKNOWN;
		d3dpp->BackBufferWidth = w;
		d3dpp->BackBufferHeight = h;
		d3dpp->EnableAutoDepthStencil = TRUE;
		d3dpp->AutoDepthStencilFormat = D3DFMT_D16;
	}


public:
	int createWindow(HINSTANCE hInstance, std::string name, int w, int h);
	void waitRR(bool yesOrNo);
	void stopLoop();
	void messagesLoop(DRAW_CALL_BACK_PTR drawFuncPtr);
	void setDrawFunction(DRAW_CALL_BACK_PTR drawFuncPtr);
};

