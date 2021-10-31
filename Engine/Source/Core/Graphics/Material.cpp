#include "pch.h"
#include "Material.h"
#include "Application.h"
#include "JGGraphics.h"

namespace JG
{
	SharedPtr<IMaterial> IMaterial::Create(const String& name)
	{
		auto api = JGGraphics::GetInstance().GetGraphicsAPI();
		JGASSERT_IF(api != nullptr, "GraphicsApi is nullptr");
		return api->CreateMaterial(name);
	}
	SharedPtr<IMaterial> IMaterial::Create(const String& name, SharedPtr<IShader> shader)
	{
		auto api = JGGraphics::GetInstance().GetGraphicsAPI();
		JGASSERT_IF(api != nullptr, "GraphicsApi is nullptr");
		return api->CreateMaterial(name, shader);
	}

}