#pragma once
#include "GameObject.h"


namespace JG
{
	// ���� �ý���
	class GlobalGameSystem : public GameObject
	{
		JGCLASS
	protected:
		virtual void Start()   override;
		virtual void Destory() override;
	public:
		virtual ~GlobalGameSystem() = default;
	};



	// ���忡���� ����Ǵ� �ý���
	class GameSystem : public GameObject
	{
		JGCLASS
		friend class GameObject;
		friend class GameWorld;
		GameWorld* mGameWorld = nullptr;
	public:
		virtual ~GameSystem() = default;
	protected:
		virtual void Start()   override;
		virtual void Destory() override;
	public:
		GameWorld* GetOwnerGameWorld() const {
			return mGameWorld;
		}
	};
}