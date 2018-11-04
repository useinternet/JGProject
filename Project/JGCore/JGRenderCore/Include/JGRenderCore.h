#pragma once
#include"Common/dllExportDefine.h"
#include<Windows.h>
namespace JGRC
{
	class DirectX;
	class CORE_EXPORT JGRenderCore
	{
	private:
		DirectX* m_DX;
		class Material* mt;
		class Material* plane;
	public:
		JGRenderCore();
		virtual ~JGRenderCore();

		void Init(HWND hWnd);
		void Draw();
	private:
		void modelDraw();
	};
}



