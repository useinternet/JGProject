#pragma once

// 다이렉트 X
#ifdef USEDIRECTX
#define DIRECTINPUT_VERSION 0x0800

#include<DirectX/D3D11.h>
#include<DirectX/DXGI.h>
#include<DirectX/D3Dcompiler.h>
#include<DirectX/D3Dcommon.h>
#include<DirectX/D3D11Shader.h>
#include<DirectX/dinput.h>
#include<DirectX/D3DX11async.h>
#include<DirectX/d3dx9core.h>


// lib
#ifdef WIN64
#pragma comment(lib,"DirectX/d3d11.lib")
#pragma comment(lib,"DirectX/dxgi.lib")
#pragma comment(lib,"DirectX/d3dcompiler.lib")
#pragma comment(lib,"DirectX/dxguid.lib")
#pragma comment(lib,"DirectX/dinput8.lib")
#ifdef _DEBUG
#pragma comment(lib,"DirectX/d3dx11d.lib")
#pragma comment(lib,"DirectX/d3dx10d.lib")
#pragma comment(lib,"DirectX/D3dx9d.lib")

#else
#pragma comment(lib,"DirectX/d3dx11.lib")
#pragma comment(lib,"DirectX/d3dx10.lib")
#pragma comment(lib,"DirectX/D3dx9.lib")
#endif
#endif
#endif