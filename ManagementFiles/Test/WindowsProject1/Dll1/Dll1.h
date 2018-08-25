#pragma once
#include<Windows.h>
#define MAX_LOADSTRING 100

#pragma once
#define DllExport __declspec(dllexport) 
#include <windows.h> 
#include <windowsx.h>
#include <d3d11.h> 
#include <d3dx11.h> 
#include <d3dx10.h> 
#include <DirectXMath.h> 
using namespace DirectX; 
// include the Direct3D Library fil
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dx11.lib")
#pragma comment (lib, "d3dx10.lib") 
// global declarations 
IDXGISwapChain* swapchain;
// the pointer to the swap chain interface 
ID3D11Device *dev; 
// the pointer to our Direct3D device interface 
ID3D11DeviceContext *devcon; 
// the pointer to our Direct3D device context 
ID3D11RenderTargetView *backbuffer; 
// the pointer to our back buffer 
extern "C" 
{
	__declspec(dllexport) void InitD3D(HWND hWnd, int Width, int Height); 
	// sets up and initializes Direct3D 
} 
extern "C" 
{ 
	__declspec(dllexport) void RenderFrame(void); 
	// renders a single frame 
} 
extern "C"
{ 
	__declspec(dllexport) void CleanD3D(void); 
	// closes Direct3D and releases memory 
} 
