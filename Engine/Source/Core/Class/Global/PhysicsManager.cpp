#include "pch.h"
#include "PhysicsManager.h"
#include "Application.h"


using namespace physx;

namespace JG
{
	PhysicsManager::PhysicsManager()
	{
		Init();
	}
	PhysicsManager::~PhysicsManager()
	{

		if (mCPUDispatcher != nullptr)
		{
			mCPUDispatcher->release();
			mCPUDispatcher = nullptr;
		}
		if (mPhysics != nullptr)
		{
			mPhysics->release();
			mPhysics = nullptr;
		}
		if (mPvdTransport != nullptr)
		{
			mPvdTransport->release();
			mPvdTransport = nullptr;
		}
		if (mPvd != nullptr)
		{
			mPvd->release();
			mPvd = nullptr;
		}
		if (mFoundation != nullptr)
		{
			mFoundation->release();
			mFoundation = nullptr;
		}
	}
	physx::PxPhysics* PhysicsManager::GetPxPhysics() const
	{
		return mPhysics;
	}
	u64 PhysicsManager::CreatePxScene()
	{
		PxSceneDesc sceneDesc  = PxSceneDesc(PxTolerancesScale());
		sceneDesc.filterShader = PxDefaultSimulationFilterShader;
		sceneDesc.gravity.y    = 0.0f;
		sceneDesc.cpuDispatcher = mCPUDispatcher;
		auto scene = mPhysics->createScene(sceneDesc);
		
		PhysicsHandle handle = (PhysicsHandle)scene;

		mScenePool[handle] = scene;
		return handle;
	}
	PhysicsHandle PhysicsManager::CreatePxRigidStatic()
	{
		auto rigidStatic = mPhysics->createRigidStatic(physx::PxTransform());

		PhysicsHandle handle = (PhysicsHandle)rigidStatic;

		mRigidStaticPool[handle] = rigidStatic;

		return handle;
	}
	PhysicsHandle PhysicsManager::CreatePxRigidDynamic()
	{
		auto rigidDynamic = mPhysics->createRigidDynamic(physx::PxTransform());

		PhysicsHandle handle = (PhysicsHandle)rigidDynamic;

		mRigidDynamicPool[handle] = rigidDynamic;

		return handle;
	}

	PhysicsHandle PhysicsManager::CreatePxShape(const std::function<PxShapeData()>& returnAction)
	{
		auto pxShapeData = returnAction();
		if (pxShapeData.Geometry == nullptr)
		{
			return 0;
		}
		auto m     = mPhysics->createMaterial(pxShapeData.StaticFriction, pxShapeData.DynamicFriction, pxShapeData.Restitution);
		auto shape = mPhysics->createShape(*pxShapeData.Geometry, *m);

		PhysicsHandle handle = (PhysicsHandle)shape;


		mShapePool[handle] = std::pair<physx::PxMaterial*, physx::PxShape*>(m, shape);
		return handle;
	}

	void PhysicsManager::RemovePxScene(PhysicsHandle handle)
	{
		if (mScenePool.find(handle) != mScenePool.end())
		{
			mScenePool[handle]->release();
			mScenePool.erase(handle);
		}
	}

	void PhysicsManager::RemovePxRigidStatic(PhysicsHandle handle)
	{
		if (mRigidStaticPool.find(handle) != mRigidStaticPool.end())
		{
			mRigidStaticPool[handle]->release();
			mRigidStaticPool.erase(handle);
		}
	}

	void PhysicsManager::RemovePxRigidDynamic(PhysicsHandle handle)
	{
		if (mRigidDynamicPool.find(handle) != mRigidDynamicPool.end())
		{
			mRigidDynamicPool[handle]->release();
			mRigidDynamicPool.erase(handle);
		}
	}

	void PhysicsManager::RemovePxShape(PhysicsHandle handle)
	{
		if (mShapePool.find(handle) != mShapePool.end())
		{
			mShapePool[handle].first->release();
			mShapePool[handle].second->release();
			mShapePool.erase(handle);
		}
	}

	void PhysicsManager::PxSceneReadWrite(PhysicsHandle handle, const std::function<void(physx::PxScene*)>& action)
	{
		if (mScenePool.find(handle) != mScenePool.end())
		{
			action(mScenePool[handle]);
		}
	}
	void PhysicsManager::PxRigidStaticReadWrite(PhysicsHandle handle, const std::function<void(physx::PxRigidStatic*)>& action)
	{
		if (mRigidStaticPool.find(handle) != mRigidStaticPool.end())
		{
			action(mRigidStaticPool[handle]);
		}
	}
	void PhysicsManager::PxRigidDynamicSceneReadWrite(PhysicsHandle handle, const std::function<void(physx::PxRigidDynamic*)>& action)
	{
		if (mRigidDynamicPool.find(handle) != mRigidDynamicPool.end())
		{
			action(mRigidDynamicPool[handle]);
		}
	}
	void PhysicsManager::PxShapeSceneReadWrite(PhysicsHandle handle, const std::function<void(physx::PxShape*, physx::PxMaterial*)>& action)
	{
		if (mShapePool.find(handle) != mShapePool.end())
		{
			action(mShapePool[handle].second, mShapePool[handle].first);
		}
	}
	void PhysicsManager::Init()
	{
		auto gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, mDefaultAllocatorCallback, mDefaultErrorCallback);
		if (gFoundation == nullptr)
		{
			return;
		}

		auto gPvd = physx::PxCreatePvd(*gFoundation);
		if (gPvd == nullptr)
		{
			return;
		}

#ifdef _DEBUG
		mPvdTransport = PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
		if (mPvdTransport != nullptr)
		{
			gPvd->connect(*mPvdTransport, PxPvdInstrumentationFlag::eALL);
		}
		bool recordMemoryAllocations = true;
#else
		bool recordMemoryAllocations = false;
#endif

		mPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(), recordMemoryAllocations, gPvd);

		if (mPhysics == nullptr)
		{
			return;
		}


		mCPUDispatcher = PxDefaultCpuDispatcherCreate(Scheduler::GetInstance().GetThreadCount());
		if (mCPUDispatcher == nullptr)
		{
			return;
		}

		Scheduler::GetInstance().ScheduleByFrame(1, 0, -1, SchedulePriority::BeginSystem, [&]() -> EScheduleResult
		{
			f32 tick = Application::GetInstance().GetAppTimer()->GetTick();
			for (auto& _pair : mScenePool)
			{
				_pair.second->simulate(tick);
			}
			return EScheduleResult::Continue;
		});



		Scheduler::GetInstance().ScheduleByFrame(1, 0, -1, SchedulePriority::EndSystem, [&]() -> EScheduleResult
		{
			for (auto& _pair : mScenePool)
			{
				_pair.second->fetchResults(true);
			}
			return EScheduleResult::Continue;
		});

		JG_CORE_INFO("Create PhysXAPI : {0}", PX_PHYSICS_VERSION);
	}
}