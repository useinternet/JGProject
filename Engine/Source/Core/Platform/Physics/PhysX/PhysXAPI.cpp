#include "pch.h"
#include "PhysXAPI.h"

#include "physx/PxPhysXConfig.h"
#include "physx/PxPhysicsAPI.h"
#include "physx/PxPhysics.h"


using namespace physx;




namespace JG
{
	static physx::PxDefaultErrorCallback gDefaultErrorCallback;
	static physx::PxDefaultAllocator     gDefaultAllocatorCallback;
	static physx::PxFoundation* gFoundation = nullptr;
	static physx::PxPvd* gPvd = nullptr;
	static physx::PxPvdTransport* gPvdTransport = nullptr;
	static physx::PxPhysics* gPhysics = nullptr;
	static physx::PxDefaultCpuDispatcher* gCPUDispatcher = nullptr;

	bool PhysXAPI::Create()
	{
		bool result = true;
		JG_CORE_INFO("Create PhysXAPI : {0}", PX_PHYSICS_VERSION);
		auto gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gDefaultAllocatorCallback, gDefaultErrorCallback);
		if (gFoundation == nullptr)
		{
			result = false;
		}

		auto gPvd = physx::PxCreatePvd(*gFoundation);
		if (gPvd == nullptr)
		{
			result = false;
		}

#ifdef _DEBUG
		gPvdTransport = PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
		if (gPvdTransport != nullptr)
		{
			gPvd->connect(*gPvdTransport, PxPvdInstrumentationFlag::eALL);
		}
		bool recordMemoryAllocations = true;
#else
		bool recordMemoryAllocations = false;
#endif

		gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(), recordMemoryAllocations, gPvd);

		if (gPhysics == nullptr)
		{
			result = false;
		}


		gCPUDispatcher = PxDefaultCpuDispatcherCreate(Scheduler::GetInstance().GetThreadCount());
		if (gCPUDispatcher == nullptr)
		{
			result = false;
		}
		{
			//PxSceneDesc sceneDesc = PxSceneDesc(PxTolerancesScale());
//sceneDesc.filterShader = PxDefaultSimulationFilterShader;
//sceneDesc.gravity.y = -1.2f;
//sceneDesc.cpuDispatcher = gCPUDispatcher;




			//auto scene = pysics->createScene(sceneDesc);

			//auto m = pysics->createMaterial(0.0f, 0.0f, 0.0f);


			//PxBoxGeometry geo = PxBoxGeometry(50, 50, 50);
			//auto shape = pysics->createShape(geo, *m);


			//auto testBody = pysics->createRigidDynamic(PxTransform(PxVec3(0, 0, 0)));
			//testBody->attachShape(*shape);

			//scene->addActor(*testBody);



			//while (true)
			//{
			//	scene->simulate(1.0f / 60.0f);
			//	scene->fetchResults(true);
			//	auto t = testBody->getGlobalPose();
			//	JG_CORE_INFO("Test Box Position : [ {0} , {1} ,  {2} ] ", t.p.x, t.p.y, t.p.z);
			//}

			//m->release();
			//shape->release();
			//testBody->release();
			//cpuDispatcher->release();
			//scene->release();

			//pvd->release();
			//foundation->release();
			//pysics->release();
		}


		return result;
	}
	void PhysXAPI::Destroy()
	{
		if (gCPUDispatcher == nullptr)
		{
			gCPUDispatcher->release();
			gCPUDispatcher = nullptr;
		}
		if (gPhysics == nullptr)
		{
			gPhysics->release();
			gPhysics = nullptr;
		}
		if (gPvdTransport == nullptr)
		{
			gPvdTransport->release();
			gPvdTransport = nullptr;
		}
		if (gPvd == nullptr)
		{
			gPvd->release();
			gFoundation = nullptr;
		}
		if (gFoundation == nullptr)
		{
			gFoundation->release();
			gFoundation = nullptr;
		}
	}


}
	
