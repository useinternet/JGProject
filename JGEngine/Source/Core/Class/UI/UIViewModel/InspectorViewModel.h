#pragma once
#include "Class/UI/UIViewModel.h"



namespace JG
{
	class InspectorModel;
	class InspectorViewModel : public UIViewModel
	{
		InspectorModel*   mInspectorModel = nullptr;
		SharedPtr<IAsset> mAssetInstance = nullptr;
	protected:
		virtual void Initialize() override;
		virtual void Destroy() override;
		virtual void OnEvent(IEvent& e) override;
	public:
		IJGObject* GetTargetObject() const;
		void SetTargetObject(IJGObject* gameObject);

		const SortedSet<String>& FindComponentTypeList(const String& filter);
		void SelectComponentType(const String& typeName);
	private:
		bool ResponseDestroyGameObject(NotifyDestroyJGObjectEvent& e);
		bool ResponseSelectedGameNodeInEditor(NotifySelectedGameNodeInEditorEvent& e);
		bool ResponseSelectedAssetInEditor(NotifySelectedAssetInEditorEvent& e);
	};
}