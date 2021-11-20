#pragma once
#include "JGCore.h"
#include "GameSettings.h"
#include "Application.h"
#include "Manager/GameLayerManager.h"
#include "Event/EditorEvent.h"
#include "Event/GameEvent.h"
namespace JG		
{
	namespace Graphics
	{
		class SceneObject;
		class Light;
	}

	class IGameObject : public IJGObject
	{
		JGCLASS
	private:
		friend class GameObjectFactory;
		friend class GameLogicSystemLayer;
	public:
		virtual ~IGameObject() = default;
	public:
		virtual void Awake()   = 0;
		virtual void Start()   = 0;
		virtual void Destory() = 0;
		virtual bool IsAlive() const = 0;
	public:
		virtual const String& GetName() const = 0;
		virtual void  SetName(const String& name) = 0;
	};


	
	struct ChangeData
	{
		Type Type;
		IGameObject* Object = nullptr;
		u64 Flags = 0;
		u64 UserMsg = 0;
	};

	ENUM_FLAG(EChangeDataFlags)
	enum class EChangeDataFlags
	{
		None  = 0x0000, 

	};


	class GameObject : public IGameObject
	{
		JGCLASS
	private:
		mutable u64    mID = 0;
		mutable String mName;
		bool mIsAlive    = true;
		List<SharedPtr<ScheduleHandle>> mScheduleHandleList;

	public:
		GameObject() = default;
		virtual ~GameObject();
	protected:
		virtual void Awake() override {}
		virtual void Start() override {}
		virtual void Destory() override {mIsAlive = false;}
		virtual void Update() {}
		virtual void LateUpdate() {}
	protected: // ½ºÄÉÁì °ü·Ã
		SharedPtr<ScheduleHandle> Schedule(f32 delay, f32 tickCycle, i32 repeat, i32 priority, const SyncTaskFunction& task);
		SharedPtr<ScheduleHandle> ScheduleOnce(f32 delay, i32 priority, const SyncTaskFunction& task);
		SharedPtr<ScheduleHandle> ScheduleByFrame(i32 delayFrame, i32 frameCycle, i32 repeat, i32 priority, const SyncTaskFunction& task);
		SharedPtr<ScheduleHandle> ScheduleOnceByFrame(i32 delayFrame, i32 priority, const SyncTaskFunction& task);
		SharedPtr<ScheduleHandle> ScheduleAsync(const AsyncTaskFunction& task);


		void DrawDebugRay(const JRay& ray, f32 length, Color& color);
		void DrawDebugLine(const JVector3& startPos, const JVector3& endPos,Color& color);
		void DrawDebugBox(const JVector3& location, const JQuaternion& quat, const JVector3& size, const Color& color);
		void DrawDebugSphere(const JVector3& center, f32 r, const Color& color);
		



		template<class T>
		void SendEvent(T& e)
		{
			Application::GetInstance().SendEvent(e);
		}


		template<class T>
		void SendEventImmediate(T& e)
		{
			Application::GetInstance().SendEventImmediate(e);
		}
	public:
		virtual void MakeJson(SharedPtr<JsonData> jsonData)   const override;
		virtual void LoadJson(SharedPtr<JsonData> jsonData) override;
	public:
		virtual bool IsAlive() const override { return mIsAlive; }
		virtual const String& GetName() const override;
		virtual void SetName(const String& name) override;
		virtual void OnChange(const ChangeData& data) {};
		//virtual void OnInspectorGUI() {}
	private:
		void ArrangeScheduleHandle();
	public:
		static void DestoryObject(class GameComponent* component);
		static void DestoryObject(class GameNode* gameNode);
		static void DestoryObject(class GameWorld* gameWorld);
		static void DestoryObject(class GameSystem* gameSys);
	};
}