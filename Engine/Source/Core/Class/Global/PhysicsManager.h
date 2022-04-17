#pragma once
#include "JGCore.h"
//#include "physx/PxPhysXConfig.h"
//#include "physx/PxPhysicsAPI.h"
//#include "physx/PxPhysics.h"

namespace JG
{
	using PhysicsHandle = u64;


	class PxShapeData
	{
	public:
		//SharedPtr<physx::PxGeometry> Geometry = nullptr;
		PhysicsHandle MaterialHandle = 0;
	};
	class PhysicsManager : public IGlobalSingleton<PhysicsManager>
	{
	private:
		friend class Application;

		enum
		{
			PhysicsType_None        = 0,
			PhysicsType_Scene       = 0x00001,
			PhysicsType_Actor       = 0x00002,
			PhysicsType_RigidActor  = 0x00008,
			PhysicsType_RigidStatic = 0x00010 | PhysicsType_Actor | PhysicsType_RigidActor,
			PhysicsType_RigidDynamic= 0x00020 | PhysicsType_Actor | PhysicsType_RigidActor,
			PhysicsType_Shape       = 0x00040,
			PhysicsType_Material    = 0x00080,
		};
		enum class ECommandType
		{
			Remove,
			Attach,
			Detach,
			Write,
		};
		class HandleData
		{
		public:
			i32 Type    = PhysicsType_None;
			void* Ptr   = nullptr;
			HashSet<PhysicsHandle> DependencyHandles;
		};
		class HandleCommand : public IJGObject
		{
			JGCLASS
		public:
			virtual ECommandType GetCommandType() const = 0;
		};

		class RemoveCommand : public HandleCommand
		{
			JGCLASS
		public:
			PhysicsHandle Handle;
		public:
			virtual ECommandType GetCommandType() const override { return ECommandType::Remove; }
		};

		class AttachCommand : public HandleCommand
		{
			JGCLASS
		public:
			PhysicsHandle Dest;
			PhysicsHandle Src;

			virtual ECommandType GetCommandType() const override { return ECommandType::Attach; }
		};

		class DetachCommand : public HandleCommand
		{
			JGCLASS
		public:
			PhysicsHandle Dest;
			PhysicsHandle Src;

			virtual ECommandType GetCommandType() const override { return ECommandType::Detach; }
		};


		class WriteCommandBase : public HandleCommand
		{
			JGCLASS
		public:
			PhysicsHandle Handle = 0;
		public:
			virtual ECommandType GetCommandType() const override { return ECommandType::Write; }
			virtual void Excute(PhysicsHandle handle) = 0;
		};

		template<class PxClass>
		class WriteCommand : public WriteCommandBase
		{
		public:
			std::function<void(PxClass*)> Action;
		public:
			virtual void Excute(PhysicsHandle handle) override
			{
				if (Action != nullptr && PhysicsManager::GetInstance().Is<PxClass>(handle))
				{
					Action(PhysicsManager::GetInstance().As<PxClass>(handle));
				}
			}
		};










		//physx::PxDefaultErrorCallback mDefaultErrorCallback;
		//physx::PxDefaultAllocator     mDefaultAllocatorCallback;
		//physx::PxFoundation*		  mFoundation = nullptr;
		//physx::PxPvd*				  mPvd = nullptr;
		//physx::PxPvdTransport*		  mPvdTransport = nullptr;
		//physx::PxPhysics*			  mPhysics = nullptr;
		//physx::PxDefaultCpuDispatcher* mCPUDispatcher = nullptr;


		//Dictionary<i32, u64> mPhysicsTypeHashCode;
		//Dictionary<PhysicsHandle, UniquePtr<HandleData>>  mHandleDataPool;
		//Dictionary<PhysicsHandle, physx::PxScene*>        mScenePool;


		//Queue<SharedPtr<HandleCommand>> mCommandQueue;

	public:
		PhysicsManager();
		virtual ~PhysicsManager();
	public:
		//physx::PxPhysics* GetPxPhysics() const;
	public:
	//	PhysicsHandle CreatePxScene();
	//	PhysicsHandle CreatePxRigidStatic(const JVector3& p, const JQuaternion& q);
	//	PhysicsHandle CreatePxRigidDynamic(const JVector3& p, const JQuaternion& q);
	//	PhysicsHandle CreatePxShape(const std::function<PxShapeData()>& returnAction);
	//	PhysicsHandle CreatePxMaterial(f32 staticFriction, f32 dynamicFriction, f32 restitution);





	//	void PushRemoveCommand(PhysicsHandle handle);
	//	void PushAttachCommand(PhysicsHandle dest, PhysicsHandle src);
	//	void PushDetachCommand(PhysicsHandle dest, PhysicsHandle src);
	//	template<class PxClass>
	//	bool PushWriteCommand(PhysicsHandle handle, const std::function<void(PxClass*)>& action)
	//	{
	//		if (Is<PxClass>(handle) == false)
	//		{
	//			return false;
	//		}
	//		auto command = CreateSharedPtr<WriteCommand<PxClass>>();
	//		command->Handle = handle;
	//		command->Action = action;
	//		mCommandQueue.push(command);

	//		return true;
	//	}
	//	template<class PxClass>
	//	bool ExcuteReadCommand(PhysicsHandle handle, const std::function<void(const PxClass*)>& action)
	//	{
	//		if (Is<PxClass>(handle) == false)
	//		{
	//			return false;
	//		}
	//		if (action != nullptr)
	//		{
	//			action(As<PxClass>(handle));
	//		}
	//	}

	//private:
	//	void Init();
	//	void Command_Remove(SharedPtr<HandleCommand> command);
	//	void Command_Attach(SharedPtr<HandleCommand> command);
	//	void Command_Detach(SharedPtr<HandleCommand> command);
	//	void Command_Write(SharedPtr<HandleCommand> command);

	//	PhysicsHandle CreateHandle(void* ptr, i32 type);
	//	HandleData*   GetHandleData(PhysicsHandle handle);
	//public:
	//	template<class PxClass>
	//	bool Is(PhysicsHandle handle)
	//	{
	//		auto data = GetHandleData(handle);
	//		if (data == nullptr)
	//		{
	//			return false;
	//		}
	//		if (mPhysicsTypeHashCode.find(data->Type) == mPhysicsTypeHashCode.end())
	//		{
	//			return false;
	//		}
	//		bool isFind = false;
	//		for (auto& _pair : mPhysicsTypeHashCode)
	//		{
	//			if (_pair.first & data->Type && _pair.second == typeid(PxClass).hash_code())
	//			{
	//				isFind = true;
	//				break;
	//			}
	//		}
	//		return isFind;
	//	}

	//	template<class PxClass>
	//	PxClass* As(PhysicsHandle handle)
	//	{
	//		if (Is<PxClass>(handle) == true)
	//		{
	//			auto data = GetHandleData(handle);
	//			return (PxClass*)data->Ptr;
	//		}
	//		else
	//		{
	//			return nullptr;
	//		}
	//	}


	};
}