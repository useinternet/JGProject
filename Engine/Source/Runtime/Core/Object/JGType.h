#pragma once

#include "Memory/Memory.h"
#include "String/Name.h"
#include "String/String.h"
#include "ObjectDefines.h"

class JGType : public IMemoryObject
{
	friend class PObjectGlobalsPrivateUtils;
	friend class GObjectGlobalSystem;
protected:
	uint64 ID = TYPE_NULL_ID;
	uint64 Size = 0;
	PName  Name = NAME_NONE;
public:
	JGType();
	virtual ~JGType() = default;
public:
	bool operator==(const JGType& type) const
	{
		return GetID() == type.GetID();
	}
	bool operator!=(const JGType& type) const
	{
		return GetID() != type.GetID();
	}
public:
	uint64 GetID() const;
	uint64 GetSize() const;

	void SetName(const PName& name)
	{
		Name = name;
	}
	const PName& GetName() const
	{
		return Name;
	}

	template<class T>
	static uint64 GenerateTypeID()
	{
		static uint64 type_id = TYPE_NULL_ID;
		if (type_id == TYPE_NULL_ID)
		{
			type_id = typeid(T).hash_code();
		}
		return type_id;
	}

	template<class T>
	static JGType GenerateType()
	{
		static PString type_name;
		if (type_name.Empty())
		{
			type_name = typeid(T).name();
			HList<PString> tokens = type_name.Split(' ');
			if (tokens.size() > 1)
			{
				type_name = tokens[tokens.size() - 1];
			}
		}

		JGType result;
		result.ID = GenerateTypeID<T>();
		result.Size = sizeof(T);
		result.SetName(PName(type_name));

		return result;
	}

	template<>
	static JGType GenerateType<void>()
	{
		static const char* type_name = typeid(void).name();

		JGType result;
		result.ID = GenerateTypeID<void>();
		result.Size = 0;
		result.SetName(PName(type_name));

		return result;
	}

	template<class T>
	static JGType GenerateType(T* _this)
	{
		return GenerateType<T>();
	}
};

namespace std {
	template <>
	struct hash<JGType>
	{
		std::size_t operator()(const JGType& k) const noexcept
		{
			return k.GetID();
		}
	};
}