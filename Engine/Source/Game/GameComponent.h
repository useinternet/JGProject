#pragma once
#include "GameObject.h"
#include "Graphics/JGGraphics.h"
#include "Class/Asset/Asset.h"
#include "Class/Asset/AssetManager.h"

namespace JG
{
	/* GameComponent
	// GameNode 에 들어가는 클래스
	// ex) MeshRenderer, SpriteRenderer, 뭐 Image, Transform 등등 이런거
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
		bool mIsAwake = false;
		bool mIsRunStart = false;

		HashSet<String> mBindedActionNames;
		HashSet<String> mBindedAxisNames;
	public:
		virtual ~GameComponent() = default;
	protected:
		virtual void Start() override;
		virtual void Destory() override;
	public:
		virtual void MakeJson(SharedPtr<JsonData> jsonData)   const override;
		virtual void LoadJson(SharedPtr<JsonData> jsonData) override;
		
	protected: // 컴포넌트 유틸 함수
		void BindInputAction(const String& actionName, EInputAction inputAction, const std::function<void()>& action);
		void BindInputAxis(const String& axisName,const std::function<void(float)>& action);
		void SetShowCursor(bool isShow);
	public:
		GameNode*  GetOwner() const;
		GameWorld* GetGameWorld() const;
		void SetActive(bool isActive);
		bool IsActive() const;
	};
}