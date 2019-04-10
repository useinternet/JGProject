#pragma once
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include<Data/Log.h>
#include<Data/Path.h>
#include<Data/Data_IO.h>
#include"Dx2DTypeDefines.h"


#ifdef _DEVELOP_DEBUG
#pragma comment(lib,"Common_develop.lib")
#endif
#ifdef _DEVELOP_RELEASE
#pragma comment(lib,"Common_developR.lib")
#endif
#ifdef _RELEASE
#pragma comment(lib,"Common.lib")
#endif


REISTER_COMMONPATH
#define DX2D_LOG_PATH GLOBAL_PATH / "Engine/Log/Direct2D.txt"



#define DX2D_LOG(LogLevel, Contents, ...) ENGINE_LOG(Dx2D::Dx2DDevice::g_LogGroupName.c_str(), LogLevel, Contents, __VA_ARGS__)
#define DX2D_LOG_INFO(Contents, ...)     DX2D_LOG(Common::Log::Info, Contents, __VA_ARGS__)
#define DX2D_LOG_TRACE(Contents, ...)    DX2D_LOG(Common::Log::Default, Contents, __VA_ARGS__)
#define DX2D_LOG_WARNING(Contents, ...)  DX2D_LOG(Common::Log::Warning, Contents, __VA_ARGS__)
#define DX2D_LOG_ERROR(Contents, ...)    DX2D_LOG(Common::Log::Error, Contents, __VA_ARGS__)
#define DX2D_LOG_CRITICAL(Contents, ...) DX2D_LOG(Common::Log::Critical, Contents, __VA_ARGS__)