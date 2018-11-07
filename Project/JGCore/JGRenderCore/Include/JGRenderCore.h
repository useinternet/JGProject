#pragma once
#include"Common/dllExportDefine.h"
#include<Windows.h>
#include<memory>
namespace JGRC
{
	class DirectX;
	class CORE_EXPORT JGRenderCore
	{
	private:
		DirectX* m_DX;
		class Material* mt;
		class Material* plane;
		class SRSScene*  scene;
		class SRSRenderTarget* RTT;
		std::unique_ptr<class Mesh> rabbit;
		std::unique_ptr<class Mesh> ground;
	public:
		JGRenderCore();
		virtual ~JGRenderCore();

		void Init(HWND hWnd);
		void Draw();
	private:
		// ป๙วร
		void SceneDraw();
		void modelDraw();
		void rabbitDraw(float yaw);
		void groundDraw(float yaw);
	};
}



