#pragma once
#include "CoreDefines.h"
#include "Platform/Platform.h"

class HGuid
{
	friend class HPlatform;
private:
	uint64 _data1;
	uint16 _data2;
	uint16 _data3;
	uint8  _data4[8];

	mutable PString _dataStr;
public:
	HGuid();
	bool IsValid() const;
	bool operator==(const HGuid& inGuid) const;
	bool operator!=(const HGuid& inGuid) const;

	const PString& ToString() const;
	uint64 GetHashCode() const;
	static HGuid New();
	static HGuid FromString(const PString& inGuidStr);
};

namespace std {
	template <>
	struct hash<HGuid>
	{
		std::size_t operator()(const HGuid& k) const noexcept
		{
			return (std::size_t)HPlatform::GuidHash(k);
		}
	};
}