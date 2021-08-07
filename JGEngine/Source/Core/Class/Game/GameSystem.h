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
		virtual void OnInspectorGUI() override;
	};



	// ���忡���� ����Ǵ� �ý���
	class GameSystem : public GameObject
	{
		JGCLASS

		friend class GameWorld;
		GameWorld* mGameWorld = nullptr;
	protected:
		virtual void Start()   override;
		virtual void Destory() override;
	public:
		virtual void OnInspectorGUI() override;
	};
}