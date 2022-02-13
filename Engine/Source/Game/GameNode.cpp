#include "pch.h"
#include "GameNode.h"
#include "GameWorld.h"
#include "Components/Transform.h"
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
		if (IsAlive() == false) return;
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
		if (IsAlive() == false) return;
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
					if (com == nullptr)
					{
						continue;
					}
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
			JG_LOG_ERROR("This Type({0}) is not Component Category.", type.GetName());
			return nullptr;
		}
		auto obj = GameObjectFactory::GetInstance().CreateObjectByType(type);
		if (obj == nullptr)
		{
			JG_LOG_ERROR("This Type({0}) is not registered.", type.GetName());
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
	void GameNode::ForEach(const std::function<void(GameComponent*)>& action)
	{
		for (auto& com : mComponents)
		{
			action(com);
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