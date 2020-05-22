#pragma once
#pragma once
#ifdef DIRECTX12_EXPORTS

#define DIRECTX12_API  __declspec(dllexport)

#else

#define DIRECTX12_API  __declspec(dllimport)

#endif

#include <CommonCore.h>
#define DX12_LOG(level, str, ...) LOG(level, DX12, str, __VA_ARGS__)





#include <d3d12.h>
#include <dxgi1_6.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXCollision.h>
#include <DirectXColors.h>
#include <wrl.h>
#include <d3dcompiler.h>
#include <d3dx12.h>



