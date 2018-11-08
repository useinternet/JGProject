#pragma once
#include"Common/JGRCGlobalType.h"
#include<JGCommon/JGCommon.h>
#include<Windows.h>
#include<string>
namespace JGRC
{
	// 나중에 스왑체인으로 (윈도우) 나누기
	class CORE_EXPORT JGRenderCore
	{
	private:
		class DirectX* m_DX;
		class JGSRSystem* m_SRSytsem;
	private:
		JGRenderCore(const JGRenderCore& copy) = delete;
		JGRenderCore& operator=(const JGRenderCore& rc) = delete;
	public:
		JGRenderCore();
		virtual ~JGRenderCore();

		void Init(HWND hWnd);
		void Push(class Material* mt);
		void Draw();
		bool OutputMaterialData(std::string* hlslPath,EShaderType* type,const UINT Count,const std::string& OuputMaterial);
		class Material* CreateMaterial(const std::string& mtPath);


	private:
		void ManagerInit();


		// 임시
	public:
		jgMatrix4x4 GetProjectionMatrix();
	};
}



