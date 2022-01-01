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


		List<String> splitedName = StringExtend::Split(name, '/');

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


		u32 bufferCnt = JGGraphics::GetInstance().GetBufferCount() + 1;

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

	void GraphicsHelper::InitComputer(const String& name, SharedPtr<IComputeShader> computeShader, List<SharedPtr<IComputer>>* out_c_list)
	{
		if (out_c_list == nullptr)
		{
			return;
		}
		if (computeShader == nullptr || computeShader->IsSuccessed() == false)
		{
			return;
		}


		u32 bufferCnt = JGGraphics::GetInstance().GetBufferCount() + 1;
		

		List<SharedPtr<IComputer>>& c_list = *out_c_list;
		c_list.resize(bufferCnt);


		i32 index = 0;
		for (auto& c : c_list)
		{
			c = IComputer::Create(name + "_" + std::to_string(index), computeShader);
			++index;
		}
	}

	u32 GraphicsHelper::GetCompeleteBufferIndex(u32 bufferIndex)
	{
		u32 bufferCnt = JGGraphics::GetInstance().GetBufferCount();
		return (bufferIndex + bufferCnt) % (bufferCnt + 1);
	}

}