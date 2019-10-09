﻿// pch.h: 미리 컴파일된 헤더 파일입니다.
// 아래 나열된 파일은 한 번만 컴파일되었으며, 향후 빌드에 대한 빌드 성능을 향상합니다.
// 코드 컴파일 및 여러 코드 검색 기능을 포함하여 IntelliSense 성능에도 영향을 미칩니다.
// 그러나 여기에 나열된 파일은 빌드 간 업데이트되는 경우 모두 다시 컴파일됩니다.
// 여기에 자주 업데이트할 파일을 추가하지 마세요. 그러면 성능이 저하됩니다.

#ifndef PCH_H
#define PCH_H

// 여기에 미리 컴파일하려는 헤더 추가
#include "framework.h"
#include <Windows.h>
#include <iostream>
#include <vector>
#include <list>
#include <chrono>
#include <memory>
#include <ppl.h>
#include <ppltasks.h>
#include <map>
#include <queue>
#include <unordered_map>
#include <mutex>
#include <fstream>
#include <string>
#include <cmath>
#include <map>
#include <functional>
#include  <Psapi.h>
#include <algorithm>
#include <experimental/filesystem>
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"

#include <d3d12.h>
#include <dxgi1_6.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXCollision.h>
#include <DirectXColors.h>
#include <wrl.h>
#include <d3dcompiler.h>
#include <d3dx12.h>
#include "CommonCore.h"


using namespace Microsoft::WRL;
namespace fs = std::experimental::filesystem;
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"Dxgi.lib")
#pragma comment(lib,"dxguid.lib")
#endif //PCH_H


