#pragma once

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
#include <cassert>
#include <filesystem>

namespace fs = std::filesystem;

#define _KB   1024
#define _2KB  2048
#define _3KB  3072
#define _4KB  4096
#define _8KB  8192
#define _16KB 16384
#define _32KB 32768
#define _64KB 65536
#define _128KB 131072
#define _256KB 262144
#define _512KB 524288

#define _MB   1048576
#define _2MB  2097152
#define _4MB  4194304
#define _8MB  8388608
#define _16MB 16777216
#define _32MB 33554432
#define _64MB 67108864
#define _128MB 134217728
#define _256MB 268435456
#define _512MB 536870912

#define _GB 1073741824

#define JG_MEMORY_OFFSET 8
#define JG_ASSERT(expression) assert(expression)

#define JG_ENUM_FLAG(enumName) \
enum class enumName; \
inline enumName operator|(enumName c1, enumName c2) \
{ \
	return (enumName)((int32)c1 | (int32)c2); \
} \
inline bool operator&(enumName c1, enumName c2) \
{ \
	return (bool)((int32)c1 & (int32)c2); \
} \
inline enumName operator~(enumName flags)\
{\
	return (enumName)~((int)flags);\
}\

using int8 = char;
using int16 = short;
using int32 = int;
using int64 = long long;


using uint8 = unsigned char;
using uint16 = unsigned short;
using uint32 = unsigned int;
using uint64 = unsigned long long;

using float32 = float;
using float64 = double;


using HAtomicBool   = std::atomic_bool;
using HAtomicInt32  = std::atomic_int;
using HAtomicUint32 = std::atomic_uint;

using ThreadID = uint64;


template<class Key, class Value>
using HHashMap = std::unordered_map<Key, Value>;

template<class Key, class Value>
using HMap = std::map<Key, Value>;

template<class T>
using HHashSet = std::unordered_set<T>;

template<class T>
using HSet = std::set<T>;

template<class T>
using HList = std::vector<T>;

template<class T>
using HQueue = std::queue<T>;

template<class T>
using HStack = std::stack<T>;

using HConditionVariable = std::condition_variable;
using HMutex = std::mutex;

template<class T>
using HLockGuard = std::lock_guard<T>;

template<class First, class Second>
using HPair = std::pair<First, Second>;

using HRawString = std::string;
using HRawWString = std::wstring;


const static int8  JG_INT8_Min = (int8)-128i8;
const static int8  JG_INT8_MAX = (int8)127;
const static int16 JG_INT16_Min = (int16)-32768i16;
const static int16 JG_INT16_MAX = (int16)32767;
const static int32 JG_INT32_Min = (int32)-2147483648i32;
const static int32 JG_INT32_MAX = (int32)2147483647;
const static int64 JG_INT64_Min = (int64)-9223372036854775808i64;
const static int64 JG_INT64_MAX = (int64)9223372036854775807;

const static uint8  JG_UINT8_MAX = 0xff;
const static uint16 JG_UINT16_MAX = 0xffff;
const static uint32 JG_UINT32_MAX = 0xffffffff;
const static uint64 JG_UINT64_MAX = 0xffffffffffffffff;

const static float32 JG_FLOAT32_MAX = (float32)3.40282e+38;
const static float32 JG_FLOAT32_MIN = (float32)1.17549e-38;

const static float64 JG_FLOAT64_MAX = 1.79769e+308;
const static float64 JG_FLOAT64_MIN = 2.22507e-308;

enum
{
	INDEX_NONE = -1,
	NAME_NONE  = -1,
	NULL_ID	   = -1,
};

#define JG_MEMBER_FUNC_BIND(funcName) std::bind(this, &##funcName)