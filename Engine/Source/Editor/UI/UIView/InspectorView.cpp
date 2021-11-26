#include "pch.h"
#include "InspectorView.h"
#include "Imgui/imgui.h"
#include "Class/Asset/Asset.h"
#include "GameObjectFactory.h"
#include "GameNode.h"
#include "UI/ContextUI/ComponentFinderContextView.h"
#include "UI/UIManager.h"
namespace JG
{
	InspectorView::InspectorView()
	{
		UIManager::GetInstance().RegisterMainMenuItem("Windows/Inspector", 0, [&]()
		{
			Open();
		}, nullptr);
		memset(mFindFilterStr, 0, 256);
	}

	void InspectorView::Load()
	{

	}
	void InspectorView::Initialize()
	{
		memset(mFindFilterStr, 0, 256);
	}
	void InspectorView::OnGUI()
	{
		ImGui::Begin("Inspector", &mOpenGUI);
		UIManager::GetInstance().ShowInspectorUI(GetTargetObject());
		ImGui::End();
		if (mOpenGUI == false)
		{
			mOpenGUI = true;
			Close();
		}
	}
	void InspectorView::Destroy()
	{
		mFilterStr.clear();
		mComponentTypeSet.clear();
		mFilteredTypeList.clear();
		SetTargetObject(nullptr);
	}

	IJGObject* InspectorView::GetTargetObject() const
	{
		return mTargetObject;
	}
	void InspectorView::SetTargetObject(IJGObject* gameObject)
	{
		mTargetObject = gameObject;
	}
	const SortedSet<String>& InspectorView::FindComponentTypeList(const String& filter)
	{
		bool isDirty = false;
		auto& originSet = GameObjectFactory::GetInstance().GetGameComponentTypeSet();
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
	void InspectorView::SelectComponentType(const String& typeName)
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


	void InspectorView::OnEvent(IEvent& e)
	{
		EventDispatcher dispatcher(e); 
		dispatcher.Dispatch<NotifyDestroyJGObjectEvent>(EVENT_BIND_FN(&InspectorView::ResponseDestroyGameObject));
		dispatcher.Dispatch<NotifySelectedGameNodeInEditorEvent>(EVENT_BIND_FN(&InspectorView::ResponseSelectedGameNodeInEditor));
		dispatcher.Dispatch< NotifySelectedAssetInEditorEvent>(EVENT_BIND_FN(&InspectorView::ResponseSelectedAssetInEditor));
	}
	bool InspectorView::ResponseDestroyGameObject(NotifyDestroyJGObjectEvent& e)
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
	bool InspectorView::ResponseSelectedGameNodeInEditor(NotifySelectedGameNodeInEditorEvent& e)
	{
		if (e.SelectedGameNode != nullptr && e.SelectedGameNode->IsAlive() == true)
		{
			SetTargetObject(e.SelectedGameNode);
		}

		return false;
	}
	bool InspectorView::ResponseSelectedAssetInEditor(NotifySelectedAssetInEditorEvent& e)
	{
		if (fs::exists(e.SelectedAssetPath) == false) return false;

		
		auto asset = AssetDataBase::GetInstance().LoadOriginAsset(e.SelectedAssetPath);
		
		if (asset != nullptr)
		{
			mAssetInstance = asset;
			SetTargetObject(mAssetInstance.get());
		}
		return false;
	}
}