#pragma once
#include "JGCore.h"
#include "physx/PxPhysXConfig.h"
#include "physx/PxPhysicsAPI.h"
#include "physx/PxPhysics.h"

namespace JG
{
	using PhysicsHandle = u64;

	class PxShapeData
	{
	public:
		SharedPtr<physx::PxGeometry> Geometry = nullptr;
		f32 StaticFriction  = 0.0f;
		f32 DynamicFriction = 0.0f;
		f32 Restitution     = 0.0f;
	};

	class PhysicsManager : public GlobalSingleton<PhysicsManager>
	{
		friend class Application;

		physx::PxDefaultErrorCallback mDefaultErrorCallback;
		physx::PxDefaultAllocator     mDefaultAllocatorCallback;
		physx::PxFoundation*		  mFoundation = nullptr;
		physx::PxPvd*				  mPvd = nullptr;
		physx::PxPvdTransport*		  mPvdTransport = nullptr;
		physx::PxPhysics*			  mPhysics = nullptr;
		physx::PxDefaultCpuDispatcher* mCPUDispatcher = nullptr;




		Dictionary<PhysicsHandle, physx::PxScene*>        mScenePool;
		Dictionary<PhysicsHandle, physx::PxRigidStatic*>  mRigidStaticPool;
		Dictionary<PhysicsHandle, physx::PxRigidDynamic*> mRigidDynamicPool;
		Dictionary<PhysicsHandle, std::pair<physx::PxMaterial*, physx::PxShape*>> mShapePool;
	public:
		PhysicsManager();
		virtual ~PhysicsManager();
	public:
		physx::PxPhysics* GetPxPhysics() const;
	public:
		PhysicsHandle  CreatePxScene();
		PhysicsHandle CreatePxRigidStatic();
		PhysicsHandle CreatePxRigidDynamic();
		PhysicsHandle CreatePxShape(const std::function<PxShapeData()>& returnAction);


		void RemovePxScene(PhysicsHandle handle);
		void RemovePxRigidStatic(PhysicsHandle handle);
		void RemovePxRigidDynamic(PhysicsHandle handle);
		void RemovePxShape(PhysicsHandle handle);


		void PxSceneReadWrite(PhysicsHandle handle, const std::function<void(physx::PxScene*)>& action);
		void PxRigidStaticReadWrite(PhysicsHandle handle, const std::function<void(physx::PxRigidStatic*)>& action);
		void PxRigidDynamicSceneReadWrite(PhysicsHandle handle, const std::function<void(physx::PxRigidDynamic*)>& action);
		void PxShapeSceneReadWrite(PhysicsHandle handle, const std::function<void(physx::PxShape*, physx::PxMaterial*)>& action);
	private:
		void Init();

	};
}