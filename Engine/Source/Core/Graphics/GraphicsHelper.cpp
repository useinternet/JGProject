#include "pch.h"
#include "GraphicsHelper.h"
#include "JGGraphics.h"
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

	void GraphicsHelper::InitStrucutredBuffer(const String& name, u64 elementCount, u64 elementSize, List<SharedPtr<IStructuredBuffer>>* out_sb_list)
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
			sb = IStructuredBuffer::Create(name + "_" + std::to_string(index), elementSize, elementCount);
			++index;
		}
	}
}