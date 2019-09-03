#pragma once

#include "GUI/EditorGUI/IGWindow.h"




namespace JE
{
	class Performance : public IGWindow
	{
	protected:
		virtual void OnLoad() override;
		virtual void OnGUI() override;
		virtual void OnDestroy() override;
	private:
		void DrawEngineTimerChart();
		void DrawCPUUsageChart();
		void DrawMemoryChart();
		void DrawTimerNodes();
		void DrawTimerInfo();
	private:
		void PushBarColorByValue(float value, float default_v, float warn_v);
		void PushStyleColorByValue(ImGuiCol col, float value, float default_v, float warn_v);
		void PopColorByValue();
		void DrawTickPerFunction(EnginePerformance::TimerNode& node);
	private:
		uint32_t m_FPS_Period = 60;
		uint32_t m_FPS_Index = 0;
		uint32_t m_Min_FPS = 9999999;
		uint32_t m_Max_FPS = 0.0f;
		uint32_t m_PrevMin_FPS = 0.0f;
		uint32_t m_PrevMax_FPS = 0.0f;
		uint32_t m_MinSameFPSCount = 0;
		uint32_t m_MaxSameFPSCount = 0;
		std::vector<float> m_FPS;
		std::vector<float> m_Tick;

		uint32_t m_Tick_Period = 300;
		uint32_t m_Tick_Index = 0;
		float m_MaxTick = 0.0f;
		float m_MinTick = 99999.0f;
		float m_PrevMaxTick = 0.0f;
		float m_PrevMinTick = 0.0f;
		uint32_t m_MinSameTickCount = 0;
		uint32_t m_MaxSameTickCount = 0;

		uint32_t m_StabilizeCount = 0;
		TimerByFunction m_TimerFuncByOneSec;
		TimerByFunction m_TimerFuncByTick;
		TimerByFunction m_TimerFuncByHalfSec;
		std::string m_CurrTimerNodeName = "None";
		std::shared_ptr<EnginePerformance::TimerNode> m_CurrTimerNode;
		std::string m_PrevTimerNodeName = "None";
		float m_TotalTickPercent = 0.0f;
		float m_ParentTickPercent = 0.0f;
		std::vector<float> m_ChildTickPercent;
		float m_GoalTotalTickPercent = 0.0f;
		float m_GoalParentTickPercent = 0.0f;
		std::vector<float> m_GoalChildTickPercent;

		// CPU
		EnginePerformance::CPUInfo m_CPUInfo;
		EnginePerformance::CPUInfo m_GoalInfo;
		float m_LerpMount = 0.0f;

		// Memory
		EnginePerformance::MemoryInfo m_MemInfo;
		EnginePerformance::MemoryInfo m_GoalMemInfo;
		float m_TotalUsedMemRatio = 0.0f;
		float m_ProcessUsedByMeMemRatio = 0.0f;
			

		// 설정 변수
		ImVec4 m_DefaultColor = { 0.0f,1.0f,0.0f,1.0f };
		ImVec4 m_WarnColor = { 1.0f,1.0f,0.0f,1.0f };
		ImVec4 m_ErrorColor = { 1.0f,0.0f,0.0f,1.0f };
		float m_InfoMaxWidth = 220.0f;
		std::vector<ImVec4> m_ThreadColorMap;
		//
	};
}

