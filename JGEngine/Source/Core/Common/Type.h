#pragma once
#include <Common/Define.h>
#include <Common/String.h>
#include <atomic>
#include <typeinfo>


#define JGTYPE(x) JG::Type(JG::TypeID<##x>())
#define TYPE_NULL_ID -1
namespace JG
{
	template<class T>
	class TypeID 
	{
		friend class Type;
		static u64 ID;
	public:
		TypeID(T* _this = nullptr)
		{
			if (ID == TYPE_NULL_ID)
			{
				ID = typeid(T).hash_code();
			}
		}
	};
	template<class T>
	u64 TypeID<T>::ID = TYPE_NULL_ID;



	class Type
	{
		u64    mID = TYPE_NULL_ID;
		mutable bool   mIsNameDirty = true;
		mutable std::string mName;
	public:
		Type()
		{
			mID   = TYPE_NULL_ID;
		}

		template<class T>
		constexpr Type(const TypeID<T>& typeID)
		{
			mID   = typeID.ID;
			mName = typeid(T).name();
			mIsNameDirty = true;
		}
		

		template<class T>
		constexpr Type& operator=(const TypeID<T>& typeID)
		{
			mID = typeID.ID;
			mName = typeid(T).name();
			mIsNameDirty = true;
			return *this;
		}
		Type& operator=(const Type& type)
		{
			mID   = type.mID;
			mName = type.GetName();
			return *this;
		}
		bool operator==(const Type& type) const
		{
			return mID == type.mID;
		}
		bool operator!=(const Type& type) const
		{
			return mID != type.mID;
		}
	public:
		u64 GetID() const {
			return mID;
		}
		const String& GetName() const {
			if (mIsNameDirty)
			{
				mIsNameDirty = false;
				u64 pos = mName.find(" ", 0);
				if (pos != std::string::npos)
				{
					mName = mName.substr(pos + 1);
				}
			}

			return mName;
		}
	private:

		friend std::hash<JG::Type>;
	};



	


}


namespace std
{
	template<>
	struct hash<JG::Type>
	{
		size_t operator()(JG::Type const& type) const noexcept
		{
			return type.mID;
		}
	};
}