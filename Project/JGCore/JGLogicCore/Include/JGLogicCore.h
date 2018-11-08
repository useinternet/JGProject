#pragma once



#pragma once
#ifdef JGLOGICCORE_EXPORTS 
#define LOGICCORE_EXPORT __declspec(dllexport)
#else
#define LOGICCORE_EXPORT __declspec(dllimport)
#endif
#pragma warning (disable : 4996 4005 4251)

#ifdef _DEBUG
#pragma comment(lib,"JGCommon/JGCommon_d.lib")
#else
#pragma comment(lib,"JGCommon/JGCommon.lib")
#endif
#ifdef _DEBUG
#pragma comment(lib,"JGRenderCore_d.lib")
#else
#pragma comment(lib,"JGRenderCore.lib")
#endif


namespace JGRC { 
	class JGRenderCore;
	class Material;
	class Mesh;
}

namespace JGLC
{
	class LOGICCORE_EXPORT JGLogicCore
	{
	private:
		JGRC::JGRenderCore* m_RenderCore;
		class JGRC::Material* ground_M;
		class JGRC::Mesh* groundMesh;
		class JGRC::Material* rabbit_M;
		class JGRC::Mesh* rabbitMesh;
	public:
		~JGLogicCore();
		void TestInit(JGRC::JGRenderCore* Rc);
		void TestTick();

		void GroundSetting(float yaw);
		void RabbitSetting(float yaw);
	};

}

