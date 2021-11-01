#pragma once
#include "GameObject.h"
#include "Graphics/JGGraphics.h"
#include "Class/Asset/Asset.h"
#include "Class/Asset/AssetManager.h"

namespace JG
{
	/* GameComponent
	// GameNode �� ���� Ŭ����
	// ex) MeshRenderer, SpriteRenderer, �� Image, Transform ��� �̷���
	*/

	class GameNode;
	class GameWorld;
	class GameComponent : public GameObject
	{
		friend GameNode;
		JGCLASS
	private:
		GameNode*  mOwnerNode = nullptr;
		GameWorld* mGameWorld = nullptr;
		bool mIsActive = true;
		bool mIsRunStart = false;
	protected:
		virtual void Start() override;
		virtual void Destory() override;
	public:
		virtual void MakeJson(SharedPtr<JsonData> jsonData)   const override;
		virtual void LoadJson(SharedPtr<JsonData> jsonData) override;
	public:
		GameNode*  GetOwner() const;
		GameWorld* GetGameWorld() const;
	public:
		void SetActive(bool isActive);
		bool IsActive() const;
	};
}