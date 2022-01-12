#include "pch.h"
#include "Computer.h"
#include "JGGraphics.h"


namespace JG
{
	SharedPtr<IComputer> IComputer::Create(const String& name, SharedPtr<IComputeShader> shader)
	{
		auto api = JGGraphics::GetInstance().GetGraphicsAPI();
		JGASSERT_IF(api != nullptr, "GraphicsApi is nullptr");

		return api->CreateComputer(name, shader);
	}

}