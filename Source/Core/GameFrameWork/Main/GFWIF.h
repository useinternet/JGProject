#pragma once




#include "GFWCore.h"
#include "GFWClasses/GFWObject.h"
namespace GFW
{
	class GameWorld;
	class CameraManager;
	/*
	렌더링 오브젝트 관리
	*/
	class GFWIF
	{
		friend class GameFrameWork;
		class GEObjectPool
		{
		public:
			std::unordered_map<GE::SceneObject*, SceneObjectRef> SceneObjectPool;
			std::unordered_map<GE::Scene*, SceneRef>             ScenePool;
		};
	private:
		static void Create();
		static void Destroy();
		static void Update();

		static void LoadWorld(const std::wstring& name);
	public:
		static void RegisterInstanceWithGraphicsEngine(GFWObject* instance);
		static void UnRegisterInstanceWithGraphicsEngine(GFWObject* instance);

		static GE::Scene*       Request3DSceneWithGraphicsEngine(GFWObject* instance);
		static GE::SceneObject* RequestSceneObjectWithGraphicsEngine(GFWObject* instance);

		static GameWorld* GetGameWorld();
		/*
		GameLayer
		UILayer
		DebugLayer
		
		*/
	private:
		std::unordered_map<GFWObject*, std::unique_ptr<GEObjectPool>> GEObjectPoolByObject;
		std::map<uint32_t, std::queue<std::unique_ptr<GEObjectPool>>> DestroyGEObjectRefQueue;
		uint32_t BufferIndex = 0;
		uint32_t BufferCount = 3;
		std::unique_ptr<GameWorld>     World;
		std::unique_ptr<CameraManager> _CameraManager;


		 
		/*
		디테일 패널
		뷰포트 패널

		*/
	};
}