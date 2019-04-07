#pragma once
#include"GuiComponent.h"
#include<chrono>
#include<Util/SystemInfo.h>

namespace JgGui
{
	class Gui_SystemInfo : public GuiComponent
	{
		enum ESystemByte
		{
			ESB_Byte = 0,
			ESB_KB,
			ESB_MB,
			ESB_GB
		};
	private:
		Common::Util::SystemInfo m_Info;
		const std::string m_bStr  = " Byte";
		const std::string m_kbStr = " KB";
		const std::string m_mbStr = " MB";
		const std::string m_gbStr = " GB";
		std::chrono::high_resolution_clock m_Clock;
		std::chrono::high_resolution_clock::time_point m_StartPoint;
		std::chrono::high_resolution_clock::time_point m_EndPoint;
		float m_AccPoint = 0.0f;
		int   m_FpsCount = 0;
		int   m_Fps = 0;
		int m_byteCalcCount = ESB_MB;
		float m_ProgressBarXSize = 300;
	public:
		Gui_SystemInfo(const std::string& name = "DeviceInformation");
		virtual ~Gui_SystemInfo();
	public:
		virtual void NewFrame() override;
	};
}