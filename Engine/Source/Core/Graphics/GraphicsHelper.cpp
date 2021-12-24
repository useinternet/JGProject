#include "pch.h"
#include "GraphicsHelper.h"
#include "Shader.h"


namespace JG
{
	String GraphicsHelper::GetShaderTemplateName(SharedPtr<IGraphicsShader> shader)
	{
		if (shader == nullptr) {
			return "";
		}
		String name = shader->GetName();


		List<String> splitedName = StringExtend::Split(name, '/');

		if (splitedName.empty() == true)
		{
			return "";
		}

		return splitedName[0];
	}
}