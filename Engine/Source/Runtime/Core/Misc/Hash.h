#pragma once




#include "CoreDefines.h"

#ifdef _M_X64
#define ENABLE_SSE_CRC32 1
#else
#define ENABLE_SSE_CRC32 0
#endif

#if ENABLE_SSE_CRC32
#pragma intrinsic(_mm_crc32_u32)
#pragma intrinsic(_mm_crc32_u64)
#endif

class HHash
{
public:
	template<class T>
	static uint64 Hash(const T& obj)
	{
		return (uint64)(std::hash<T>()(obj));
	}

	static uint64 HashRange(const uint32* const Begin, const uint32* const End, uint64 NumHash)
	{
#if ENABLE_SSE_CRC32
		const uint64* Iter64 = (const uint64*)(((uint64)Begin + 7) & ~7);
		const uint64* const End64 = (const uint64* const)((uint64)End & ~7);

		// If not 64-bit aligned, start with a single u32
		if ((uint32*)Iter64 > Begin)
			NumHash = _mm_crc32_u32((uint32)NumHash, *Begin);

		// Iterate over consecutive u64 values
		while (Iter64 < End64)
			NumHash = _mm_crc32_u64((uint64)NumHash, *Iter64++);

		// If there is a 32-bit remainder, accumulate that
		if ((uint32*)Iter64 < End)
			NumHash = _mm_crc32_u32((uint32)NumHash, *(uint32*)Iter64);
#else
		// An inexpensive hash for CPUs lacking SSE4.2
		for (const u32* Iter = Begin; Iter < End; ++Iter)
			NumHash = 16777619U * NumHash ^ *Iter;
#endif

		return NumHash;
	}

	template <typename T>
	static uint64 HashState(const T* StateDesc, uint64 Count = 1, uint64 NumHash = 2166136261U)
	{
		static_assert((sizeof(T) & 3) == 0 && alignof(T) >= 4, "State object is not word-aligned");
		return HashRange((uint32*)StateDesc, (uint32*)(StateDesc + Count), NumHash);
	}
};