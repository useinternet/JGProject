

#define WIN32_LEAN_AND_MEAN
#include<Windows.h>
#include<string>
#include<chrono>
#include<vector>
#include<queue>
#include<deque>
#include<random>
#include<unordered_map>
#include <wincodec.h>
#include <wrl\client.h>
#include<set>
#include <memory>
#include<unordered_set>
#include<functional>
#include <wrl.h>
#include<dxgi1_5.h>
#include <d3d12.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>

#include <list>
#include<map>
#include<fstream>
#include<filesystem>
#include<thread>
#include<mutex>
#include<iostream>
#include <intrin.h>
#include<stdlib.h>
#include<assert.h>
#pragma warning(disable : 4996)
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "WindowsCodecs.lib")

#define INLINE __forceinline
#if defined(min)
#undef min
#endif

#if defined(max)
#undef max
#endif

#if defined(CreateWindow)
#undef CreateWindow
#endif

#define _KB(x) (x * 1024)
#define _MB(x) (x * 1024 * 1024)

#define _64KB _KB(64)
#define _1MB _MB(1)
#define _2MB _MB(2)
#define _4MB _MB(4)
#define _8MB _MB(8)
#define _16MB _MB(16)
#define _32MB _MB(32)
#define _64MB _MB(64)
#define _128MB _MB(128)
#define _256MB _MB(256)
#define PI 3.14159265359f
#define TT(x) L##x


namespace fs = std::experimental::filesystem;