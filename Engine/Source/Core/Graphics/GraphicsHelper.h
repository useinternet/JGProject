#pragma once


#include "JGCore.h"
#include "GraphicsDefine.h"


namespace JG
{
	class IGraphicsShader;
	class IComputeShader;
	class IStructuredBuffer;
	class IByteAddressBuffer;
	class ITexture;
	class GraphicsHelper
	{
	public:
		static String GetShaderTemplateName(SharedPtr<IGraphicsShader> shader);




		static void InitRenderTextures(const TextureInfo& info, const String& name, List<SharedPtr<ITexture>>* out_t_list);
		static void InitByteAddressBuffer(const String& name, u64 elementCount, List<SharedPtr<IByteAddressBuffer>>* out_b_list);
		static void InitStrucutredBuffer(const String& name, u64 elementCount, u64 elementSize, List<SharedPtr<IStructuredBuffer>>* out_sb_list);
	};
}
