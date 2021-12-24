#pragma once


#include "JGCore.h"
#include "GraphicsDefine.h"


namespace JG
{
	class IGraphicsShader;
	class GraphicsHelper
	{
	public:
		static String GetShaderTemplateName(SharedPtr<IGraphicsShader> shader);

	};
}
