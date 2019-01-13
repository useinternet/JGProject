#include"CommonData.h"
#include"Scene.h"
#include"DxCore/DxCore.h"
using namespace JGRC;

Scene*     CommonData::m_Scene = nullptr;
DxCore*    CommonData::m_DxCore = nullptr;
ResourceManager* CommonData::m_ResourceManager = nullptr;
CommonData::CommonData(DxCore* core, Scene* scene, ResourceManager* manager)
{
	m_DxCore = core;
	m_Scene = scene;
	m_ResourceManager = manager;
}
Scene* CommonData::_Scene() 
{
	return m_Scene;
}
DxCore* CommonData::_Core() 
{
	return m_DxCore;
}
ID3D12Device* CommonData::_Device() 
{
	return m_DxCore->Device();
}
ResourceManager* CommonData::_ResourceManager() 
{
	return m_ResourceManager;
}