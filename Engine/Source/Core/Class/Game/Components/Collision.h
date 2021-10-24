#pragma once
#include "Class/Game/GameComponent.h"


namespace JG
{
	class Collision : public GameComponent
	{
		JGCLASS
	protected:
		String   mCollisionType;
		JVector3 mCenter;
		f32 mMass = 10.0f;
		f32 mStaticFriction  = 0.5f;
		f32 mDynamicFriction = 0.5f;
		f32 mRestitution     = 0.5f;


		bool mIsDebugDraw = false;
		bool mIsSimulate  = false;


		PhysicsHandle mRigidHandle;
		PhysicsHandle mMaterialHandle;
		PhysicsHandle mShapeHandle;


		bool mIsDirty = true;
	protected:
		virtual void Awake() override;
		virtual void Start() override;
		virtual void Destory() override;
	protected:
		virtual void Update() override;
		virtual void LateUpdate() override;
		virtual void MakeJson(SharedPtr<JsonData> jsonData) const override;
		virtual void LoadJson(SharedPtr<JsonData> jsonData) override;
		virtual void OnInspectorGUI() override;
	protected:
		void CreateRigid();
		void CreateMaterial();
		void CreateShape();

		void Reset();
	protected:
		virtual SharedPtr<physx::PxGeometry> CreatePxGeometry() { return nullptr; }
		virtual void DrawDebugShape(const physx::PxRigidActor* actor) {}
	public:
		void SetCenter(const JVector3& center);
		const JVector3& GetCenter() const;

		bool IsSimulate() const;
		void SetSimulate(bool isSimulate);

		bool IsDebugDraw() const;
		void SetDebugDraw(bool isDebugDraw);
	protected:
		Color GetDebugColor() const {
			return Color::Green();
		}
	private:
		virtual void OnChange(const ChangeData& data) override;
	};


	class SphereCollision : public Collision
	{
		JGCLASS
	private:
		f32  mRadius = 1.0f;
	protected:
		virtual void MakeJson(SharedPtr<JsonData> jsonData)   const override;
		virtual void LoadJson(SharedPtr<JsonData> jsonData) override;
		virtual void OnInspectorGUI() override;

		virtual SharedPtr<physx::PxGeometry> CreatePxGeometry() override;
		virtual void DrawDebugShape(const physx::PxRigidActor* actor) override;
	public:
		void SetRadius(f32 r);
		f32  GetRadius() const;
	};


	class BoxCollision : public Collision
	{
		JGCLASS
	private:
		JVector3 mSize = JVector3(1,1,1);
	protected:
		virtual void MakeJson(SharedPtr<JsonData> jsonData)   const override;
		virtual void LoadJson(SharedPtr<JsonData> jsonData) override;
		virtual void OnInspectorGUI() override;


		virtual SharedPtr<physx::PxGeometry> CreatePxGeometry() override;
		virtual void DrawDebugShape(const physx::PxRigidActor* actor) override;
	public:
		void SetSize(const JVector3& size);
		const JVector3&  GetSize() const;
	};
}