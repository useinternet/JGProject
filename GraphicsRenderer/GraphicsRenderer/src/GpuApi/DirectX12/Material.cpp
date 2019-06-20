#include"pch.h"
#include"Material.h"
#include"RootSignatureCache.h"
#include"GBuffer.h"
#include"VertexCollection.h"
#include"GraphicsDevice.h"
#include"Commander.h"
namespace GR
{
	namespace Dx12
	{
		Material::Material()
		{
			m_isBuild = false;
		}
		void Material::BindTexture(ETextureSlot slot, const Texture& texture)
		{
			bool is_new = true;
			if (!m_Textures[(int)slot].IsValid())
			{
				RequestRebuild();
			}
			m_Textures[(int)slot] = texture;
		}
		void Material::BindCubeTexture(ETextureSlot slot, const Texture& texture)
		{
			bool is_new = true;
			if (!m_CubeTextures[(int)slot].IsValid())
			{
				RequestRebuild();
			}
			m_CubeTextures[(int)slot] = texture;

		    auto desc = m_CubeTextures[(int)slot]->GetDesc();
	        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
			srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
			srvDesc.Format = desc.Format;

			srvDesc.TextureCube.MipLevels = desc.MipLevels;
			srvDesc.TextureCube.MostDetailedMip = 0;
			srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;

			m_CubeTextures[(int)slot].SetSRVDesc(&srvDesc);
		}
		void Material::SetShader(const std::wstring& vsName, const std::wstring& psName)
		{
			RequestRebuild();
			m_ShaderVSName = vsName;
			m_ShaderPSName = psName;
		}
		void Material::SetDesc(const MaterialDesc& desc)
		{
			if (m_MatDesc != desc)
				RequestRebuild();
			m_MatDesc = desc;
		}
		void Material::RequestRebuild()
		{
			m_isBuild = false;
		}
		void Material::BindResourceAtGPU(GraphicsCommander* commander)
		{
			if (!m_isBuild)
				Build();
			commander->SetPipelineState(m_PSO);

			auto srvs = GetTextureSRV();
			auto cube_srvs = GetCubeTextureSRV();


			commander->SetSRVDescriptorTable(G_Common_RootParam_Texture, (uint32_t)srvs.size(), srvs.data());
			commander->SetSRVDescriptorTable(G_Common_RootParam_CubeTexture, (uint32_t)cube_srvs.size(),
				cube_srvs.data());
		}
		std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> Material::GetTextureSRV() const
		{
			std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> handles(G_Common_Value_NumTextureSlot);
			for (uint32_t i = 0; i < (uint32_t)ETextureSlot::Num_Slot; ++i)
			{
				if (m_Textures[i].IsValid())
				{
					handles[i] = m_Textures[i].GetSRV();
				}
				else
				{
					handles[i].ptr = 0;
				}

			}
			return handles;
		}
		std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> Material::GetCubeTextureSRV() const
		{
			std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> handles(G_Common_Value_NumTextureSlot);
			for (uint32_t i = 0; i < (uint32_t)ETextureSlot::Num_Slot; ++i)
			{
				if (m_CubeTextures[i].IsValid())
				{
					handles[i] = m_CubeTextures[i].GetSRV();
				}
				else
				{
					handles[i].ptr = 0;
				}

			}
			return handles;
		}
		GraphicsPSO& Material::GetPSO()
		{
			Build();
			return m_PSO;
		}
		void Material::Build()
		{
			if (m_isBuild)
				return;
			
			auto device = GraphicsDevice::GetPointer();
			std::vector<D3D_SHADER_MACRO> macro;
			GraphicsPSO pso = GBuffer::GetPSO();


			// 스케레탈 메시인지 아닌지..
			if (m_MatDesc.is_skeletal)
			{
				pso.SetInputLayout(SkinnedVertex::GetNumInputLayout(), SkinnedVertex::GetInputLayoutDesc());
				macro.push_back({ USE_SKELETAL ,  MACRO_DEFINE });
			}
			else
			{
				pso.SetInputLayout(Vertex::GetNumInputLayout(), Vertex::GetInputLayoutDesc());
			}
			// 텍스쳐
			std::vector<std::string> mem((int)ETextureSlot::Num_Slot * 2);
			for (uint32_t i = 0; i < (int)ETextureSlot::Num_Slot; ++i)
			{
				if (m_Textures[i].IsValid())
				{
					mem[i * 2] =  USE_SLOT(i);
					macro.push_back({ mem[i * 2].c_str(), MACRO_DEFINE });
		
				}
				if (m_CubeTextures[i].IsValid())
				{
					mem[i * 2 + 1] = USE_CUBESLOT(i);
					macro.push_back({ mem[i * 2 + 1].c_str(), MACRO_DEFINE });
				}
			}
			if (m_Textures[(int)ETextureSlot::SpecularBRDF].IsValid() && m_CubeTextures[(int)ETextureSlot::SpecularMap].IsValid() &&
				m_CubeTextures[(int)ETextureSlot::IrradianceMap].IsValid())
			{
				macro.push_back({ USE_IBL, MACRO_DEFINE });
			}



			macro.push_back({ nullptr, nullptr });
			// 셰이더 컴파일
			auto shaderDirpath = device->GetGraphicsShaderDirPath();
			VertexShader vs;
			device->CreateShader(shaderDirpath + m_ShaderVSName, &vs,macro);

			PixelShader ps;
			device->CreateShader(shaderDirpath + m_ShaderPSName, &ps, macro);

			if (vs.GetBufferPointer() == nullptr || ps.GetBufferPointer() == nullptr)
			{
				assert(false && "failed create shader");
				// log
				return;
			}
			pso.BindVertexShader(vs);
			pso.BindPixelShader(ps);


			pso.Finalize();
			m_PSO = pso;
			m_isBuild = true;
		}
	}
}