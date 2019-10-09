#include "pch.h"
#include "Performance.h"
#include  <random>
using namespace std;
namespace JE
{
	void Performance::OnLoad()
	{
		m_FPS.resize(m_FPS_Period, 0.0f);
		m_Tick.resize(m_Tick_Period, 0.0f);
		m_TimerFuncByOneSec.Set(
			[&]() 	
		{
			// FPS
			m_FPS[m_FPS_Index] = (float)GlobalLinkData::GetFPS();
			m_Max_FPS = std::max<float>(m_FPS[m_FPS_Index], m_Max_FPS);
			m_Min_FPS = std::min<float>(m_FPS[m_FPS_Index], m_Min_FPS);
			m_FPS_Index = (++m_FPS_Index) % m_FPS_Period;
			if (m_PrevMin_FPS != m_Min_FPS) m_PrevMin_FPS = m_Min_FPS;
			else
			{
				m_MinSameFPSCount++;
				if (m_MinSameFPSCount >= m_FPS_Period)
				{
					m_MinSameFPSCount = 0;
					m_Min_FPS = m_FPS[m_FPS_Index];
				}
			}
			if (m_PrevMax_FPS != m_Max_FPS) m_PrevMax_FPS = m_Max_FPS;
			else
			{
				m_MaxSameFPSCount++;
				if (m_MaxSameFPSCount >= m_FPS_Period)
				{
					m_MaxSameFPSCount = 0;
					m_Max_FPS = m_FPS[m_FPS_Index];
				}
			}

			// MemoryInfo
			m_GoalMemInfo = GlobalLinkData::_EnginePerformance->GetMemInfo();


			// CPU
			m_GoalInfo = GlobalLinkData::_EnginePerformance->GetCPUInfo();
			m_LerpMount = 0.0f;
			if (m_ThreadColorMap.empty())
			{
				m_ThreadColorMap.resize(m_GoalInfo.CoreCount);
				//std::random_device rd;

				////
				//// Engines 
				////
				//std::mt19937 e2(rd());
				//std::uniform_real_distribution<> dist(0,1.0f);
				//


			}
			
		}, 1.0f);
		m_TimerFuncByTick.Set(
			[&]()
		{
			// Tick
			m_Tick[m_Tick_Index] = (float)GlobalLinkData::GetTick();
			m_MaxTick = std::max<float>(m_Tick[m_Tick_Index], m_MaxTick);
			m_MinTick = std::min<float>(m_Tick[m_Tick_Index], m_MinTick);

			m_Tick_Index = (++m_Tick_Index) % m_Tick_Period;

			if (m_PrevMinTick != m_MinTick) m_PrevMinTick = m_MinTick;
			else
			{
				m_MinSameTickCount++;
				if (m_MinSameTickCount >= m_Tick_Period)
				{
					ENGINE_LOG_TRACE("{0} Change", m_Tick[m_Tick_Index]);
					m_MinSameTickCount = 0;
					m_MinTick = m_Tick[m_Tick_Index];
				}
			}
			if (m_PrevMaxTick != m_MaxTick) m_PrevMaxTick = m_MaxTick;
			else
			{
				m_MaxSameTickCount++;
				if (m_MaxSameTickCount >= m_Tick_Period)
				{
					m_MaxSameTickCount = 0;
					m_MaxTick = m_Tick[m_Tick_Index];
				}
			}
			m_LerpMount += (float)GlobalLinkData::GetTick();
			m_LerpMount = std::min<float>(1.0f, m_LerpMount);

			// Memory
			m_MemInfo.PhyMem = m_MemInfo.PhyMem + (long long)((double)(m_GoalMemInfo.PhyMem - m_MemInfo.PhyMem) * (double)m_LerpMount);
			m_MemInfo.UsedPhyMem = m_MemInfo.UsedPhyMem + (long long)((double)(m_GoalMemInfo.UsedPhyMem - m_MemInfo.UsedPhyMem) * (double)m_LerpMount);
			m_MemInfo.UsedPhyMemByMe = m_MemInfo.UsedPhyMemByMe + (long long)((double)(m_GoalMemInfo.UsedPhyMemByMe - m_MemInfo.UsedPhyMemByMe) * (double)m_LerpMount);

			m_TotalUsedMemRatio = (float)m_MemInfo.UsedPhyMem / (float)m_MemInfo.PhyMem;
			m_ProcessUsedByMeMemRatio = (float)m_MemInfo.UsedPhyMemByMe / (float)m_MemInfo.PhyMem;

			//CPU
			m_CPUInfo.CoreCount = m_GoalInfo.CoreCount;
			m_CPUInfo.TotalUsage = m_CPUInfo.TotalUsage + (m_GoalInfo.TotalUsage - m_CPUInfo.TotalUsage) * m_LerpMount;
			if (m_CPUInfo.CoreUsage.empty())
				m_CPUInfo.CoreUsage.resize(m_GoalInfo.CoreCount);
			else
			{
				for (int i = 0; i < (int)m_CPUInfo.CoreCount; ++i)
				{
					m_CPUInfo.CoreUsage[i] = m_CPUInfo.CoreUsage[i] + (m_GoalInfo.CoreUsage[i] - m_CPUInfo.CoreUsage[i]) * m_LerpMount;
				}
			}

			// Tick
			if (m_CurrTimerNode)
			{
				m_TotalTickPercent = m_TotalTickPercent + (m_GoalTotalTickPercent - m_TotalTickPercent) * (m_LerpMount * 2);
				m_TotalTickPercent = std::max<float>(0.0f, m_TotalTickPercent);


				m_ParentTickPercent = m_ParentTickPercent + (m_GoalParentTickPercent - m_ParentTickPercent) * (m_LerpMount * 2);
				m_ParentTickPercent = std::max<float>(0.0f, m_ParentTickPercent);


				for (uint32_t i = 0; i < (uint32_t)m_GoalChildTickPercent.size(); ++i)
				{
					m_ChildTickPercent[i] = m_ChildTickPercent[i] + (m_GoalChildTickPercent[i] - m_ChildTickPercent[i]) * (m_LerpMount * 2);
					m_ChildTickPercent[i] = std::max<float>(0.0f, m_ChildTickPercent[i]);
				}
			}



		}, 0.0f);
		m_TimerFuncByHalfSec.Set(
			[&]() 
		{
			if (!m_CurrTimerNode)
				return;




			if (m_CurrTimerNodeName != m_PrevTimerNodeName)
			{
				ENGINE_LOG_TRACE("Chanage {0} -> {1}", m_PrevTimerNodeName, m_CurrTimerNodeName);
				m_TotalTickPercent = 0.0f;
				m_ParentTickPercent = 0.0f;
				m_ChildTickPercent.clear();
				m_GoalTotalTickPercent = 0.0f;
				m_GoalParentTickPercent = 0.0f;
				m_GoalChildTickPercent.clear();
				m_PrevTimerNodeName = m_CurrTimerNodeName;

				m_ChildTickPercent.resize(m_CurrTimerNode->Childs.size(), 0.0f);
				m_GoalChildTickPercent.resize(m_CurrTimerNode->Childs.size(), 0.0f);
			}

			if (m_CurrTimerNode->Childs.size() != m_GoalChildTickPercent.size())
				return;



			m_GoalTotalTickPercent = m_CurrTimerNode->Tick / GlobalLinkData::GetTick();
			if (m_CurrTimerNode->Parent)
			{
				m_GoalParentTickPercent = m_CurrTimerNode->Tick / m_CurrTimerNode->Parent->Tick;
			}
			std::sort(m_CurrTimerNode->Childs.begin(), m_CurrTimerNode->Childs.end(),
				[](std::shared_ptr<EnginePerformance::TimerNode> node1, std::shared_ptr<EnginePerformance::TimerNode> node2)
			{
				return node1->Tick > node2->Tick;
			});
			for (uint32_t i = 0; i < (uint32_t)m_GoalChildTickPercent.size(); ++i)
			{
				m_GoalChildTickPercent[i] = m_CurrTimerNode->Childs[i]->Tick / m_CurrTimerNode->Tick;
			}
		}, 0.5f);

		m_TimerFuncByOneSec.Start();
		m_TimerFuncByTick.Start();
		m_TimerFuncByHalfSec.Start();
	}
	
	void Performance::OnGUI()
	{
	

		auto tick = GlobalLinkData::GetTick();
		auto fps = GlobalLinkData::GetFPS();

		

		ImVec2 window_size = ImGui::GetWindowSize();
		float visual_chart_height = std::max<float>(m_InfoMaxWidth, window_size.y * 0.5f);
		if (ImGui::BeginChild("Visual Chart", { window_size.x, m_InfoMaxWidth }, true))
		{
			ImGui::Columns(2);
			
			ImGui::SetColumnWidth(0, (window_size.x * 0.5f));
			ImGui::SetColumnWidth(1, (window_size.x * 0.5f));
			DrawMemoryChart();


		
			
	
	

			ImGui::NextColumn();
			DrawCPUUsageChart();

	
		}
		ImGui::EndChild();





		ImGui::Separator();

		ImGui::BeginGroup();
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 1.f,1.f });
		if (ImGui::BeginChild("Performance Window"))
		{
			ImGui::Columns(2);
			ImGui::SetColumnWidth(0, (window_size.x * 0.5f));
			ImGui::SetColumnWidth(1, (window_size.x * 0.5f));
			DrawTimerNodes();
			ImGui::NextColumn();
			DrawTimerInfo();
		}ImGui::EndChild();
		ImGui::PopStyleVar();
		ImGui::EndGroup();




		
		if (m_StabilizeCount < 20)
			m_StabilizeCount++;
		else
		{
			m_TimerFuncByTick.Tick(tick);
			m_TimerFuncByOneSec.Tick(tick);
			m_TimerFuncByHalfSec.Tick(tick);
		}
		m_CurrTimerNode.reset();
	}

	void Performance::OnDestroy()
	{

	}
	void Performance::PushBarColorByValue(float value, float default_v, float warn_v)
	{
		PushStyleColorByValue(ImGuiCol_PlotHistogram, value, default_v, warn_v);
	}
	void Performance::PushStyleColorByValue(ImGuiCol col, float value, float default_v, float warn_v)
	{
		if (value < default_v)   ImGui::PushStyleColor(col, m_DefaultColor);
		else if (value < warn_v) ImGui::PushStyleColor(col, m_WarnColor);
		else                     ImGui::PushStyleColor(col, m_ErrorColor);
	}
	void Performance::PopColorByValue()
	{
		ImGui::PopStyleColor();
	}

	void Performance::DrawEngineTimerChart()
	{
		ImGui::BeginGroup();
		auto tick = GlobalLinkData::GetTick();
		auto fps = GlobalLinkData::GetFPS();
		float width = ImGui::GetColumnWidth();
		float chart_width = width;
		float chart_height = 50.0f;

		if (fps >= 60) ImGui::PushStyleColor(ImGuiCol_PlotLines, m_DefaultColor);
		else if(fps >= 30) ImGui::PushStyleColor(ImGuiCol_PlotLines, m_WarnColor);
		else ImGui::PushStyleColor(ImGuiCol_PlotLines, m_ErrorColor);


		ImGui::PlotLines("##Frame Times", m_FPS.data(), (int)m_FPS.size(), 0,
			("FPS : " + std::to_string(fps)).c_str(), 0, 144, { chart_width, chart_height }, 4);
		if (ImGui::IsItemHovered())
		{
			
			string min_str = "MinFPS : " + to_string(m_Min_FPS) + "\n";
			string max_str = "MaxFPS : " + to_string(m_Max_FPS);
			ImGui::SetTooltip((min_str + max_str).c_str());
		}

		ImGui::PlotLines("##Tick Times", m_Tick.data(), (int)m_Tick.size(), 0,
			("Tick : " + std::to_string(tick) + " sec").c_str(),
			m_MinTick, m_MaxTick, { chart_width, chart_height }, 4);
		if (ImGui::IsItemHovered())
		{
			string min_str = "MinTick : " + to_string(m_MinTick) + " sec \n";
			string max_str = "MaxTick : " + to_string(m_MaxTick) + " sec";

			ImGui::SetTooltip((min_str + max_str).c_str());
		}
		ImGui::PopStyleColor();
		ImGui::EndGroup();
	
	}
	void Performance::DrawCPUUsageChart()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 1.f,1.f });
		if (ImGui::BeginChild("cpu window"))
		{
			PushBarColorByValue((float)m_CPUInfo.TotalUsage, 40.0f, 80.0f);
			ImGui::Text("Total"); ImGui::SameLine(); ImGui::ProgressBar((float)m_CPUInfo.TotalUsage * 0.01f);
			PopColorByValue();
			ImGui::Separator();
			ImGui::Columns(2);
			int count = 0;
			for (auto& info : m_CPUInfo.CoreUsage)
			{
				if (count < 10) ImGui::Text("CPU %d", count++);
				else ImGui::Text("CPU%d", count++);
				ImGui::SameLine();
				PushBarColorByValue((float)info, 40.0f, 80.0f);
				ImGui::ProgressBar((float)info * 0.01f);
				PopColorByValue();
				ImGui::NextColumn();
			}
		}
		ImGui::EndChild();
		ImGui::PopStyleVar();
	}
	void Performance::DrawMemoryChart()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 1.f,1.f });
		if (ImGui::BeginChild("Timer and Memory Info"))
		{
			DrawEngineTimerChart();
			ImGui::Separator();
			ImGui::Columns(2);
			ImGui::SetColumnWidth(0, 70.0f);
			static std::string mem_unit = "KB";
			static float mem_divide = 1.0f;
			if (ImGui::BeginChild("Mem Unit Button Window"))
			{
				if (ImGui::Button("Byte", { 50.0f, 20.0f }))
				{
					mem_divide = mem_divide * 1024.0f;
					mem_unit = "Byte";
				}
				if (ImGui::Button("KB", { 50.0f, 20.0f }))
				{
					mem_divide = 1.0f;
					mem_unit = "KB";
				}
				if (ImGui::Button("MB", { 50.0f, 20.0f }))
				{
					mem_divide = 1 / 1024.0f;
					mem_unit = "MB";
				}
				if (ImGui::Button("GB", { 50.0f, 20.0f }))
				{
					mem_divide = 1 / 1024.0f / 1024.0f;
					mem_unit = "GB";
				}
			}ImGui::EndChild();
			ImGui::NextColumn();

			if (ImGui::BeginChild("Mem State Window"))
			{
				ImGui::Text("Used Total : %0.2f %s", ((float)m_MemInfo.UsedPhyMem * mem_divide), mem_unit.c_str());
				ImGui::Text("Used By Me : %.2f %s", ((float)m_MemInfo.UsedPhyMemByMe * mem_divide), mem_unit.c_str());
				ImGui::Separator();
			
				ImGui::Text("Used    "); ImGui::SameLine();
				PushBarColorByValue(m_TotalUsedMemRatio, 0.4f, 0.8f);
				ImGui::ProgressBar(m_TotalUsedMemRatio);
				PopColorByValue();
				ImGui::Text("Occupied"); ImGui::SameLine();
				PushBarColorByValue(m_ProcessUsedByMeMemRatio, 0.4f, 0.8f);
				ImGui::ProgressBar(m_ProcessUsedByMeMemRatio);
				PopColorByValue();
			}ImGui::EndChild();
		


		}ImGui::EndChild();
		ImGui::PopStyleVar();
	}
	void Performance::DrawTimerNodes()
	{
		auto e_performance = GlobalLinkData::_EnginePerformance->GetCategoryTimerMap();
		if (ImGui::BeginChild("Performance Tree", { 0,0 }, true))
		{
			for (auto& category : e_performance)
			{
				bool is_open = ImGui::CollapsingHeader(category.first.c_str());

				if (is_open)
				{
					for (auto& node : category.second)
					{
						DrawTickPerFunction(*node.second);
					}
				}
			}

		}ImGui::EndChild();

	}
	void Performance::DrawTimerInfo()
	{
		if (!m_CurrTimerNode) return;
		if (ImGui::BeginChild("TimerTick Infomation"))
		{
			std::map<std::thread::id, uint32_t> thread_ID;
			uint32_t thread_ID_count = 0;
			ImGui::Text(("Function : " + m_CurrTimerNode->Name).c_str());
			ImGui::Separator();
			ImGui::Text("TotalRatio");
			ImGui::ProgressBar(m_TotalTickPercent);
			ImGui::Separator();
			if (m_CurrTimerNode->Parent)
			{
				PushBarColorByValue(m_ParentTickPercent, 0.4f, 0.8f);
				ImGui::Text(("ParentRatio : " + m_CurrTimerNode->Parent->Name).c_str());
				ImGui::ProgressBar(m_ParentTickPercent);
				PopColorByValue();
				ImGui::Separator();
			}
			ImGui::Text("ChildRatio");
			std::sort(m_CurrTimerNode->Childs.begin(), m_CurrTimerNode->Childs.end(),
				[](std::shared_ptr<EnginePerformance::TimerNode> node1, std::shared_ptr<EnginePerformance::TimerNode> node2)
			{
				return node1->Tick > node2->Tick;
			});
			if (!m_ChildTickPercent.empty() && m_ChildTickPercent.size() == m_CurrTimerNode->Childs.size())
			{
			
				int count = 0;
				for (auto& child : m_CurrTimerNode->Childs)
				{
					auto iter = thread_ID.find(child->ThreadID);
					uint32_t id = 0;
					if (iter == thread_ID.end())
					{
						thread_ID.emplace(child->ThreadID,thread_ID_count++);
					}
					id = thread_ID[child->ThreadID];
					
					ImGui::Text(("Name : " + child->Name).c_str());
					ImGui::Columns(2);
					ImGui::SetColumnWidth(0, 100.0f);
					ImGui::Text("Thread : %d", id);
					ImGui::NextColumn();
					PushBarColorByValue(m_ChildTickPercent[count], 0.4f, 0.8f);
					ImGui::ProgressBar(m_ChildTickPercent[count++]);
					PopColorByValue();
					ImGui::Columns(1);
				}
			}
		}ImGui::EndChild();
	}
	void Performance::DrawTickPerFunction(EnginePerformance::TimerNode& node)
	{
		string name = node.Name;
		if (m_CurrTimerNodeName == name)
		{
			m_CurrTimerNode = make_shared<EnginePerformance::TimerNode>(node);
		}
		auto flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnDoubleClick;
		if (node.Childs.empty())
		{
			flags = ImGuiTreeNodeFlags_Leaf;
		}

		bool is_open = ImGui::TreeNodeEx(name.c_str(), flags);
		if (ImGui::IsItemClicked())
		{
			m_CurrTimerNodeName = name;
			ENGINE_LOG_TRACE("{0}", name);
		}
		if (is_open)
		{

			std::sort(node.Childs.begin(), node.Childs.end(),
				[](std::shared_ptr<EnginePerformance::TimerNode> node1,
					std::shared_ptr<EnginePerformance::TimerNode> node2) ->bool
			{
				return node1->Name < node2->Name;
			});


			for (auto& child : node.Childs)
			{
				DrawTickPerFunction(*child);
			}


			ImGui::TreePop();
		}
	}


}
