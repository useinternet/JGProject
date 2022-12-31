#pragma once
#include "Core.h"
#include "GUIDefine.h"

class WWidget;
class HGUIBuilder
{
private:
	enum class ECommand
	{
		None,
		BeginHorizontal,
		EndHorizontal,
		BeginVertical,
		EndVertical
	};

	class PCommandValue : public IMemoryObject
	{
	public:
		virtual ~PCommandValue() = default;
	};

	class PWidgetCommandValue : public PCommandValue
	{
	public:
		PSharedPtr<WWidget> Widget;

		virtual ~PWidgetCommandValue() = default;
	};

	struct HCommandData
	{
		ECommand Command;
		PSharedPtr<PCommandValue> CommandValue;
	};

public:
	void BeginHorizontal(int32 fixedWidth = INDEX_NONE);
	void EndHorizontal();

	void BeginVertical(int32 fixedHeight = INDEX_NONE);
	void EndVertical();

	template<class T>
	PSharedPtr<T> PushWidget()
	{
		return nullptr;
	}

private:
	HQueue<HCommandData> _commandQueue;
};


