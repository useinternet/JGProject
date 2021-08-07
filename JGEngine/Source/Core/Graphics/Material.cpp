#include "pch.h"
#include "Material.h"
#include "Application.h"


namespace JG
{
	SharedPtr<IMaterial> IMaterial::Create(const String& name, SharedPtr<IShader> shader)
	{
		auto api = Application::GetInstance().GetGraphicsAPI();
		JGASSERT_IF(api != nullptr, "GraphicsApi is nullptr");
		return api->CreateMaterial(name, shader);
	}

}