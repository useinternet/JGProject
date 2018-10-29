#pragma once

// 다이렉트 X
#ifdef USEDIRECTX
#define DIRECTINPUT_VERSION 0x0800
#include"D3D11.h"
#include"DXGI.h"
#include"D3Dcompiler.h"
#include"D3Dcommon.h"
#include"D3D11Shader.h"
#include"dinput.h"
#include"D3DX11async.h"
#include"d3dx9core.h"
// lib
#ifdef WIN64
#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"dinput8.lib")
#ifdef _DEBUG
#pragma comment(lib,"d3dx11d.lib")
#pragma comment(lib,"d3dx10d.lib")
#pragma comment(lib,"D3dx9d.lib")

#else
#pragma comment(lib,"d3dx11.lib")
#pragma comment(lib,"d3dx10.lib")
#pragma comment(lib,"D3dx9.lib")
#endif
#endif
#endif