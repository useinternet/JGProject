#pragma once
#include"d3dx12.h"
#include"ScreenGrab12.h"
#include"WICTextureLoader12.h"
#include"Dx12Exception.h"
#include"Dx12TypeDefines.h"
#include<dxgi1_5.h>
#include <d3d12.h>
#include<Data/Log.h>
#include<Data/Path.h>
#include<Math/Transformation.h>
#include"DirectXToolKit/VertexCollection.h"
#include<wrl.h>

#ifdef _DEVELOP_DEBUG
#pragma comment(lib,"Common_develop.lib")
#pragma comment(lib,"DirectXTex_d.lib")
#endif
#ifdef _DEVELOP_RELEASE
#pragma comment(lib,"Common_developR.lib")
#pragma comment(lib,"DirectXTex.lib")
#endif
#ifdef _RELEASE
#pragma comment(lib,"Common.lib")
#pragma comment(lib,"DirectXTex.lib")
#endif
REISTER_COMMONPATH

#define GLOBAL_SHADER_PATH(x) GLOBAL_PATH / std::string("Contents/Engine/Shaders/") + x \

#define GLOBAL_TEXTURE_PATH(x) GLOBAL_PATH / std::string("Contents/Engine/Textures/") + x \

#define DX12_LOG(LogLevel, Contents, ...) ENGINE_LOG(Dx12::DxDevice::g_LogGroupName.c_str(), LogLevel, Contents, __VA_ARGS__)
#define DX12_LOG_INFO(Contents, ...)     DX12_LOG(Common::Log::Info, Contents, __VA_ARGS__)
#define DX12_LOG_TRACE(Contents, ...)    DX12_LOG(Common::Log::Default, Contents, __VA_ARGS__)
#define DX12_LOG_WARNING(Contents, ...)  DX12_LOG(Common::Log::Warning, Contents, __VA_ARGS__)
#define DX12_LOG_ERROR(Contents, ...)    DX12_LOG(Common::Log::Error, Contents, __VA_ARGS__)
#define DX12_LOG_CRITICAL(Contents, ...) DX12_LOG(Common::Log::Critical, Contents, __VA_ARGS__)












