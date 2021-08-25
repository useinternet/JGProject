#pragma once
#include "GameObject.h"
#include "GameObjectFactory.h"
namespace JG
{
	class GameComponent;
	class GameWorld;
	class Transform;
	class GameNode : public GameObject
	{
		JGCLASS
	private:
		GameNode*  mParent    = nullptr;
		GameWorld* mGameWorld = nullptr;
		List<GameNode*>      mChilds;
		List<GameComponent*> mComponents;

		bool mIsActiveSelf = true;
		bool mIsActive     = true;
		bool mIsRunAwake = false;
		bool mIsRunStart = false;
		Transform* mTransform = nullptr;
	
		String mTargetLayer = GameLayer::DEFAULT_LAYER;


		UniquePtr<JBBox> mPickingBoundingBox;
		std::function<bool(const JBBox*, const JRay&, void*)> mPicking3DInteractionFunc;
		std::function<bool(const JBBox*, const JVector2&, void*)> mPicking2DInteractionFunc;
		List<byte> mInteractionUserData;
	protected:
		virtual void Start() override;
		virtual void Destory() override;
	public: 
		virtual void Update() override;
		virtual void LateUpdate() override;
	public:
		GameNode();
		GameNode(GameWorld* gameWorld);
		virtual ~GameNode() = default;
	public:
		virtual void MakeJson(SharedPtr<JsonData> jsonData)   const override;
		virtual void LoadJson(SharedPtr<JsonData> jsonData) override;
	public:
		virtual void OnInspectorGUI() override;
		virtual SharedPtr<IRenderItem> PushRenderItem() override;
		virtual SharedPtr<ILightItem>  PushLightItem() override;
	public:
		GameNode* AddNode(const String& name);

		template<class T>
		T* AddComponent()
		{
			auto obj = GameObjectFactory::GetInstance().CreateObject<T>();
			obj->mOwnerNode = this;
			obj->mGameWorld = mGameWorld;
			mComponents.push_back(obj);
			static_cast<IGameObject*>(obj)->Awake();
			return obj;
		}
		GameComponent* AddComponent(const Type& type);
		// 일단 이 클래스의 배열에서 제거하고, 
		void Destroy(GameNode* node);
		void Destroy(GameComponent* component);


		void ForEach(const std::function<void(GameNode*)>& action);



		void SendChangeData(const ChangeData& data, EChangeDataFlags flags = EChangeDataFlags::None);

	public:
		GameNode* FindNode(const String& name) const;
		GameNode* FindNode(u32 index) const;


		template<class T>
		T* FindComponent() const
		{
			for (auto& com : mComponents)
			{
				if (com->GetObjectType().GetID() == Type(TypeID<T>()).GetID())
				{
					return static_cast<T*>(com);
				}
			}
			return nullptr;
		}
	public:
		u64 GetChildCount() const;
		virtual void SetParent(GameNode* node);
		GameNode*    GetParent() const;
		Transform*   GetTransform() const;

		void SetPickingBoundingBox(const JBBox& boundingBox,
			const std::function<bool(const JBBox*, const JRay&, void*)>& picking3DInteraction     = nullptr,
			const std::function<bool(const JBBox*, const JVector2&, void*)>& picking2DInteraction = nullptr,
			void* interactionUserData = nullptr, u64 userDataSize = 0);
		const JBBox* GetPickingBoundingBox() const;

		void SetLayer(const String& layer);
		const String& GetLayer();

		bool IsActive() const;
	protected:
		void Picking3DRecursive(List<GameNode*>& refPickingObjectList, const JRay& pickingRay);
		void Picking2DRecursive(List<GameNode*>& refPickingObjectList, const JVector2& pickingPos);
		void DestroyRecursive();
	};


}