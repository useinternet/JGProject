#pragma once
#include"DxCommon/DxCommon.h"





namespace JGRC
{
	class Scene;
	class ScreenManager;
	class GpuCpuSynchronizer;
	class CommandListManager;
	class DxDevice;
	class RootSignatureManager;
	class RCORE_EXPORT CommonData
	{
	protected:
		static Scene*                m_Scene;
		static DxDevice*             m_DxDevice;
		static ScreenManager*        m_ScreenManager;
		static ResourceManager*      m_ResourceManager;
		static CommandListManager*   m_CommandListManager;
		static GpuCpuSynchronizer*   m_GCS;
		static RootSignatureManager* m_RootSigManager;
		static EngineFrameResourceManager* m_EngineFrameResourceManager;
	public:
		CommonData(
			DxDevice* core,
			Scene* scene, 
			ResourceManager* manager,
			ScreenManager* screen,
			CommandListManager* cmdManager,
			GpuCpuSynchronizer* gcs, 
			RootSignatureManager* RootSigManager,
			EngineFrameResourceManager* EFRsManager);
	public:
		static void RegisterScene(Scene* scene);
		static Scene* _Scene();
		static DxDevice* _DxDevice();
		static ScreenManager* _ScreenManager();
		static ResourceManager* _ResourceManager();
		static CommandListManager* _CmdListManager();
		static GpuCpuSynchronizer* _GCSynchronizer();
		static RootSignatureManager* _RootSigManager();
		static EngineFrameResourceManager* _EngineFrameResourceManager();
	};
}