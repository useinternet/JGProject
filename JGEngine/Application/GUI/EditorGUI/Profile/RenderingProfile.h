#pragma once
#include "GUI/EditorGUI/IGWindow.h"



namespace JE
{
	class RenderingProfile : public IGWindow
	{
		enum {
			SRV, UAV, RTV, DSV, Allocator_NUM
		};
		ImVec2 m_MinStateBarSize = { 512, 32 };
		Debug::GUIAllocatorInfo m_GUIAllocatorInfo;
		Debug::GUIAllocatorInfo m_GUIThreadDumpInfo;
		Debug::DescritporAllocatorInfo m_DescriptorAllocatorInfo[Allocator_NUM];
		Debug::DescritporAllocatorInfo m_ThreadDumpInfo[Allocator_NUM];
		std::vector<Debug::ResourceInfo> m_ResourceInfos;
		std::vector<Debug::ResourceInfo> m_ThreadDumpResourceInfos;
		AsyncEventHandler m_LoadDebugInfoEventHandler;
	
		//
		uint64_t m_SavedAddress = 0;



		//timer 
		TimerByFunction m_FunctionTimer;
	protected:
		virtual void OnLoad() override;
		virtual void OnGUI() override;
		virtual void OnEventFromRE(ToEditFromReEvent& e) override;
		virtual void OnDestroy() override;
	private:
		void GUIAllocatorDebugInfo(const Debug::GUIAllocatorInfo& info);
		void DescriptorAllocatorDebugInfo(const Debug::DescritporAllocatorInfo& info, uint64_t& select_address);
		void AllResourceDebugInfo(const std::vector<Debug::ResourceInfo>& info, uint64_t& select_address);
	protected:
	    void SendAllocatorDebugInfo(SendAllocatorDebugInfoEvent& e);
		void SendAllResourceDebugInfo(SendAllResourceDebugInfoEvent& e);

	};
}