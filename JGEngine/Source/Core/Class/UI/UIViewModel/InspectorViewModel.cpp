#include "pch.h"
#include "InspectorViewModel.h"
#include "Class/Asset/Asset.h"
#include "Class/Game/GameObjectFactory.h"
#include "Class/Game/GameNode.h"
#include "Class/UI/UIModel/InspectorModel.h"
namespace JG
{
	void InspectorViewModel::Initialize()
	{
		mInspectorModel = RegisterUIModel<InspectorModel>();
	}
	void InspectorViewModel::Destroy()
	{
		UIViewModel::Destroy();
		mInspectorModel = nullptr;
	}
	void InspectorViewModel::OnEvent(IEvent& e)
	{
		EventDispatcher dispatcher(e); // ResponseSelectedGameNodeInEditor
		dispatcher.Dispatch<NotifyDestroyJGObjectEvent>(EVENT_BIND_FN(&InspectorViewModel::ResponseDestroyGameObject));
		dispatcher.Dispatch<NotifySelectedGameNodeInEditorEvent>(EVENT_BIND_FN(&InspectorViewModel::ResponseSelectedGameNodeInEditor));
		dispatcher.Dispatch< NotifySelectedAssetInEditorEvent>(EVENT_BIND_FN(&InspectorViewModel::ResponseSelectedAssetInEditor));
	}
	IJGObject* InspectorViewModel::GetTargetObject() const
	{
		if (mInspectorModel != nullptr)
		{
			return mInspectorModel->GetTargetObject();
		}
		return nullptr;
	}
	void InspectorViewModel::SetTargetObject(IJGObject* gameNode)
	{
		if (mInspectorModel != nullptr)
		{
			mInspectorModel->SetTargetObject(gameNode);
		}
	}
	const SortedSet<String>& InspectorViewModel::FindComponentTypeList(const String& filter)
	{
		if (mInspectorModel != nullptr)
		{
			return mInspectorModel->FindComponentTypeList(filter);
		}
		static SortedSet<String> temp;
		return temp;
	}
	void InspectorViewModel::SelectComponentType(const String& typeName)
	{
		if (GetTargetObject() == nullptr)
		{
			return;
		}
		auto gameObject = GetTargetObject();

		bool isGameNode = GameObjectFactory::GetInstance().IsGameNode(gameObject->GetType());
		if (isGameNode == true)
		{
			auto gameNode = static_cast<GameNode*>(gameObject);
			if (gameNode)
			{
				auto type = GameObjectFactory::GetInstance().GetGameObjectType(typeName);
				gameNode->AddComponent(type);
			}
		}
		
	}
	bool InspectorViewModel::ResponseDestroyGameObject(NotifyDestroyJGObjectEvent& e)
	{
		auto targetObject = GetTargetObject();
		if (targetObject != nullptr)
		{
			if (targetObject == e.DestroyedObject)
			{
				SetTargetObject(nullptr);
			}
		}
		return false;
	}
	bool InspectorViewModel::ResponseSelectedGameNodeInEditor(NotifySelectedGameNodeInEditorEvent& e)
	{
		if (e.SelectedGameNode != nullptr)
		{
			SetTargetObject(e.SelectedGameNode);
		}
		
		return false;
	}
	bool InspectorViewModel::ResponseSelectedAssetInEditor(NotifySelectedAssetInEditorEvent& e)
	{
		auto asset = AssetDataBase::GetInstance().GetIAsset((*e.SelectedAsset));
		if (asset != nullptr)
		{
			SetTargetObject(asset);
		}
		return false;
	}
}