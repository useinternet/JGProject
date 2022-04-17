#pragma once
#include "Define.h"
#include "Type.h"
#include "Class/FileIO.h"
namespace JG
{

#define JGCLASS \
public: \
	virtual JG::Type GetType() const override { return JG::Type(JG::TypeID(this));} \
private: \


	class ISubscriber
	{
	public:
		virtual ~ISubscriber() = default;
	};



	
	class IJGObject : public ISubscriber, public IJson
	{
	public:
		virtual Type GetType() const  = 0;
		template<class T> 
		bool Is() const {
			return JGTYPE(T) == GetType(); 
		}
		template<class T>
		T* As() const {
		
			if (Is<T>() == false)
			{
				return nullptr; 
			}
			else
			{
				return (T*)(this);
			}
		}
	public:
		virtual void MakeJson(SharedPtr<JsonData> jsonData) const override {}
		virtual void LoadJson(SharedPtr<JsonData> jsonData) override {}
		virtual ~IJGObject() = default;
	};
}