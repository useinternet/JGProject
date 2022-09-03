#pragma once
#include "CoreDefines.h"
#include "Memory/Memory.h"
#include "Misc/Delegate.h"
#include "Object/ObjectGlobals.h"

class PTaskArguments;
JG_DECLARE_DELEGATE_ONEPARAM(PTaskDelegate, PTaskArguments)

class PTaskArguments : public IMemoryObject
{
	friend class PTask;
	struct PArgumentData
	{
		JGType Type;
		PList<byte> BtData;

		PArgumentData(const JGType& type, const PList<byte>& btData)
			: Type(type)
			, BtData(btData) {}
	};

	PList<PArgumentData> _argumentDatas;

	PTaskArguments() = default;

public:
	virtual ~PTaskArguments() = default;

	template<class T>
	bool GetValue(int32 index, T* outValue) const
	{
		if (index >= GetCount() || outValue == nullptr)
		{
			return false;
		}

		const PArgumentData& argData = _argumentDatas[index];
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
		PList<byte> btData;
		btData.resize(sizeof(T));

		memcpy(btData.data(), &value, sizeof(T));

		_argumentDatas.push_back(PArgumentData(JGTYPE(T), btData));
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

	AtomicBool _bIsRunning;
private:
	PTask();

public:
	virtual ~PTask() = default;

	bool IsRunning() const;
	bool IsWaiting() const;
	bool IsValid() const;

	template<class T, class ... Args>
	void Bind(const PTaskDelegate& taskDelegate, const Args& ... args)
	{
		_taskDelegate = taskDelegate;

		_taskArguments.reset();
		_taskArguments.add(args...);
	}

	void DoTask();
};


