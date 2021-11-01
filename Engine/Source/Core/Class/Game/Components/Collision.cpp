#include "pch.h"
#include "Collision.h"
#include "Transform.h"
#include "Class/Game/GameWorld.h"
namespace JG
{

	void Collision::Awake()
	{



	}
	void Collision::Start()
	{

	}

	void Collision::Destory()
	{
		GameComponent::Destory();
	}

	void Collision::Update()
	{
		GameComponent::Update();
	}

	void Collision::LateUpdate()
	{
		GameComponent::LateUpdate();
	}

	void Collision::MakeJson(SharedPtr<JsonData> jsonData) const
	{
		GameComponent::MakeJson(jsonData);

	}

	void Collision::LoadJson(SharedPtr<JsonData> jsonData)
	{
		GameComponent::LoadJson(jsonData);

	}

	void Collision::OnInspectorGUI()
	{
		GameComponent::OnInspectorGUI();

	}


	void Collision::OnChange(const ChangeData& data)
	{

	}


}