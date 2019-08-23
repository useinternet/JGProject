#pragma once
#include <iostream>
#include <utility>

namespace std
{
	template <typename T>
	inline void hash_combine(std::size_t& seed, const T& v)
	{
		std::hash<T> hasher;
		seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	}

	template<typename T1, typename T2>
	struct hash<std::pair<T1, T2>>
	{
		std::size_t operator()(const std::pair<T1, T2>& p) const noexcept
		{
			std::size_t seed1 = 0;
			hash_combine(seed1, p.first);
			hash_combine(seed1, p.second);

			std::size_t seed2 = 0;
			hash_combine(seed2, p.second);
			hash_combine(seed2, p.first);

			return std::min(seed1, seed2);
		}
	};
}