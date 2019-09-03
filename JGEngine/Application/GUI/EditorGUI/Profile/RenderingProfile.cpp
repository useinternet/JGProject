#include "pch.h"
#include "RenderingProfile.h"


using namespace std;

namespace JE
{
	void RenderingProfile::TestPerformance(std::shared_ptr<EnginePerformance::TimerNode> node)
	{
		//auto name = node->Name + " : ";
		//auto tick = to_string(node->Tick) + " sec";
		//ImGui::Text(name.c_str()); ImGui::SameLine(); ImGui::Text(tick.c_str());
		//for (auto& ss : node->Childs)
		//{
		//	TestPerformance(ss);
		//}
	}
	void RenderingProfile::OnLoad()
	{
		AsyncEvent asyncEvent;
		asyncEvent.Function = [&]() {
			RequestDescriptorAllocatorDebugInfoEvent e;
			e.SentIGWindowID = (uint64_t)this;
			e.SentIGWindow   = GetName();
			EventNotify(e);

			RequestAllResourceDebugInfoEvent e2;
			e2.SentIGWindowID = (uint64_t)this;
			e2.SentIGWindow = GetName();
			EventNotify(e2);
		};
		m_LoadDebugInfoEventHandler = GlobalLinkData::EngineEventManager->GetAsyncEventHandler(asyncEvent);
		m_LoadDebugInfoEventHandler.Excute();
		m_FunctionTimer.Set([&]() {
			if (m_LoadDebugInfoEventHandler.IsComplete())
			{

				m_LoadDebugInfoEventHandler.Reset();

				for (int i = 0; i < Allocator_NUM; ++i)
				{
					m_DescriptorAllocatorInfo[i] = std::move(m_ThreadDumpInfo[i]);
				}
				m_GUIAllocatorInfo = std::move(m_GUIThreadDumpInfo);
				m_ResourceInfos = std::move(m_ThreadDumpResourceInfos);
				m_LoadDebugInfoEventHandler.Excute();
			}
		}, 0.5f);
		m_FunctionTimer.Start();
	}
	void RenderingProfile::OnGUI()
	{
		uint64_t select_address = 0;
		
		if(ImGui::BeginTabBar((GetName() + "_TabBar").c_str()))
		{

			if (ImGui::BeginTabItem("Allocator State"))
			{
				if (ImGui::TreeNodeEx("GUI", ImGuiTreeNodeFlags_DefaultOpen))
				{
					GUIAllocatorDebugInfo(m_GUIAllocatorInfo);
					ImGui::TreePop();
				}
				for (int i = 0; i < Allocator_NUM; ++i)
				{
				
					if (ImGui::TreeNodeEx(m_DescriptorAllocatorInfo[i].DescriptorAllocatorType.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
					{
						DescriptorAllocatorDebugInfo(m_DescriptorAllocatorInfo[i], select_address);
						ImGui::TreePop();
					}
				}
		
				ImGui::EndTabItem();
			}
	
			if (ImGui::BeginTabItem("All Resource States"))
			{
				AllResourceDebugInfo(m_ResourceInfos, select_address);
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("LogCheck"))
			{
				if (ImGui::Button("Trace"))
				{
					ENGINE_LOG_TRACE("LOG");
				}
				if (ImGui::Button("Info"))
				{
					ENGINE_LOG_INFO("LOG");
				}
				if (ImGui::Button("Error"))
				{
					ENGINE_LOG_ERROR("LOG");
				}
				if (ImGui::Button("Warning"))
				{
					ENGINE_LOG_WARN("LOG");
				}
				if (ImGui::Button("fatal"))
				{
					ENGINE_LOG_FATAL("LOG");
				}

		/*		auto node = GlobalLinkData::_EnginePerformance->GetRootTimeNode();
				TestPerformance(node);*/

				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}

		if (ImGui::IsMouseDoubleClicked(0))
		{
			m_SavedAddress = 0;
		}
		if (select_address)
			m_SavedAddress = select_address;


		//
		m_FunctionTimer.Tick(GlobalLinkData::GetTick());
	}
	void RenderingProfile::OnEventFromRE(ToEditFromReEvent& e)
	{
		switch (e.GetCommand())
		{
		case ToEditFromReCommand::SendAllocatorDebugInfo:
			SendAllocatorDebugInfo(CONVERT_EVENT(SendAllocatorDebugInfoEvent, e));
			return;
		case ToEditFromReCommand::SendAllResourceDebugInfo:
			SendAllResourceDebugInfo(CONVERT_EVENT(SendAllResourceDebugInfoEvent, e));
			return;
		}
	}
	void RenderingProfile::OnDestroy()
	{


	}
	void RenderingProfile::GUIAllocatorDebugInfo(const Debug::GUIAllocatorInfo& info)
	{
		float allocate_space = 1.0f - (float)m_GUIAllocatorInfo.num_allocated_descriptor / (float)m_GUIAllocatorInfo.numDescriptor;


		ImGui::BeginGroup();
		std::string str = "Remaining Space : " + std::to_string(allocate_space * 100) + " %%";
		ImGui::Text(str.c_str());
		ImGui::Separator();

		ImGui::Columns(3, 0, true);
		ImGui::Text("BindingResourceName");
		ImGui::NextColumn();
		ImGui::Text("CpuAddress");
		ImGui::NextColumn();
		ImGui::Text("GpuAddress");
		ImGui::NextColumn();
		ImGui::Separator();

		ImVec4 color = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);

		ImGui::PushStyleColor(ImGuiCol_Text, color);
		for (auto& allcoation : m_GUIAllocatorInfo.AllocationInfo)
		{
			std::string BindingResourceName = allcoation.bind_reosurce_name;
			std::string CpuAddress = std::to_string(allcoation.cpu_address);
			std::string GpuAddress = std::to_string(allcoation.gpu_address);
			ImGui::Text(BindingResourceName.c_str());
			ImGui::NextColumn();
			ImGui::Text(CpuAddress.c_str());
			ImGui::NextColumn();
			ImGui::Text(GpuAddress.c_str());
			ImGui::NextColumn();
			ImGui::Separator();
		}
		ImGui::PopStyleColor();

		ImGui::Columns(1);
		ImGui::EndGroup();
	}
	void RenderingProfile::DescriptorAllocatorDebugInfo(const Debug::DescritporAllocatorInfo& info, uint64_t& select_address)
	{
		ImVec2 window_size = ImGui::GetWindowSize();
		for (auto& heap_info : info.descriptor_heapInfo)
		{
			float allocate_space = 1.0f - (float)heap_info.num_allocated_descriptor / (float)heap_info.max_numDescriptor;
			// num | remaining space | 
			std::string str = "HeapID : " + std::to_string(heap_info.num);
			if (!ImGui::TreeNodeEx(str.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
			{
				return;
			}
			ImGui::BeginGroup();
			ImGui::Separator();
			str = "Remaining Space : " + std::to_string(allocate_space * 100) + " %%";
			ImGui::Text(str.c_str());
			ImGui::Separator();


			//       start_offset, numDescriptor, cpu_start_address,
			ImGui::Columns(3, 0, true);
			ImGui::Text("StartOffset");
			ImGui::NextColumn();
			ImGui::Text("NumDescriptor");
			ImGui::NextColumn();
			ImGui::Text("CpuAddress");
			ImGui::NextColumn();
			ImGui::Separator();
			
			for (auto& block_pair : heap_info.descriptor_blocks)
			{
				auto& block = block_pair.second;
				std::string offset = std::to_string(block.start_offset);
				std::string numDescriptor = std::to_string(block.numDescriptor);
				std::string cpu_address = std::to_string(block.address);
		
				ImVec4 color = (block.is_allocate) ?
					ImVec4(1.0f, 0.0f, 0.0f, 1.0f) :
					ImVec4(0.0f, 1.0f, 0.0f, 1.0f);


				ImGui::PushStyleColor(ImGuiCol_Text, color);
				ImGui::Text(offset.c_str());
				ImGui::NextColumn();
				ImGui::Text(numDescriptor.c_str());
				ImGui::NextColumn();

				bool select_table = (m_SavedAddress == 0 || m_SavedAddress != block.address) ?
					false : true;

                if (ImGui::Selectable(cpu_address.c_str(), select_table, ImGuiSelectableFlags_AllowDoubleClick)
	                && ImGui::IsMouseDoubleClicked(0))
                {
	                select_address = block.address;
                }
                ImGui::NextColumn();
                ImGui::Separator();

                ImGui::PopStyleColor();

			}

			ImGui::Columns(1);
			ImGui::EndGroup();
			ImGui::TreePop();
		}
	}
	void RenderingProfile::AllResourceDebugInfo(const std::vector<Debug::ResourceInfo>& info, uint64_t& select_address)
	{
		static char input_str[256];
		ImGui::InputText("##SearchText", &input_str[0], 256);
		ImGui::Separator();
		string str = "ResourceTotalCount : " + to_string(info.size());
		// name  ref  srv uav rtv dsv   tooltip -> desc
		ImGui::Text(str.c_str());
		ImGui::Separator();
		ImGui::Columns(6);
		ImGui::Text("Name");
		ImGui::NextColumn();
		ImGui::Text("RefCount");
		ImGui::NextColumn();
		ImGui::Text("SRV");
		ImGui::NextColumn();
		ImGui::Text("UAV");
		ImGui::NextColumn();
		ImGui::Text("RTV");
		ImGui::NextColumn();
		ImGui::Text("DSV");
		ImGui::NextColumn();
		ImGui::Separator();

		ImVec4 green = { 0.0f,1.0f,0.0f,1.0f };
		ImVec4 red = { 1.0f,0.0f,0.0f,1.0f };
		bool is_tool_tip = false;
		for (auto& resource : info)
		{
			if (resource.name.find(input_str) == std::string::npos)
			{
				continue;
			}
			ImGui::TextColored(green, resource.name.c_str());
			if (ImGui::IsItemHovered())
			{
				std::string tool_tip = R"(** DescInfo **
Name : %s 
Dimension : %s 
Width : %d   Height : %d 
Array Size : %d 
MipLevels : %d 
Format : %s 
SampleCount : %d   SampleQuality : %d 
TextureLayout : %s 
Flags : %s )";
				ImGui::SetTooltip(tool_tip.c_str(),
					resource.name.c_str(),
					resource.dimension.c_str(),
					resource.width, resource.height,
					resource.array_size,
					resource.miplevels,
					resource.format.c_str(),
					resource.sampleCount,
					resource.sampleQuality,
					resource.texture_layout.c_str(),
					resource.flags.c_str());
			}
			ImGui::NextColumn();

			ImGui::Text(to_string(resource.ref_count).c_str());
			ImGui::NextColumn();


			ImGui::PushStyleColor(ImGuiCol_Text, red);
			for (auto& srv : resource.srv_addresses)
			{
				bool select_table = (m_SavedAddress == 0 || m_SavedAddress != srv) ?
					false : true;

				if (ImGui::Selectable(to_string(srv).c_str(),
					select_table,
					ImGuiSelectableFlags_AllowDoubleClick) &&
					ImGui::IsMouseDoubleClicked(0))
				{
					select_address = srv;
				}

				
			}
			ImGui::NextColumn();
			for (auto& uav : resource.uav_addresses)
			{
				bool select_table = (m_SavedAddress == 0 || m_SavedAddress != uav) ?
					false : true;
				if (ImGui::Selectable(to_string(uav).c_str(),
					select_table,
					ImGuiSelectableFlags_AllowDoubleClick) &&
					ImGui::IsMouseDoubleClicked(0))
				{
					select_address = uav;
				}
			}
			ImGui::NextColumn();
			for (auto& rtv : resource.rtv_addresses)
			{
				bool select_table = (m_SavedAddress == 0 || m_SavedAddress != rtv) ?
					false : true;
				if (ImGui::Selectable(to_string(rtv).c_str(),
					select_table,
					ImGuiSelectableFlags_AllowDoubleClick) &&
					ImGui::IsMouseDoubleClicked(0))
				{
					select_address = rtv;
				}
			}
			ImGui::NextColumn();
			for (auto& dsv : resource.dsv_addresses)
			{
				bool select_table = (m_SavedAddress == 0 || m_SavedAddress != dsv) ?
					false : true;
				if (ImGui::Selectable(to_string(dsv).c_str(),
					select_table,
					ImGuiSelectableFlags_AllowDoubleClick) &&
					ImGui::IsMouseDoubleClicked(0))
				{
					select_address = dsv;
				}
			}

			ImGui::PopStyleColor();
			ImGui::NextColumn();
			ImGui::Separator();
		}
		ImGui::Columns(1);
	}

	void RenderingProfile::SendAllocatorDebugInfo(SendAllocatorDebugInfoEvent& e)
	{
		m_ThreadDumpInfo[SRV] = std::move(e.SrvDebugInfo);
		m_ThreadDumpInfo[UAV] = std::move(e.UavDebugInfo);
		m_ThreadDumpInfo[RTV] = std::move(e.RtvDebugInfo);
		m_ThreadDumpInfo[DSV] = std::move(e.DsvDebugInfo);
		m_GUIThreadDumpInfo = std::move(e.GUIDebugInfo);
	}
	void RenderingProfile::SendAllResourceDebugInfo(SendAllResourceDebugInfoEvent& e)
	{
		m_ThreadDumpResourceInfos = std::move(e.ResourceInfoArray);
	}
}