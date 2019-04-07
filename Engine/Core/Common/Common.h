#pragma once
#define WIN32_LEAN_AND_MEAN
#include<Windows.h>

#include<iostream>
#include<string>
#include<fstream>
#include<vector>
#include<queue>
#include<unordered_map>
#include<thread>
#include<mutex>
#include<deque>
#include<map>
#include<set>
#include<functional>
#include<algorithm>
#pragma warning(disable : 4996)
#include <DirectXMath.h>
#include <intrin.h>
#include<filesystem>

namespace fs = std::experimental::filesystem;
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
#define JG_PI 3.14159265359f
#define JG_2PI 6.28318530718f
#define TT(x) L##x
#include"Math/MathHelper.h"

