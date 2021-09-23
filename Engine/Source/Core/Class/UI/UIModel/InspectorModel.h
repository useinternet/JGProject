#pragma once

#include "Class/UI/UIModel.h"


namespace JG
{
	class GameNode;
	class InspectorModel : public UIModel
	{
		IJGObject* mTargetObject = nullptr;
		String            mFilterStr;
		HashSet<Type>     mComponentTypeSet;
		SortedSet<String> mFilteredTypeList;
	protected:
		virtual void Initialize() override;
		virtual void Destroy() override;
	public:
		IJGObject* GetTargetObject() const;
		void SetTargetObject(IJGObject* object);
		const SortedSet<String>& FindComponentTypeList(const String& filter);
	};
}