#include "pch.h"
#include "ReObject.h"
#include "DxObject/RenderDevice.h"
#include "Object/Shader/ShaderLib.h"
namespace RE
{
	RenderDevice* ReObject::m_RenderDevice = nullptr;
	ShaderLibManager* ReObject::m_ShaderLibManager = nullptr;
	ShaderModuleManager* ReObject::m_ShaderModuleManager = nullptr;
	std::unordered_map<uint64_t, RenderItemManager*> ReObject::m_RenderItemManager;
}