#pragma once


#include "JGCore.h"
#include "GraphicsDefine.h"


namespace JG
{
	namespace Graphics
	{
		class StaticRenderObject;
	}
	class IGraphicsShader;
	class IComputeShader;
	class IStructuredBuffer;
	class IByteAddressBuffer;
	class ITexture;
	class ITopLevelAccelerationStructure;
	class IShaderScript;
	class IShader;
	class GraphicsHelper
	{
	public:
		static String GetShaderTemplateName(SharedPtr<IGraphicsShader> shader);
		static SharedPtr<Graphics::StaticRenderObject> CreateSkyBox(const JVector3& eyePos, f32 farZ, const String& texturePath);


		static void InitTopLevelAccelerationStructure(List<SharedPtr< ITopLevelAccelerationStructure>>* out_as_list);
		static void InitRenderTextures(const TextureInfo& info, const String& name, List<SharedPtr<ITexture>>* out_t_list);
		static void InitByteAddressBuffer(const String& name, u64 elementCount, List<SharedPtr<IByteAddressBuffer>>* out_b_list);
		static void InitStrucutredBuffer(const String& name, u64 elementCount, u64 elementSize, List<SharedPtr<IStructuredBuffer>>* out_sb_list, EBufferLoadMethod method = EBufferLoadMethod::GPULoad);





		static u32 NumMantissaBitsInFloatFormat(u32 FloatFormatBitLength);
	};
}
