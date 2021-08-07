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
		//virtual const String& GetName() const = 0;
	public:
		virtual void MakeJson(SharedPtr<JsonData> jsonData)   const override {}
		virtual void LoadJson(SharedPtr<JsonData> jsonData) override {}
	public:
		virtual void OnInspectorGUI() override {}
		virtual ~IJGObject() = default;
	};
}