#include <vector>
#include <unordered_map>
#include <map>
#include <unordered_set>
#include <set>
#include <queue>
#include <stack>
#include <iostream>
#include <sstream>
#include <memory>
#include <thread>
#include <atomic>
#include <mutex>
#include <shared_mutex>
#include <cassert>
#include <filesystem>
#include <chrono>
#include <string>
#include <chrono>
#include <cctype>
#include <codecvt>
#include <xhash>

namespace fs = std::filesystem;

#ifdef _DIRECTX12
#include <d3d12.h>
#include <dxgi1_6.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXCollision.h>
#include <DirectXColors.h>
#include <wrl.h>
#include <d3dcompiler.h>
#include <dxgidebug.h>
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"Dxgi.lib")
#pragma comment(lib,"dxguid.lib")
#endif

#ifdef _PLATFORM_WINDOWS
#include <Rpc.h>
#pragma comment(lib, "Rpcrt4.lib")
#endif 
