#pragma once


#include "JGCore.h"
#include "GraphicsDefine.h"


namespace JG
{
	class IGraphicsShader;
	class IComputeShader;
	class ITexture;
	class IComputer;
	class GraphicsHelper
	{
	public:
		static String GetShaderTemplateName(SharedPtr<IGraphicsShader> shader);



		static void InitRenderTextures(const TextureInfo& info, const String& name, List<SharedPtr<ITexture>>* out_t_list);
		static void InitComputer(const String& name, SharedPtr<IComputeShader> computeShader, List<SharedPtr<IComputer>>* out_c_list);




		static u32 GetCompeleteBufferIndex(u32 bufferIndex);
	};
}
