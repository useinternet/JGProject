export module CoreDefines;

import <vector>;
import <unordered_map>;
import <map>;
import <unordered_set>;
import <set>;
import <queue>;

using int8  = char;
using int16 = short;
using int32 = int;
using int64 = long long;


using uint8  = unsigned char;
using uint16 = unsigned short;
using uint32 = unsigned int;
using uint64 = unsigned long long;



template<class Key, class Value>
using HashMap = std::unordered_map<Key, Value>;

template<class Key, class Value>
using Map = std::map<Key, Value>;

template<class T>
using HashSet = std::unordered_set<T>;

template<class T>
using Set = std::set<T>;



