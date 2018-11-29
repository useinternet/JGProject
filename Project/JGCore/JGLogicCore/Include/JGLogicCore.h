#pragma once

#include<JGCommon/JGCommon.h>
#include<Windows.h>
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
	class Mesh;
	class JGShaderArray;
	class SpotLight;
}
namespace JGLC
{
	class LOGICCORE_EXPORT JGLogicCore
	{
	public:
		JGRC::JGRenderCore * Rc;
		JGRC::JGShaderArray* CubeShader;
		JGRC::Mesh* Cube = nullptr;

		JGRC::Mesh* rabbit = nullptr;
		JGRC::JGShaderArray* rabbitShader;
		HWND hWnd;
		JGRC::SpotLight* sample;

		// 카메라 위치
		jgMatrix4x4 viewMatrix;
	public:
		~JGLogicCore();
		void TestInit(JGRC::JGRenderCore* Rc,HWND hWnd);
		void TestTick();


		void GroundInit();
		void GroundHlslInit();
		void GroundParamInit();
		void GroundRender(real yaw);
		void RabbitInit();
		void RabiitHlslInit();
		void RabbitParamInit();
		void RabbitRender(real yaw);

	};

}

