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



#include<vector>
namespace JGRC { 
	class JGRenderCore;
	class Mesh;
	class JGShader;
}

namespace JGLC
{
	class LOGICCORE_EXPORT JGLogicCore
	{
	public:
		JGRC::JGRenderCore * Rc;
		std::vector<JGRC::JGShader*> arr;
		JGRC::Mesh* Cube = nullptr;
		HWND hWnd;
	public:
		~JGLogicCore();
		void TestInit(JGRC::JGRenderCore* Rc,HWND hWnd);
		void TestTick();

	};

}

