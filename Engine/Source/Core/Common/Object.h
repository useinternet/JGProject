#pragma once
#include "Define.h"
#include "Type.h"
#include "Class/FileIO.h"
namespace JG
{

#define JGCLASS \
public: \
	virtual Type GetType() const override { return Type(TypeID(this));} \
private: \


	class IInspectorGUI
	{
	public:
		virtual ~IInspectorGUI() = default;
		virtual void OnInspectorGUI() = 0;
	
	};

	class ISubscriber
	{
	public:
		virtual ~ISubscriber() = default;
	};



	
	class IJGObject : public IInspectorGUI, public ISubscriber, public IJson
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
		virtual void MakeJson(SharedPtr<JsonData> jsonData)   const override {}
		virtual void LoadJson(SharedPtr<JsonData> jsonData) override {}
	public:
		virtual void OnInspectorGUI() override {}
		virtual ~IJGObject() = default;
	};
}