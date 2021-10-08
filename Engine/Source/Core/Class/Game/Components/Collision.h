#pragma once
#include "Class/Game/GameComponent.h"


namespace JG
{
	class Collision : public GameComponent
	{
		JGCLASS
	private:
		String mCollisionType;
		JVector3 mCenter;
		f32 mMass = 10.0f;


		bool mIsShapeDirty = true;
		bool mIsRigidDirty = true;
		PhysicsHandle mRigidHandle;
		PhysicsHandle mShapeHandle;



		bool mIsDebugDraw    = false;
		bool mIsDynamicRigid = false;
	protected:
		virtual void Awake() override;
		virtual void Start() override;
		virtual void Destory() override;
	protected:
		virtual void Update() override;
		virtual void LateUpdate() override;
		virtual void MakeJson(SharedPtr<JsonData> jsonData)   const override;
		virtual void LoadJson(SharedPtr<JsonData> jsonData) override;
		virtual void OnInspectorGUI() override;
	private:
		void CreateRigid();
		void UpdateRigid();
		void RemoveRigid();
		void CreateShape();
		void RemoveShape();
	protected:
		virtual SharedPtr<physx::PxGeometry> CreatePxGeometry() { return nullptr; }
		virtual void DrawDebugShape(physx::PxRigidStatic* rigidStatic) {}
		void SendShapeDirty();


	public:
		void SetCenter(const JVector3& center);
		const JVector3& GetCenter() const;
	protected:
		Color GetDebugColor() const {
			return Color::Green();
		}

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
		virtual void DrawDebugShape(physx::PxRigidStatic* rigidStatic) override;
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
		virtual void DrawDebugShape(physx::PxRigidStatic* rigidStatic) override;
	public:
		void SetSize(const JVector3& size);
		const JVector3&  GetSize() const;
	};
}