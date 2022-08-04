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


using AtomicBool   = std::atomic_bool;
using AtomicInt32  = std::atomic_int;
using AtomicUint32 = std::atomic_uint;

using ThreadID = std::thread::id;


template<class Key, class Value>
using PHashMap = std::unordered_map<Key, Value>;

template<class Key, class Value>
using PMap = std::map<Key, Value>;

template<class T>
using PHashSet = std::unordered_set<T>;

template<class T>
using PSet = std::set<T>;

template<class T>
using PList = std::vector<T>;

template<class T>
using PQueue = std::queue<T>;

template<class T>
using PStack = std::stack<T>;


using PMutex = std::mutex;

template<class T>
using PLockGuard = std::lock_guard<T>;

template<class First, class Second>
using PPair = std::pair<First, Second>;




#define JG_MEMORY_OFFSET 8


#define JG_ASSERT(expression) assert(expression)


