#include "pch.h"
#include "PhysicsManager.h"
#include "Application.h"


//using namespace physx;

namespace JG
{
	PhysicsManager::PhysicsManager()
	{
		//Init();
	}
	PhysicsManager::~PhysicsManager()
	{

		/*if (mCPUDispatcher != nullptr)
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
		}*/
	}
	/*physx::PxPhysics* PhysicsManager::GetPxPhysics() const
	{
		return mPhysics;
	}
	u64 PhysicsManager::CreatePxScene()
	{
		PxSceneDesc sceneDesc  = PxSceneDesc(PxTolerancesScale());
		sceneDesc.filterShader = PxDefaultSimulationFilterShader;
		sceneDesc.cpuDispatcher = mCPUDispatcher;
		auto scene = mPhysics->createScene(sceneDesc);
		return CreateHandle(scene, PhysicsType_Scene);
	}
	PhysicsHandle PhysicsManager::CreatePxRigidStatic(const JVector3& p, const JQuaternion& q)
	{
		auto rigidStatic = mPhysics->createRigidStatic(physx::PxTransform(p.x, p.y, p.z, physx::PxQuat(q.x, q.y, q.z, q.w)));
		return CreateHandle(rigidStatic, PhysicsType_RigidStatic);
	}
	PhysicsHandle PhysicsManager::CreatePxRigidDynamic(const JVector3& p, const JQuaternion& q)
	{
		auto rigidDynamic = mPhysics->createRigidDynamic(physx::PxTransform(p.x, p.y, p.z, physx::PxQuat(q.x, q.y, q.z, q.w)));
		return CreateHandle(rigidDynamic, PhysicsType_RigidDynamic);
	}

	PhysicsHandle PhysicsManager::CreatePxShape(const std::function<PxShapeData()>& returnAction)
	{
		auto pxShapeData = returnAction();
		if (pxShapeData.Geometry == nullptr || pxShapeData.MaterialHandle == 0)
		{
			return 0;
		}


		auto handleData = GetHandleData(pxShapeData.MaterialHandle);
		if (handleData == nullptr || (handleData->Type & PhysicsType_Material) == false)
		{
			return 0;
		}
		physx::PxMaterial* m = (physx::PxMaterial*)handleData->Ptr;
		auto shape = mPhysics->createShape(*pxShapeData.Geometry, *m, true);

		return 	CreateHandle(shape, PhysicsType_Shape);
	}

	PhysicsHandle PhysicsManager::CreatePxMaterial(f32 staticFriction, f32 dynamicFriction, f32 restitution)
	{
		auto m = mPhysics->createMaterial(staticFriction, dynamicFriction, restitution);
		return CreateHandle(m, PhysicsType_Material);
	}

	void PhysicsManager::PushRemoveCommand(PhysicsHandle handle)
	{
		auto command = CreateSharedPtr<RemoveCommand>();
		command->Handle = handle;
		mCommandQueue.push(command);
	}

	void PhysicsManager::PushAttachCommand(PhysicsHandle dest, PhysicsHandle src)
	{
		auto command = CreateSharedPtr<AttachCommand>();
		command->Dest = dest;
		command->Src = src;
		mCommandQueue.push(command);
	}

	void PhysicsManager::PushDetachCommand(PhysicsHandle dest, PhysicsHandle src)
	{
		auto command = CreateSharedPtr<DetachCommand>();
		command->Dest = dest;
		command->Src = src;
		mCommandQueue.push(command);
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


		mPhysicsTypeHashCode[PhysicsType_Scene]        = typeid(physx::PxScene).hash_code();
		mPhysicsTypeHashCode[PhysicsType_Actor]        = typeid(physx::PxActor).hash_code();
		mPhysicsTypeHashCode[PhysicsType_RigidActor]   = typeid(physx::PxRigidActor).hash_code();
		mPhysicsTypeHashCode[PhysicsType_RigidStatic]  = typeid(physx::PxRigidStatic).hash_code();
		mPhysicsTypeHashCode[PhysicsType_RigidDynamic] = typeid(physx::PxRigidDynamic).hash_code();
		mPhysicsTypeHashCode[PhysicsType_Shape]        = typeid(physx::PxShape).hash_code();
		mPhysicsTypeHashCode[PhysicsType_Material]     = typeid(physx::PxMaterial).hash_code();




		Scheduler::GetInstance().ScheduleByFrame(1, 0, -1, SchedulePriority::BeginSystem, [&]() -> EScheduleResult
		{
			while (mCommandQueue.empty() == false)
			{
				auto command = mCommandQueue.front(); mCommandQueue.pop();

				switch (command->GetCommandType())
				{
				case ECommandType::Write:
					Command_Write(command);
					break;
				case ECommandType::Attach:
					Command_Attach(command);
					break;
				case ECommandType::Detach:
					Command_Detach(command);
					break;
				case ECommandType::Remove:
					Command_Remove(command);
					break;
				default:
					JG_LOG_ERROR("is not supported physics command type");
					break;
				}
			}



			f32 tick = Application::GetInstance().GetAppTimer()->GetTick();
			if (tick <= 0.0f) return EScheduleResult::Continue;
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

		JG_LOG_INFO("Create PhysXAPI : {0}", PX_PHYSICS_VERSION);
	}

	void PhysicsManager::Command_Remove(SharedPtr<HandleCommand> command)
	{
		auto removeCommand = command->As<RemoveCommand>();
		auto data = GetHandleData(removeCommand->Handle);
		if (data == nullptr)
		{
			return;
		}
		if (data->Type & PhysicsType_Scene)
		{
			physx::PxScene* scene = (physx::PxScene*)data->Ptr;
			scene->release();
			scene = nullptr;
			mScenePool.erase(removeCommand->Handle);
		}
		else if (data->Type & PhysicsType_Actor)
		{
			physx::PxActor* actor = (physx::PxActor*)data->Ptr;
			for (auto& handle : data->DependencyHandles)
			{
				auto handleData = GetHandleData(handle);
				if (handleData == nullptr) continue;

				physx::PxScene* scene = nullptr;
				if (handleData->Type & PhysicsType_Scene)
				{
					scene = (physx::PxScene*)handleData->Ptr;
				}
				if (scene != nullptr)
				{
					scene->removeActor(*actor);
				}
			}
			actor->release();
			actor = nullptr;
		}
		else if (data->Type & PhysicsType_Material)
		{
			physx::PxMaterial* m = (physx::PxMaterial*)data->Ptr;
			m->release();
		}
		else if (data->Type & PhysicsType_Shape)
		{
			physx::PxShape* shape = (physx::PxShape*)data->Ptr;
			for (auto& handle : data->DependencyHandles)
			{
				auto handleData = GetHandleData(handle);
				if (handleData == nullptr) continue;

				physx::PxRigidActor* actor = nullptr;
				if (handleData->Type & PhysicsType_RigidActor)
				{
					actor = (physx::PxRigidActor*)handleData->Ptr;
				}
				if (actor != nullptr)
				{
					actor->detachShape(*shape);
				}
			}
			shape->release();
			shape = nullptr;
		}
		else
		{
			JG_LOG_ERROR("is not supported  PhysicsType in Command_Remove : {0}", (i32)data->Type);
		}




		for (auto& handle : data->DependencyHandles)
		{
			auto handleData = GetHandleData(handle);
			if (handleData == nullptr)
			{
				continue;
			}
			handleData->DependencyHandles.erase(removeCommand->Handle);
		}
		mHandleDataPool.erase(removeCommand->Handle);

	}

	void PhysicsManager::Command_Attach(SharedPtr<HandleCommand> command)
	{
		auto attachCommand = command->As<AttachCommand>();


		auto dest = attachCommand->Dest;
		auto src  = attachCommand->Src;
		auto destData = GetHandleData(dest);
		auto srcData  = GetHandleData(src);
		if (destData == nullptr || srcData == nullptr)
		{
			return;
		}

		if (Is<physx::PxScene>(dest) && Is<physx::PxActor>(src))
		{
			auto scene = As<physx::PxScene>(dest);
			auto actor = As<physx::PxActor>(src);

			scene->addActor(*actor);
			srcData->DependencyHandles.insert(dest);
			destData->DependencyHandles.insert(src);
		}
		else if (Is<physx::PxRigidActor>(dest) && Is<physx::PxShape>(src))
		{
			auto rigidActor = As<physx::PxRigidActor>(dest);
			auto shape = As<physx::PxShape>(src);

			rigidActor->attachShape(*shape);
			destData->DependencyHandles.insert(src);
			srcData->DependencyHandles.insert(dest);
		}
		else
		{
			JG_LOG_ERROR("is not supported  PhysicsType in Command_Attach : Dest : {0}  Src : {1}", (i32)destData->Type, (i32)srcData->Type);
		}
	}

	void PhysicsManager::Command_Detach(SharedPtr<HandleCommand> command)
	{
		auto detachCommand = command->As<DetachCommand>();
		auto dest = detachCommand->Dest;
		auto src  = detachCommand->Src;
		auto destData = GetHandleData(dest);
		auto srcData  = GetHandleData(src);
		if (destData == nullptr || srcData == nullptr)
		{
			return;
		}

		if (Is<physx::PxScene>(dest) && Is<physx::PxActor>(src))
		{
			auto scene = As<physx::PxScene>(dest);
			auto actor = As<physx::PxActor>(src);

			scene->removeActor(*actor);
			srcData->DependencyHandles.erase(dest);
			destData->DependencyHandles.erase(src);
		}
		else if (Is<physx::PxRigidActor>(dest) && Is<physx::PxShape>(src))
		{
			auto rigidActor = As<physx::PxRigidActor>(dest);
			auto shape = As<physx::PxShape>(src);

			rigidActor->detachShape(*shape);
			srcData->DependencyHandles.erase(dest);
			destData->DependencyHandles.erase(src);
		}
		else
		{
			JG_LOG_ERROR("is not supported  PhysicsType in Command_Attach : Dest : {0}  Src : {1}", (i32)destData->Type, (i32)srcData->Type);
		}
	}

	void PhysicsManager::Command_Write(SharedPtr<HandleCommand> command)
	{
		auto writeCommand = command->As<WriteCommandBase>();
		writeCommand->Excute(writeCommand->Handle);
	}

	PhysicsHandle PhysicsManager::CreateHandle(void* ptr, i32 type)
	{
		auto handle = CreateUniquePtr<HandleData>();
		handle->Ptr  = ptr;
		handle->Type = type;
		mHandleDataPool[(PhysicsHandle)ptr] = std::move(handle);
		if (type & PhysicsType_Scene)
		{
			mScenePool[(PhysicsHandle)ptr] = (physx::PxScene*)ptr;
		}
		return (PhysicsHandle)ptr;
	}
	PhysicsManager::HandleData* PhysicsManager::GetHandleData(PhysicsHandle handle)
	{
		if (mHandleDataPool.find(handle) == mHandleDataPool.end())
		{
			return nullptr;
		}
		else
		{
			return mHandleDataPool[handle].get();
		}
	}*/
}