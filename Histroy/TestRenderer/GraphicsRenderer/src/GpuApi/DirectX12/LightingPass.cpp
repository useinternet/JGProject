#include"pch.h"
#include"LightingPass.h"
#include"Commander.h"
#include"GraphicsDevice.h"
#include"VertexCollection.h"
#include"RootSignatureCache.h"
#include"Commander.h"
#include"Light.h"
namespace GR
{
	namespace Dx12
	{
		std::wstring LightingPass::m_ShaderVSPath = L"QuadVS.hlsl";
		std::wstring LightingPass::m_ShaderPSPath = L"LightingPassPS.hlsl";

		void LightingPass::SetShaderVSPath(const std::wstring& path) {
			m_ShaderVSPath = path;
		}
		void LightingPass::SetShaderPSPath(const std::wstring& path) {
			m_ShaderPSPath = path;
		}


		void LightingPass::Init(uint32_t width, uint32_t height)
		{
			auto device = GraphicsDevice::GetPointer();



			GPUResource resource = device->CreateRenderTargetGPUResource(
				DXGI_FORMAT_R8G8B8A8_UNORM, width, height, 1);

			ColorTexture texture = device->CreateColorTexture(resource);

			m_RenderTarget.AttachTexture(RenderTarget::Slot0, texture);

			auto sig = device->GetRootSignatureFromCache(ERootSignature::G_Common);
			m_PSO.BindRootSignature(sig);
			m_PSO.SetInputLayout(Vertex::GetNumInputLayout(), Vertex::GetInputLayoutDesc());
			m_PSO.SetRenderTargetFormats({ 0 }, { DXGI_FORMAT_R8G8B8A8_UNORM });

			VertexShader VS;
			PixelShader  PS;
			std::wstring dir = device->GetGraphicsShaderDirPath();

			device->CreateShader(dir + m_ShaderVSPath, &VS);
			device->CreateShader(dir + m_ShaderPSPath, &PS);

			m_PSO.BindVertexShader(VS);
			m_PSO.BindPixelShader(PS);
			m_PSO.Finalize();


			m_Viewport.Set((float)width, (float)height);
			m_ScissorRect.Set(width, height);
		}
		void LightingPass::PushLight(Light* l)
		{
			m_LightCBs.push_back(l->m_Data);
		}
		void LightingPass::BindResource(ELightingPassSlot slot, const Texture& texture)
		{
			m_Textures[(int)slot] = texture;
		}
		void LightingPass::BindSkyTexture(Texture& texture)
		{
			auto desc = texture.GetResource()->GetDesc();
			D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
			srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
			srvDesc.Format = desc.Format;
			
			srvDesc.TextureCube.MipLevels = desc.MipLevels;
			srvDesc.TextureCube.MostDetailedMip = 0;
			srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;
			
			m_SkyTexture = texture;
			m_SkyTexture.SetSRVDesc(&srvDesc);
		}
		void LightingPass::Resize(uint32_t width, uint32_t height)
		{
			assert(false && "lightingpass resize not imp");
		}
		void LightingPass::Execute(GraphicsCommander* commander, const PassCB& passcb)
		{
			auto device = GraphicsDevice::GetPointer();
			auto rootsig = device->GetRootSignatureFromCache(ERootSignature::G_Common);
			commander->SetViewport(m_Viewport);
			commander->SetScissorRect(m_ScissorRect);

			commander->ClearColor(*m_RenderTarget.GetTexture(RenderTarget::Slot0));
			commander->SetRenderTarget(m_RenderTarget.GetTexture(RenderTarget::Slot0)->GetRTV());
			commander->SetRootSignature(rootsig);
			commander->SetPipelineState(m_PSO);
			commander->SetDynamicConstantBuffer(G_Common_RootParam_PassCB, sizeof(PassCB), &passcb);
			commander->SetDynamicStructuredBuffer(G_Common_RootParam_LightCB, (uint32_t)m_LightCBs.size(),
				sizeof(LightCB), m_LightCBs.data());

			std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> srvs(G_Common_Value_NumTextureSlot);
			std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> cubesrvs(G_Common_Value_NumTextureSlot);
			for (uint32_t i = 0; i < (uint32_t)ELightingPassSlot::Num_Slot; ++i)
			{
				if (m_Textures[i].IsValid())
				{
					srvs[i] = m_Textures[i].GetSRV();
				}
			}
			
			if (m_SkyTexture.IsValid())
			{
				cubesrvs[0] = m_SkyTexture.GetSRV();
			}
			commander->SetSRVDescriptorTable(G_Common_RootParam_Texture, (uint32_t)srvs.size(), srvs.data());
			commander->SetSRVDescriptorTable(G_Common_RootParam_CubeTexture, (uint32_t)cubesrvs.size(),
				cubesrvs.data());

			commander->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			commander->Draw(6);
			m_LightCBs.clear();
			// юлго
		}

		const ColorTexture* LightingPass::GetTexture() const
		{
			return m_RenderTarget.GetTexture(RenderTarget::Slot0);
		}
		ColorTexture* LightingPass::GetTexture()
		{
			return m_RenderTarget.GetTexture(RenderTarget::Slot0);
		}
	}
}