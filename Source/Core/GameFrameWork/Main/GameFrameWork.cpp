#include "pch.h"
#include "GameFrameWork.h"
#include "GFWIF.h"
#include "GFWClasses/World.h"
#include "GFWClasses/CameraManager.h"
#include "GFWClasses/GameObject.h"
#include "Components/StaticMeshComponent.h"
using namespace std;

namespace GFW
{
	void GameFrameWork::Awake()
	{
		GFWIF::Create();

	}

	void GameFrameWork::Start()
	{
		
	}

	void GameFrameWork::Update()
	{
	

		GFWIF::Update();




	}

	void GameFrameWork::Destroy()
	{
		GFWIF::Destroy();

	}
	void GameFrameWork::ReceiveMessage(const Message& msg)
	{
		switch (msg.msgID)
		{
		case GameFrameWorkMessage::Msg_RequestEditorCamera:


			break;
		case GameFrameWorkMessage::Msg_GetSelectGameObject:


			break;
		case GameFrameWorkMessage::Msg_TestSpawnGameObject:


			break;
		}

	}
}
namespace GFW
{
	static std::unique_ptr<GFWIF> g_Instance = nullptr;
	void GFWIF::Create()
	{
		if(g_Instance == nullptr)
		{
			g_Instance = make_unique<GFWIF>();
		}

		g_Instance->_CameraManager = make_unique<CameraManager>();
		g_Instance->_CameraManager->Awake();

		GlobalSharedData::GetEventManager()->RegisterEvent(TT("GFW::GetWorldCamera"),
			Event<GE::Scene*>
			([&]() -> GE::Scene*
		{
			return g_Instance->_CameraManager->GetWorldCamera()->scene;
		}));


		GlobalSharedData::GetEventManager()->RegisterEvent(TT("GFW::LoadWorld"),
			Event<void, const std::wstring&>
			([&](const std::wstring& worldName)
		{
			LoadWorld(worldName);
		}));

		GlobalSharedData::GetEventManager()->RegisterEvent(TT("GFW::SpawnObject(Test)"),
			Event<void, const std::wstring&, JVector3>
			([&](const std::wstring& worldName, JVector3 location)
		{
			
		}));

		GlobalSharedData::GetEventManager()->DispatchEvent(TT("D::GFW::SpawnObject(Test)"),
			Event<void, const std::wstring&, JVector3 >( [&](const std::wstring& path, JVector3 location)
		{

			auto world = GFWIF::GetGameWorld();
			auto obj = world->GetSelectedWorld()->SpawnGameObject<GameObject>(TT("Å×½ºÆ®"));
			auto com = obj->CreateComponent<StaticMeshComponent>(TT("StaticMesh"));
			com->SetLocalLocation(location);
			std::vector<MeshGenerator::StaticMeshData> m;
			MeshGenerator::LoadFromFile(path, m);
			com->SetMesh(m);
		}));

	}

	void GFWIF::Destroy()
	{
		if(g_Instance->World) g_Instance->World->Destroy();

		g_Instance->_CameraManager->Destroy();
		g_Instance.reset();
		g_Instance = nullptr;
	}

	void GFWIF::Update()
	{
		auto& destroyQueue = g_Instance->DestroyGEObjectRefQueue[g_Instance->BufferIndex];

		while(!destroyQueue.empty())
		{
			destroyQueue.pop();
		}
	
		if (g_Instance->World)
		{
			g_Instance->World->Tick();
		}
		g_Instance->_CameraManager->Tick();
		g_Instance->BufferIndex = (g_Instance->BufferIndex + 1) % g_Instance->BufferCount;
	}

	void GFWIF::LoadWorld(const std::wstring& name)
	{
		if (g_Instance->World)
		{
			g_Instance->World->Destroy();
			g_Instance->World = nullptr;
		}
		g_Instance->World = make_unique<GameWorld>(name);
		g_Instance->World->Awake();
	}

	void GFWIF::RegisterInstanceWithGraphicsEngine(GFWObject* instance)
	{
		auto iter = g_Instance->GEObjectPoolByObject.find(instance);
		if (iter != g_Instance->GEObjectPoolByObject.end()) return;




		g_Instance->GEObjectPoolByObject[instance] = std::make_unique<GEObjectPool>();
	}
	void GFWIF::UnRegisterInstanceWithGraphicsEngine(GFWObject* instance)
	{
		auto iter = g_Instance->GEObjectPoolByObject.find(instance);
		if (iter == g_Instance->GEObjectPoolByObject.end()) return;

		g_Instance->DestroyGEObjectRefQueue[g_Instance->BufferIndex].push(std::move(iter->second));
		g_Instance->GEObjectPoolByObject.erase(iter);
	
	}
	GE::Scene* GFWIF::Request3DSceneWithGraphicsEngine(GFWObject* instance)
	{
		auto iter = g_Instance->GEObjectPoolByObject.find(instance);
		if (iter == g_Instance->GEObjectPoolByObject.end()) return nullptr;
		auto ref = GraphicsIF::RequestScene(GE::RendererMode::_3D);
		auto result = ref.Get();
		iter->second->ScenePool[ref.Get()] = move(ref);


		return result;
	}
	GE::SceneObject* GFWIF::RequestSceneObjectWithGraphicsEngine(GFWObject* instance)
	{
		auto iter = g_Instance->GEObjectPoolByObject.find(instance);
		if (iter == g_Instance->GEObjectPoolByObject.end()) return nullptr;

		auto ref = GraphicsIF::RequestSceneObject();
		auto result = ref.Get();
		iter->second->SceneObjectPool[ref.Get()] = move(ref);


		return result;
	}
	GameWorld* GFWIF::GetGameWorld() 
	{
		return g_Instance->World.get();
	}
}


