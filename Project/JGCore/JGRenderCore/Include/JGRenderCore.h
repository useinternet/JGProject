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

		void Init();
		class DxWindow* AddDxWindow(const DxWinConfig& config) const;
		class DxWindow* GetDxWindow(HWND hWnd);
		class HlslEditor* GetHlslEditor(const EShaderType type, const std::string& path) const;
		void Draw();
	};
}



