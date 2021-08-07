#pragma once
#pragma warning(disable : 4996 4819 4309)

#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <memory>
#include <utility>
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/reader.h>

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

#define TT(x) L##x



#define JG_ASSET_FORMAT_KEY "AssetFormat"
#define JG_ASSET_KEY        "Asset"


#define JG_TEXTURE_ASSET_KEY     "@AssetKey_TextureAssetStock"
#define JG_STATIC_MESH_ASSET_KEY "@AssetKey_StaticMeshAssetStock"
#define JG_GAMEWORLD_ASSET_KEY   "@AssetKey_GameWorld"
#define JG_ASSET_FORMAT ".jgasset"
#define JG_SHADER_SCRIPT_FORMAT   ".shaderscript"
#define JG_SHADER_TEMPLATE_FORMAT ".shadertemplate"


namespace JG
{
	using i8 = char;
	using i16 = short;
	using i32 = int;
	using i64 = long long;
	using u8 = unsigned char;
	using u16 = unsigned short;
	using u32 = unsigned int;
	using u64 = unsigned long long;
	using f32 = float;
	using f64 = double;
	using jbyte = char;
	using ptraddr = unsigned long long;
	using handle  = unsigned long long;
	


	const static i8 JG_I8_Min = -128;
	const static i8 JG_I8_MAX = 127;
	const static i16 JG_I16_Min = -32768i16;
	const static i16 JG_I16_MAX = 32767;
	const static i32 JG_I32_Min = -2147483648i32;
	const static i32 JG_I32_MAX = 2147483647;
	const static i64 JG_I64_Min = -9223372036854775808i64;
	const static i64 JG_I64_MAX = 9223372036854775807;

	const static u8  JG_U8_MAX = 0xff;
	const static u16 JG_U16_MAX = 0xffff;
	const static u32 JG_U32_MAX = 0xffffffff;
	const static u64 JG_U64_MAX = 0xffffffffffffffff;

	// Container
	template<class T>
	using List  = std::vector<T>;
	template<class T>
	using Stack = std::stack<T>;
	template<class T>
	using Queue = std::queue<T>;
	template<class T>
	using PriorityQueue = std::priority_queue<T>;


	template<class Key, class Value>
	using SortedDictionary = std::map<Key, Value>;
	template<class Key, class Value>
	using Dictionary = std::unordered_map<Key, Value>;
	template<class T>
	using SortedSet = std::set<T>;
	template<class T>
	using HashSet = std::unordered_set<T>;


	// SmartPtr
	template<class T>
	using UniquePtr = std::unique_ptr<T>;
	template<class T>
	using SharedPtr = std::shared_ptr<T>;
	template<class T>
	using WeakPtr   = std::weak_ptr<T>;
	
	template<class T, class ...Args>
	UniquePtr<T> CreateUniquePtr(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<class T, class ...Args>
	SharedPtr<T> CreateSharedPtr(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}


	// String
	using String = std::string;
	using wchar  = wchar_t;

	class JsonData;
	class IJson
	{
	public:
		virtual void MakeJson(SharedPtr<JsonData> jsonData) const = 0;
		virtual void LoadJson(SharedPtr<JsonData> jsonData)       = 0;
	public:
		virtual ~IJson() = default;
	};
}








