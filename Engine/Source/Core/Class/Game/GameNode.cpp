#include "pch.h"
#include "GameNode.h"
#include "GameWorld.h"
#include "Components/Transform.h"
#include "Class/UI/UIView/InspectorView.h"
#include "Class/UI/ContextUI/ComponentFinderContextView.h"
namespace JG
{
	void GameNode::Start()
	{
		GameObject::Start();
	}
	void GameNode::Destory()
	{
		GameObject::Destory();
		SetParent(nullptr);
	}

	void GameNode::Update()
	{
		GameObject::Update();
		if (mIsAlive == false) return;
		for (auto& com : mComponents)
		{
			if (com->IsActive())
			{
				if (com->mIsRunStart == false)
				{
					com->mIsRunStart = true;
					com->Start();
				}
				com->Update();
			}
		}

		for (auto& child : mChilds)
		{
			if (child->IsActive())
			{
				if (child->mIsRunStart == false)
				{
					child->mIsRunStart = true;
					child->Start();
				}
				child->Update();
			}
		}
	}
	void GameNode::LateUpdate()
	{
		GameObject::LateUpdate();
		if (mIsAlive == false) return;
		for (auto& com : mComponents)
		{
			if (com->IsActive())
			{
				com->LateUpdate();
			}
		}

		for (auto& child : mChilds)
		{
			if (child->IsActive())
			{
				child->LateUpdate();
			}
		}
	}
	GameNode::GameNode()
	{
		mTransform = AddComponent<Transform>();
	}
	GameNode::GameNode(GameWorld* gameWorld) : GameNode()
	{
		mGameWorld = gameWorld;
	}
	void GameNode::MakeJson(SharedPtr<JsonData> jsonData) const
	{
		GameObject::MakeJson(jsonData);

		{
			auto comListJson = jsonData->CreateJsonData();
			comListJson->GetValue().SetArray();

			for (auto& com : mComponents)
			{
				auto comJson = jsonData->CreateJsonData();
				comJson->AddMember("ComponentType", com->GetType().GetName());
				com->MakeJson(comJson);
				comListJson->GetValue().PushBack(comJson->GetValue(), jsonData->GetJsonAllocator());
			}

			jsonData->AddMember("Components", comListJson);
		}
		{
			auto childListJson = jsonData->CreateJsonData();
			childListJson->GetValue().SetArray();

			for (auto& child : mChilds)
			{
				auto childJson = jsonData->CreateJsonData();
				child->MakeJson(childJson);
				childListJson->GetValue().PushBack(childJson->GetValue(), jsonData->GetJsonAllocator());
			}

			jsonData->AddMember("Childs", childListJson);

		}
	}
	void GameNode::LoadJson(SharedPtr<JsonData> jsonData)
	{
		GameObject::LoadJson(jsonData);

		auto comListVal = jsonData->GetMember("Components");
		if (comListVal)
		{
			auto comCnt = comListVal->GetSize();
			for (u64 i = 0; i < comCnt; ++i)
			{
				auto comVal = comListVal->GetJsonDataFromIndex(i);
				if (i == 0)
				{
					mTransform->LoadJson(comVal);
				}
				else
				{
					auto comTypeName = comVal->GetMember("ComponentType")->GetString();
					auto comType = GameObjectFactory::GetInstance().GetGameObjectType(comTypeName);
					auto com = AddComponent(comType);
					com->LoadJson(comVal);
				}
			}
		}


		auto childListVal = jsonData->GetMember("Childs");
		if (childListVal)
		{
			auto childCnt = childListVal->GetSize();
			for (auto i = 0; i < childCnt; ++i)
			{
				auto childVal = childListVal->GetJsonDataFromIndex(i);
				auto node = AddNode(std::to_string(i));
				node->LoadJson(childVal);

			}
		}

	}
	void GameNode::OnInspectorGUI() 
	{
		const float width = ImGui::GetWindowWidth();
		const float wpadding = ImGui::GetStyle().FramePadding.x;

		ImGui::Dummy(ImVec2(0, 1.0f));

		ImGui::AlignTextToFramePadding();
		ImGui::Text("Name"); ImGui::SameLine();

		char objName[128] = { 0, };
		strcpy(objName, GetName().c_str());
		const float input_width = width * 0.4f;
		ImGui::SetNextItemWidth(input_width);
		if (ImGui::InputText(("##GameNode InputName"), objName, 128))
		{
			SetName(objName);
		}






		const float combo_width = width * 0.2f;
		ImGui::SameLine(width - ImGui::CalcTextSize("Layer").x - wpadding * 2 - combo_width);
		ImGui::Text("Layer"); ImGui::SameLine();
		ImGui::SetNextItemWidth(combo_width);
		ImGui::SameLine(width - combo_width - wpadding);
		if (ImGui::BeginCombo("##Layer Combo Box", mTargetLayer.c_str()))
		{
			GameLayerManager::GetInstance().ForEach([&](const String& layerName)
			{
				bool _bool = false;
				if (ImGui::Selectable(layerName.c_str(), &_bool))
				{
					SetLayer(layerName);
				}
			});
			ImGui::EndCombo();
		}


		ImGui::Dummy(ImVec2(0, 1.0f));
		ImGui::Separator();
		List<GameComponent*> removeComList;
		for (auto& com : mComponents)
		{
			bool is_open = true;
			ImGui::Spacing();
			auto id = com->GetName() + "##" + std::to_string((ptraddr)com);
			if (ImGui::CollapsingHeader(
				id.c_str(), &is_open, ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_CollapsingHeader) == true)
			{
				com->OnInspectorGUI();
			}

			if (is_open == false)
			{
				removeComList.push_back(com);
			}
		}

		for (auto& com : removeComList)
		{
			Destroy(com);
		}
		ImGui::Spacing();	ImGui::Spacing();	ImGui::Spacing();
		auto padding = ImGui::GetStyle().FramePadding;
		if (ImGui::Button("Add Component", ImVec2(ImGui::GetWindowSize().x - (padding.x * 4), 0)) == true)
		{
			UIManager::GetInstance().OpenPopupUIView<ComponentFinderContextView>(ComponentFinderInitData());
		}
		if (UIManager::GetInstance().OnContextUIView<ComponentFinderContextView>())
		{
			auto comFinder     = UIManager::GetInstance().GetPopupUIView<ComponentFinderContextView>();
			auto selectedType  = comFinder->GetSelectedComponent();
			auto inspectorView = UIManager::GetInstance().GetUIView<InspectorView>();
			if (inspectorView)
			{
				inspectorView->SelectComponentType(selectedType);
			}
		}
	}

	GameNode* GameNode::AddNode(const String& name)
	{
		auto obj = GameObjectFactory::GetInstance().CreateObject<GameNode>();
		obj->SetName(name);
		obj->SetParent(this);
		obj->mGameWorld = mGameWorld;
		obj->Awake();
		return obj;
	}
	GameComponent* GameNode::AddComponent(const Type& type)
	{

		bool isComponent = GameObjectFactory::GetInstance().IsGameComponent(type);
		if (isComponent == false)
		{
			JG_CORE_ERROR("This Type({0}) is not Component Category.", type.GetName());
			return nullptr;
		}
		auto obj = GameObjectFactory::GetInstance().CreateObjectByType(type);
		if (obj == nullptr)
		{
			JG_CORE_ERROR("This Type({0}) is not registered.", type.GetName());
			return nullptr;
		}
		auto com = static_cast<GameComponent*>(obj);
		com->mOwnerNode = this;
		com->mGameWorld = mGameWorld;
		com->Awake();
		mComponents.push_back(com);

		return com;
	}
	void GameNode::Destroy(GameNode* node)
	{
		GameObject::DestoryObject(node);
	}
	void GameNode::Destroy(GameComponent* component)
	{
		GameObject::DestoryObject(component);
	}
	void GameNode::ForEach(const std::function<void(GameNode*)>& action)
	{
		for (auto& node : mChilds)
		{
			action(node);
		}
	}
	void GameNode::SendChangeData(const ChangeData& data, EChangeDataFlags flags)
	{
		for (auto& com : mComponents)
		{
			if (com == data.Object)
			{
				continue;
			}
			com->OnChange(data);
		}
		for (auto& node : mChilds)
		{
			if (node == data.Object)
			{
				continue;
			}
			node->OnChange(data);
			node->SendChangeData(data);
		}

	}
	GameNode* GameNode::FindNode(const String& name) const
	{
		for (auto& node : mChilds)
		{
			if (node->GetName() == name)
			{
				return node;
			}
		}
		return nullptr;
	}
	GameNode* GameNode::FindNode(i64 index) const
	{
		if (mChilds.size() <= index)
		{
			return nullptr;
		}
		return mChilds[index];
	}

	void GameNode::Swap(GameNode* node1, GameNode* node2)
	{
		i64 n1 = GetNodeIndex(node1);
		i64 n2 = GetNodeIndex(node2);
		Swap(n1, n2);
	}
	void GameNode::Swap(i64 n1, i64 n2)
	{
		if (n1 == n2) return;
		if (n1 == -1 || n2 == -1) return;
		auto size = mChilds.size();
		if (size <= n1 || size <= n2) return;


		auto node1 = mChilds[n1];
		auto node2 = mChilds[n2];

		
		auto temp = mChilds[n1];
		mChilds[n1] = node2;
		mChilds[n2] = node1;

	}
	i64 GameNode::GetNodeIndex(GameNode* gameNode)
	{
		i64 result = -1;
		i32 cnt = 0;
		ForEach([&](GameNode* node)
		{
			if (gameNode == node)
			{
				result = cnt;
				return;
			}
			cnt++;
		});

		return result;
	}
	u64 GameNode::GetChildCount() const
	{
		return mChilds.size();
	}
	void GameNode::SetParent(GameNode* node)
	{
		bool isDiff = mParent != node;
		if (isDiff == false)
		{
			return;
		}
		if (mParent != nullptr)
		{
			auto& begin = mParent->mChilds.begin();
			auto& end   = mParent->mChilds.end();
			mParent->mChilds.erase(std::remove(begin, end, this), mParent->mChilds.end());
			mParent = nullptr;
		}
		if (node != nullptr)
		{
			mParent = node;
			mParent->mChilds.push_back(this);
		}
		GetTransform()->Refresh();
	}
	GameNode* GameNode::GetParent() const
	{
		return mParent;
	}
	GameWorld* GameNode::GetGameWorld() const
	{
		return mGameWorld;
	}
	Transform* GameNode::GetTransform() const
	{
		return mTransform;
	}
	void GameNode::SetPickingBoundingBox(const JBBox& boundingBox, 
		const std::function<bool(const JBBox*, const JRay&, void*)>&     picking3DInteraction,
		const std::function<bool(const JBBox*, const JVector2&, void*)>& picking2DInteraction,
		void* interactionUserData, u64 userDataSize)
	{
		if (mPickingBoundingBox == nullptr)
		{
			mPickingBoundingBox = CreateUniquePtr<JBBox>();
		}
		*mPickingBoundingBox    = boundingBox;
		mPicking3DInteractionFunc = picking3DInteraction;
		mPicking2DInteractionFunc = picking2DInteraction;
		if (interactionUserData != nullptr)
		{
			mInteractionUserData.clear();
			mInteractionUserData.resize(userDataSize);
			memcpy(&mInteractionUserData[0], interactionUserData, userDataSize);
		}

	}
	const JBBox* GameNode::GetPickingBoundingBox() const
	{
		return mPickingBoundingBox.get();
	}
	void GameNode::SetLayer(const String& layer)
	{
		if (GameLayerManager::GetInstance().IsRegisterLayer(layer) == false)
		{
			mTargetLayer = GameLayer::DEFAULT_LAYER;
		}
		else
		{
			mTargetLayer = layer;
		}
	}
	const String& GameNode::GetLayer()
	{
		return mTargetLayer;
	}
	bool GameNode::IsActive() const
	{
		return mIsActive;
	}
	void GameNode::SetActive(bool active)
	{
		mIsActive = active;
	}
	bool GameNode::IsAlive() const
	{
		return mIsAlive;
	}
	void GameNode::Picking3DRecursive(List<GameNode*>& refPickingObjectList, const JRay& pickingRay)
	{
		for (auto& child : mChilds)
		{
			child->Picking3DRecursive(refPickingObjectList, pickingRay);
		}
		auto bbox = GetPickingBoundingBox();
		if (bbox)
		{
			if (mPicking3DInteractionFunc == nullptr)
			{
				auto localRay = pickingRay;
				auto& invWorld = GetTransform()->GetInvWorldMatrix();
				localRay.dir = invWorld.TransformVector(localRay.dir);
				localRay.origin = invWorld.TransformPoint(localRay.origin);

				if (bbox->Intersection(localRay) == true)
				{
					refPickingObjectList.push_back(this);
				}
			}
			else
			{
				bool result = mPicking3DInteractionFunc(bbox, pickingRay, mInteractionUserData.data());
				if (result)
				{
					refPickingObjectList.push_back(this);
				}
			}

		}
	}
	void GameNode::Picking2DRecursive(List<GameNode*>& refPickingObjectList, const JVector2& pickingPos)
	{
		for (auto& child : mChilds)
		{
			child->Picking2DRecursive(refPickingObjectList, pickingPos);
		}
		auto bbox = GetPickingBoundingBox();
		if (bbox)
		{
			if (mPicking2DInteractionFunc == nullptr)
			{
				JBBox worldBBox;
				JMatrix worldMatrix = GetTransform()->GetWorldMatrix();

				worldBBox.min = worldMatrix.TransformPoint(bbox->min);
				worldBBox.max = worldMatrix.TransformPoint(bbox->max);
				worldBBox.min.z = 0.0f;
				worldBBox.max.z = 0.0f;


				if (worldBBox.Contain(JVector3(pickingPos, 0.0f)) == true)
				{
					refPickingObjectList.push_back(this);
				}
			}
			else
			{
				bool result = mPicking2DInteractionFunc(bbox, pickingPos, mInteractionUserData.data());
				if (result)
				{
					refPickingObjectList.push_back(this);
				}
			}
		}
	}
	void GameNode::DestroyRecursive()
	{
		mIsAlive = false;
		for (auto& child : mChilds)
		{
			child->DestroyRecursive();
		}
		for (auto& com : mComponents)
		{
			GameObjectFactory::GetInstance().DestroyObject(com);
		}
		for (auto& child : mChilds)
		{
			GameObjectFactory::GetInstance().DestroyObject(child);
		}
		GameObjectFactory::GetInstance().DestroyObject(this);
	}
}