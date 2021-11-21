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
		for (auto& name : mBindedActionNames)
		{
			InputManager::GetInstance().UnBindAction(this, name);
		}
		for (auto& name : mBindedAxisNames)
		{
			InputManager::GetInstance().UnBindAxis(this, name);
		}
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
	void GameComponent::BindInputAction(const String& actionName, EInputAction inputAction, const std::function<void()>& action)
	{
		mBindedActionNames.insert(actionName);
		InputManager::GetInstance().BindAction(this, actionName, inputAction, action, &mIsActive);
	}
	void GameComponent::BindInputAxis(const String& axisName,  const std::function<void(float)>& action)
	{
		mBindedAxisNames.insert(axisName);
		InputManager::GetInstance().BindAxis(this, axisName, action, &mIsActive);
	}

	void GameComponent::SetShowCursor(bool isShow)
	{
		Application::GetInstance().GetWindow()->SetShowCursor(isShow);
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