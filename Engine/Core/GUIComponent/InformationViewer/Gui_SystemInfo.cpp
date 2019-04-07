#include<PCH.h>
#include"Gui_SystemInfo.h"


using namespace JgGui;
using namespace std;
using namespace std::chrono;
Gui_SystemInfo::Gui_SystemInfo(const std::string& name) : GuiComponent(name)
{

}
Gui_SystemInfo::~Gui_SystemInfo()
{


}

void Gui_SystemInfo::NewFrame()
{
	
	m_Info.NewFrame();
	std::string selectByteStr;
	switch (m_byteCalcCount)
	{
	case ESB_Byte:
		selectByteStr = m_bStr;
		break;
	case ESB_KB:
		selectByteStr = m_kbStr;
		break;
	case ESB_MB:
		selectByteStr = m_mbStr;
		break;
	case ESB_GB:
		selectByteStr = m_gbStr;
		break;
	}

	float virtualMemNum = (float)m_Info.virtualTotalMem;
	float virtualMemStrUsedNum = (float)m_Info.virtualTotalMemUsed;
	float virtualMemStrUsedbyMeNum = (float)m_Info.virtualMemUsedByme;

	float phyMemNum = (float)m_Info.totalPhysMem;
	float phyMemUsedNum = (float)m_Info.totalPhysMemUsed;
	float phyMemUsedBymeNum = (float)m_Info.PhysMemUsedByme;

	// FPS 계산
	{
		m_EndPoint = m_Clock.now();
		float delta_time = (float)duration_cast<milliseconds>(m_EndPoint - m_StartPoint).count() / 1000.0f;
		m_FpsCount++;
		m_AccPoint += delta_time;
		if (m_AccPoint >= 1.0f)
		{
			m_Fps = m_FpsCount;
			m_FpsCount = 0;
			m_AccPoint = 0.0f;
		}
		m_StartPoint = m_Clock.now();
	}

	// 메모리 사용량 계산
	{
		for (int i = ESB_Byte; i < m_byteCalcCount; ++i)
		{
			virtualMemNum /= 1024;
			virtualMemStrUsedNum /= 1024;
			virtualMemStrUsedbyMeNum /= 1024;

			phyMemNum /= 1024;
			phyMemUsedNum /= 1024;
			phyMemUsedBymeNum /= 1024;
		}

	}




	string virtualMem = "TotalVirualMemory     : " + to_string(virtualMemNum) + selectByteStr;
	string virtualMemStrUsed = "TotalVirualMemoryUsed : " + to_string(virtualMemStrUsedNum) + selectByteStr;
	string virtualMemStrUsedbyMe = "VirualMemoryByMe      : " + to_string(virtualMemStrUsedbyMeNum) + selectByteStr;

	string phyMem = "TotalPhyMemory        : " + to_string(phyMemNum) + selectByteStr;
	string phyMemUsed = "TotalPhyMemoryUsed    : " + to_string(phyMemUsedNum) + selectByteStr;
	string phyMemUsedByme = "PhyMemoryUsedByMe     : " + to_string(phyMemUsedBymeNum) + selectByteStr;

	string CpuPercent = "CpuPercent : " + to_string(m_Info.GetCpuPercentage()) + " %%";
	string CpuCore = "CpuCore    : " + to_string(m_Info.GetCpuCoreNum());
	string Fps = "FPS : " + to_string(m_Fps);
	float totalvirtualMemUsedPercent = virtualMemStrUsedNum / virtualMemNum;
	float virtualMemUsedByMePercent = virtualMemStrUsedbyMeNum / virtualMemNum;
	float totalphyMemUsedPercent = phyMemUsedNum / phyMemNum;
	float phyMemUsdByMePercent = phyMemUsedBymeNum / phyMemNum;
	float cpuUsedByMePercent = m_Info.GetCpuPercentage() * 0.01f;

	// ui 그리기

	if (m_IsVisible)
	{
		GuiComponent::NewFrame();
		ImGui::Begin(m_GuiName.c_str(), &m_IsVisible);

		// 버튼 Draw
		{
			if (ImGui::Button("Byte"))
			{
				m_byteCalcCount = 0;
			}
			ImGui::SameLine();
			if (ImGui::Button("KB"))
			{
				m_byteCalcCount = 1;
			}
			ImGui::SameLine();
			if (ImGui::Button("MB"))
			{
				m_byteCalcCount = 2;
			}
			ImGui::SameLine();
			if (ImGui::Button("GB"))
			{
				m_byteCalcCount = 3;
			}
			ImGui::Separator();

		}
		ImGui::Columns(2, "colum");

		// 가상 메모리 값
		{
			ImGui::Text(virtualMem.c_str());
			ImGui::Text(virtualMemStrUsed.c_str());
			ImGui::Text(virtualMemStrUsedbyMe.c_str());
			ImGui::NextColumn();
			ImGui::Separator();
		}
		// 가상 메모리 사용량 그래프
		{
			ImGui::ProgressBar(totalvirtualMemUsedPercent, { m_ProgressBarXSize , 0 },
				("Total VirtualMem usage : " + to_string(totalvirtualMemUsedPercent * 100) + "%").c_str());
			ImGui::ProgressBar(virtualMemUsedByMePercent, { m_ProgressBarXSize , 0 },
				("VirtualMem usage : " + to_string(virtualMemUsedByMePercent * 100) + "%").c_str());
			ImGui::NextColumn();
		}

		// 메모리 값
		{
			ImGui::Text(phyMem.c_str());
			ImGui::Text(phyMemUsed.c_str());
			ImGui::Text(phyMemUsedByme.c_str());
			ImGui::NextColumn();
			ImGui::Separator();
		}
		// 메모리 사용량 그래프
		{
			ImGui::ProgressBar(totalphyMemUsedPercent, { m_ProgressBarXSize , 0 },
				("Total VirtualMem usage : " + to_string(totalphyMemUsedPercent * 100) + "%").c_str());
			ImGui::ProgressBar(phyMemUsdByMePercent, { m_ProgressBarXSize , 0 },
				("phyMem usage : " + to_string(phyMemUsdByMePercent * 100) + "%").c_str());
			ImGui::NextColumn();
		}

		// Cpu 정보
		{
			ImGui::Text(CpuPercent.c_str());
			ImGui::SameLine(ImGui::GetColumnWidth() / 2);
			ImGui::Text(Fps.c_str());
			ImGui::Text(CpuCore.c_str());
		
			ImGui::NextColumn();
			ImGui::Separator();
		}
		// cpu 사용량 그래프
		{
			ImGui::ProgressBar(cpuUsedByMePercent, { m_ProgressBarXSize , 0 },
				("CPU usage : " + to_string(cpuUsedByMePercent * 100) + "%").c_str());
			ImGui::NextColumn();
		
		}
		ImGui::Separator();
		ImGui::End();
	}
	
	ImGui::ShowDemoWindow();

}