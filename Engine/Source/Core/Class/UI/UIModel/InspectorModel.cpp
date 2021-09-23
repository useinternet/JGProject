#include "pch.h"
#include "InspectorModel.h"
#include "Class/Game/GameObjectFactory.h"
namespace JG
{
	void InspectorModel::Initialize()
	{
		mComponentTypeSet = GameObjectFactory::GetInstance().GetGameComponentTypeSet();
		for (auto& type : mComponentTypeSet)
		{
			mFilteredTypeList.insert(type.GetName());
		}

	}
	void InspectorModel::Destroy()
	{
		mFilterStr.clear();
		mComponentTypeSet.clear();
		mFilteredTypeList.clear();
		SetTargetObject(nullptr);
	}
	IJGObject* InspectorModel::GetTargetObject() const
	{
		return mTargetObject;
	}
	void InspectorModel::SetTargetObject(IJGObject* object)
	{
		mTargetObject = object;
	}
	const SortedSet<String>& InspectorModel::FindComponentTypeList(const String& filter)
	{
		bool isDirty = false;
		auto& originSet  = GameObjectFactory::GetInstance().GetGameComponentTypeSet();
		isDirty |= originSet.size() != mComponentTypeSet.size();
		isDirty |= mFilterStr != filter;

		if (isDirty)
		{
			mFilteredTypeList.clear();
			mComponentTypeSet = originSet;
			mFilterStr = filter;
			for (auto& type : originSet)
			{
				if (mFilterStr.length() == 0)
				{
					mFilteredTypeList.insert(type.GetName());
				}
				auto originName = ReplaceAll(type.GetName(), "JG::", "");
				auto filterName = filter;

				u64 pos = originName.find(filterName);
				if (pos != String::npos)
				{
					mFilteredTypeList.insert(type.GetName());
				}
			}
		}

		return mFilteredTypeList;
	}
}