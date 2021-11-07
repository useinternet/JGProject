#include "pch.h"
#include "GameComponent.h"
#include "GameNode.h"


namespace JG
{
	void GameComponent::Start()
	{
		GameObject::Start();
	}
	void GameComponent::Destory()
	{
		GameObject::Destory();
		mOwnerNode = nullptr;
	}
	void GameComponent::MakeJson(SharedPtr<JsonData> jsonData) const
	{
		GameObject::MakeJson(jsonData);
		jsonData->AddMember("IsActive", IsActive());
	}
	void GameComponent::LoadJson(SharedPtr<JsonData> jsonData)
	{
		GameObject::LoadJson(jsonData);
		auto val = jsonData->GetMember("IsActive");
		if (val)
		{
			SetActive(val->IsBool());
		}
	}
	GameNode* GameComponent::GetOwner() const
	{
		return mOwnerNode;
	}
	GameWorld* GameComponent::GetGameWorld() const
	{
		return mGameWorld;
	}
	void GameComponent::SetActive(bool isActive)
	{
		mIsActive = isActive;
	}
	bool GameComponent::IsActive() const
	{
		return mIsActive && GetOwner()->IsActive();
	}
}