#pragma once
#include "UI/UIView.h"


namespace JG
{
	
	class InspectorView : public UIView
	{
		JGCLASS
		bool mIsOpenPopup = false;
		char mFindFilterStr[256];

		IJGObject*        mTargetObject = nullptr;
		String            mFilterStr;
		HashSet<Type>     mComponentTypeSet;
		SortedSet<String> mFilteredTypeList;
		SharedPtr<IAsset> mAssetInstance = nullptr;
	public:
		InspectorView();
		virtual ~InspectorView() = default;
	protected:
		virtual void Load() override;
		virtual void Initialize() override;
		virtual void OnGUI() override;
		virtual void Destroy() override;
	public:
		IJGObject* GetTargetObject() const;
		void SetTargetObject(IJGObject* gameObject);

		const SortedSet<String>& FindComponentTypeList(const String& filter);
		void SelectComponentType(const String& typeName);

	private:
		virtual void OnEvent(IEvent& e) override;
	private:
		bool ResponseDestroyGameObject(NotifyDestroyJGObjectEvent& e);
		bool ResponseSelectedGameNodeInEditor(NotifySelectedGameNodeInEditorEvent& e);
		bool ResponseSelectedAssetInEditor(NotifySelectedAssetInEditorEvent& e);
	};
}


