#pragma once
#include "GFWClasses/GFWObject.h"


namespace GFW
{
	class GameObject;
	class SubGameWorld;
	class GAMEFRAMEWORK_API GameObjectComponent : public GFWObject
	{
		friend GameObject;
	public:
		GameObjectComponent(const std::wstring& name = TT("GameObjectComponent")) : GFWObject(name) {}
		virtual ~GameObjectComponent() {}
	protected:
		virtual void Awake()   override;
		virtual void Start()   override;
		virtual void Tick()    override;
		virtual void Destroy() override;

		GameObject* GetOwner() const {
			return m_Owner;
		}
		SubGameWorld* GetWorld() const {
			return m_OwnerWorld;
		}
	private:
		SubGameWorld* m_OwnerWorld     = nullptr;
		GameObject*   m_Owner          = nullptr;
		bool          m_IsExecuteStart = false;
	};
}