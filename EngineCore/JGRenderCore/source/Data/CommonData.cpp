#include"CommonData.h"
#include"Scene.h"
#include"DxCore/DxCore.h"
#include"DxCore/ScreenManager.h"
using namespace JGRC;

Scene*     CommonData::m_Scene  = nullptr;
DxCore*    CommonData::m_DxCore = nullptr;
ScreenManager*    CommonData::m_ScreenManager = nullptr;
ResourceManager* CommonData::m_ResourceManager = nullptr;
CommonData::CommonData(DxCore* core, Scene* scene, ResourceManager* manager, ScreenManager* screen)
{
	m_Scene = scene;
	m_ResourceManager = manager;
	m_DxCore = core;
	m_ScreenManager = screen;
}
DxCore* CommonData::_Core()
{
	return m_DxCore;
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