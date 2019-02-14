#include"CommonData.h"
#include"Scene.h"
#include"DxCore/DxDevice.h"
#include"DxCore/ScreenManager.h"
#include"DxCore/CommandListManager.h"
#include"DxCore/GpuCpuSynchronizer.h"
#include"DxCore/RootSignatureManager.h"
#include"ResourceManagement/DataManager.h"
using namespace JGRC;

Scene*                CommonData::m_Scene              = nullptr;
DxDevice*             CommonData::m_DxDevice           = nullptr;
ScreenManager*        CommonData::m_ScreenManager      = nullptr;
ResourceManager*      CommonData::m_ResourceManager    = nullptr;
DataManager*          CommonData::m_DataManager        = nullptr;
CommandListManager*   CommonData::m_CommandListManager = nullptr;
GpuCpuSynchronizer*   CommonData::m_GCS                = nullptr;
RootSignatureManager* CommonData::m_RootSigManager     = nullptr;
EngineFrameResourceManager* CommonData::m_EngineFrameResourceManager = nullptr;
CommonData::CommonData(
	DxDevice* core,
	Scene* scene,
	ResourceManager* manager,
	DataManager*     DataManager,
	ScreenManager* screen,
	CommandListManager* cmdManager,
	GpuCpuSynchronizer* gcs,
	RootSignatureManager* RootSigManager,
	EngineFrameResourceManager* EFRsManager)
{
	m_Scene = scene;
	m_ResourceManager = manager;
	m_DataManager = DataManager;
	m_DxDevice = core;
	m_ScreenManager = screen;
	m_CommandListManager = cmdManager;
	m_GCS = gcs;
	m_RootSigManager = RootSigManager;
	m_EngineFrameResourceManager = EFRsManager;
}
void CommonData::RegisterScene(Scene* scene)
{
	m_Scene = scene;
}
DxDevice* CommonData::_DxDevice()
{
	return m_DxDevice;
}
Scene* CommonData::_Scene() 
{
	return m_Scene;
}
ScreenManager* CommonData::_ScreenManager()
{
	return m_ScreenManager;
}
ResourceManager* CommonData::_ResourceManager() 
{
	return m_ResourceManager;
}
DataManager* CommonData::_DataManager()
{
	return m_DataManager;
}
CommandListManager* CommonData::_CmdListManager()
{
	return m_CommandListManager;
}
GpuCpuSynchronizer* CommonData::_GCSynchronizer()
{
	return m_GCS;
}
RootSignatureManager* CommonData::_RootSigManager()
{
	return m_RootSigManager;
}
EngineFrameResourceManager* CommonData::_EngineFrameResourceManager()
{
	return m_EngineFrameResourceManager;
}