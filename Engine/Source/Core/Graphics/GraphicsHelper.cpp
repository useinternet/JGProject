#include "pch.h"
#include "GraphicsHelper.h"
#include "JGGraphics.h"
#include "RayTracing/TopLevelAccelerationStructure.h"
#include "Shader.h"


namespace JG
{
	String GraphicsHelper::GetShaderTemplateName(SharedPtr<IGraphicsShader> shader)
	{
		if (shader == nullptr) {
			return "";
		}
		String name = shader->GetName();


		List<String> splitedName = StringHelper::Split(name, '/');

		if (splitedName.empty() == true)
		{
			return "";
		}

		return splitedName[0];
	}
	void GraphicsHelper::InitTopLevelAccelerationStructure(List<SharedPtr<ITopLevelAccelerationStructure>>* out_as_list)
	{
		if (out_as_list == nullptr)
		{
			return;
		}


		u32 bufferCnt = JGGraphics::GetInstance().GetBufferCount();

		List<SharedPtr<ITopLevelAccelerationStructure>>& as_list = *out_as_list;
		as_list.resize(bufferCnt);

		i32 index = 0;
		for (auto& as : as_list)
		{
			as = ITopLevelAccelerationStructure::Create();
		}
	}
	void GraphicsHelper::InitRenderTextures(const TextureInfo& info, const String& name,  List<SharedPtr<ITexture>>* out_t_list)
	{
		if (out_t_list == nullptr)
		{
			return;
		}


		u32 bufferCnt = JGGraphics::GetInstance().GetBufferCount();

		List<SharedPtr<ITexture>>& t_list = *out_t_list;
		t_list.resize(bufferCnt);

		i32 index = 0;
		for (auto& t : t_list)
		{
			if (t == nullptr) t = ITexture::Create(name + "_" + std::to_string(index), info);
			else t->SetTextureInfo(info);

			++index;
		}
	}

	void GraphicsHelper::InitByteAddressBuffer(const String& name, u64 elementCount, List<SharedPtr<IByteAddressBuffer>>* out_b_list)
	{
		if (out_b_list == nullptr)
		{
			return;
		}

		u32 bufferCnt = JGGraphics::GetInstance().GetBufferCount();

		List<SharedPtr<IByteAddressBuffer>>& b_list = *out_b_list;
		b_list.resize(bufferCnt);

		i32 index = 0;
		for (auto& b : b_list)
		{
			b = IByteAddressBuffer::Create(name + "_" + std::to_string(index), elementCount);
			++index;
		}
	}

	void GraphicsHelper::InitStrucutredBuffer(const String& name, u64 elementCount, u64 elementSize, List<SharedPtr<IStructuredBuffer>>* out_sb_list, EBufferLoadMethod method)
	{
		if (out_sb_list == nullptr)
		{
			return;
		}
		u32 bufferCnt = JGGraphics::GetInstance().GetBufferCount();

		List<SharedPtr<IStructuredBuffer>>& sb_list = *out_sb_list;
		sb_list.resize(bufferCnt);


		i32 index = 0;
		for (auto& sb : sb_list)
		{
			sb = IStructuredBuffer::Create(name + "_" + std::to_string(index), elementSize, elementCount, method);
			++index;
		}
	}
	u32 GraphicsHelper::NumMantissaBitsInFloatFormat(u32 FloatFormatBitLength)
	{
		switch (FloatFormatBitLength)
		{
		case 32: return 23;
		case 16: return 10;
		case 11: return 6;
		case 10: return 5;
		}

		return 0;
	}
}