#include "pch.h"
#include "ReMaterial.h"
#include "Object/Shader/ShaderModule.h"
#include "Object/DxObject/PipelineState.h"
#include "Object/DxObject/TextureManager.h"
#include "Object/DxObject/Resource.h"
#include "Object/Shader/ShaderModuleManager.h"
#include "Object/DxObject/Shader.h"
#include "Object/Shader/ShaderData.h"
using namespace std;

namespace RE
{
	std::map<EReMaterialUsage, std::string> ReMaterial::sm_ModuleNameByUsage;

	ReMaterial::ReMaterial()
	{
		m_PSO = make_shared<GraphicsPipelineState>();
		m_CompiledPixelShader = Shader::MakeShader(ShaderType::Pixel);
		m_PixelCode = "";

	}

	void ReMaterial::SetBlendState(const D3D12_BLEND_DESC& desc)
	{
		m_PSO->SetBlendState(desc);
	}
	void ReMaterial::SetDepthStencilState(const D3D12_DEPTH_STENCIL_DESC& desc)
	{
		m_PSO->SetDepthStencilState(desc);
	}
	void ReMaterial::SetRasterizerState(const D3D12_RASTERIZER_DESC& desc)
	{
		m_PSO->SetRasterizerState(desc);
	}
	bool ReMaterial::AddTexture(const std::string& name, const std::string& asset_texture_path)
	{
		if (m_TextureIndexMap.find(name) != m_TextureIndexMap.end())
			return false;
		auto t = TextureManager::GetTexture(asset_texture_path);
		uint32_t index = m_BindedTextures.size();
		m_BindedTextures.push_back(t);
		m_TextureIndexMap[name] = index;


		return true;
	}

	void ReMaterial::SetCode(const std::string& code)
	{
		m_PixelCode = code;
	}
	bool ReMaterial::Compile()
	{
		auto shader_module = GetShaderModuleManager()->FindGraphicsModule(sm_ModuleNameByUsage[m_Usage]);

		if (shader_module == nullptr)
		{
			RE_LOG_ERROR("Failed Compile Material ({0}) : Can't Find ShaderModule", GetName());
		}

		auto mat_texture = shader_module->FindTexture2D(EntryShaderModule::MatTextureArrayName());
		std::string code = m_PixelCode;

		for (auto& index_pair : m_TextureIndexMap)
		{
			std::string name = index_pair.first;
			uint32_t    index = index_pair.second;
			std::string replace_name = EntryShaderModule::MatTextureArrayName() + "[" + to_string(index) + "]";
	
			size_t pos = 0;
			while ((pos = code.find(name, pos)) != std::string::npos)
			{
				char next_c = code[pos + name.length()];
				if (next_c == '.' || next_c == ' ')
				{
					code.replace(pos, name.length(), replace_name);
				}
				pos += name.length();
			}

		}
		if (!shader_module->CustomCompile(m_CompiledPixelShader, code))
		{
			RE_LOG_ERROR("Failed Compile Material ({0}) : Compile Shader", GetName());
		}
		shader_module->MakePSO(m_PSO.get());
		m_PSO->BindShader(*m_CompiledPixelShader);
		if (!m_PSO->Finalize())
		{
			RE_LOG_ERROR("Failed Compile Material ({0}) : Failed Create PipelineState", GetName());
		}

		return true;
	}
	const Texture& ReMaterial::GetBindedTexture(uint32_t idx) const
	{
		return m_BindedTextures[idx];
	}
	const Texture& ReMaterial::GetBindedTexture(const std::string& name)
	{
		if (m_TextureIndexMap.find(name) != m_TextureIndexMap.end())
		{
			assert(false && "not find name in ReMaterial::GetBindedTexture");
		}

		uint32_t index = m_TextureIndexMap[name];
		return m_BindedTextures[index];
	}
	uint32_t ReMaterial::GetBindedTextureCount() const
	{
		return (uint32_t)m_BindedTextures.size();
	}
	const std::vector<Texture>& ReMaterial::GetBindedTextures() const
	{
		return m_BindedTextures;
	}
	void ReMaterial::BindShaderModuleByUsage(EReMaterialUsage usage, const std::string& module_name)
	{
		sm_ModuleNameByUsage[usage] = module_name;
	}


}



