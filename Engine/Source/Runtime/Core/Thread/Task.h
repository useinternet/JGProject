#pragma once
#include "CoreDefines.h"
#include "Memory/Memory.h"
#include "Misc/Delegate.h"
#include "Object/ObjectGlobals.h"

class PTaskArguments;
JG_DECLARE_DELEGATE_ONEPARAM(PTaskDelegate, const PTaskArguments&)

class PTaskArguments : public IMemoryObject
{
	friend class PTask;
	struct HArgumentData
	{
		JGType Type;
		HList<int8> BtData;

		HArgumentData(const JGType& type, const HList<int8>& btData)
			: Type(type)
			, BtData(btData) {}
	};

	HList<HArgumentData> _argumentDatas;

public:
	PTaskArguments() = default;
	virtual ~PTaskArguments() = default;

	template<class T>
	bool GetValue(int32 index, T* outValue) const
	{
		// Dummy
		index += 1;

		if (index >= GetCount() || outValue == nullptr)
		{
			return false;
		}

		const HArgumentData& argData = _argumentDatas[index];
		if (argData.BtData.size() != argData.Type.GetSize())
		{
			return false;
		}

		if (JGTYPE(T) != argData.Type)
		{
			return false;
		}

		memcpy(outValue, argData.BtData.data(), argData.BtData.size());

		return true;
	}

	int32 GetCount() const
	{
		return (int32)_argumentDatas.size();
	}

private:
	template<class T>
	void add(const T& value)
	{
		HList<int8> btData;
		btData.resize(sizeof(T));

		memcpy(btData.data(), &value, sizeof(T));

		_argumentDatas.push_back(HArgumentData(JGTYPE(T), btData));
	}

	template<class T, class ... Args>
	void add(const T& value, const Args& ... args)
	{
		add(value);
		add(args...);
	}

	void reset()
	{
		_argumentDatas.clear();
	}
};

class PTask : public IMemoryObject
{
	friend class PThread;
private:
	PTaskArguments _taskArguments;
	PTaskDelegate  _taskDelegate;

	HAtomicBool _bIsRunning;

public:
	PTask();
	virtual ~PTask() = default;

	bool IsRunning() const;
	bool IsWaiting() const;
	bool IsValid() const;

	template<class ... Args>
	void Bind(const PTaskDelegate& taskDelegate, const Args& ... args)
	{
		_taskDelegate = taskDelegate;

		_taskArguments.reset();
		_taskArguments.add((bool)0/*Dump*/, args...);
	}

	void DoTask();

public:
	template<class ... Args>
	static PSharedPtr<PTask> Create(PWeakPtr<IMemoryObject> refObject, const std::function<void(const PTaskArguments&)>& func, const Args& ... args)
	{
		PSharedPtr<PTask> task = Allocate<PTask>();
		task->Bind(PTaskDelegate::Create(refObject, func), args...);

		return task;
	}
};


