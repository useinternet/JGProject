#pragma once
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
#include <map>
#include <functional>
#include <algorithm>
#include <filesystem>
#include <atomic>
#include <chrono>
#include <ppl.h>
#include <concurrent_vector.h>
#include <filesystem>
namespace fs = std::filesystem;

#include <d3d12.h>
#include <dxgi1_6.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXCollision.h>
#include <DirectXColors.h>
#include <wrl.h>
#include <d3dcompiler.h>
#include <d3dx12.h>





using namespace Microsoft::WRL;

#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"Dxgi.lib")
#pragma comment(lib,"dxguid.lib")


#if defined(min)
#undef min
#endif

#if defined(max)
#undef max
#endif

#if defined(far)
#undef min
#endif

#if defined(near)
#undef max
#endif