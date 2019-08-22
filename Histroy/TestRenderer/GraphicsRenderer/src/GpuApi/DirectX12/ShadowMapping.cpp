#include"pch.h"
#include"GraphicsDevice.h"
#include"ShadowMapping.h"
#include"DepthStencilState.h"
#include"RootSignatureCache.h"
#include"Commander.h"
#include"DepthFromLight.h"
#include"Light.h"
namespace GR
{
	namespace Dx12
	{
		std::wstring ShadowMapping::m_ShaderVSPath = L"QuadVS.hlsl";
		std::wstring ShadowMapping::m_ShaderPSPath = L"ShadowMappingPS.hlsl";

		void ShadowMapping::SetShaderVSPath(const std::wstring& path)
		{
			m_ShaderVSPath = path;
		}
		void ShadowMapping::SetShaderPSPath(const std::wstring& path)
		{
			m_ShaderPSPath = path;
		}

		void ShadowMapping::Init(uint32_t width, uint32_t height)
		{
			auto device = GraphicsDevice::GetPointer();
			GPUResource resource = device->CreateRenderTargetGPUResource(DXGI_FORMAT_R32_FLOAT, width, height, 1);
			ColorTexture texture = device->CreateColorTexture(resource);

			m_Rendertarget.AttachTexture(RenderTarget::Slot0, texture);

			m_RootSignature = device->GetRootSignatureFromCache(ERootSignature::G_ShadowMapping);

			{
				VertexShader VS;
				PixelShader PS;

				auto dir = device->GetGraphicsShaderDirPath();
				device->CreateShader(dir + m_ShaderVSPath, &VS);
				device->CreateShader(dir + m_ShaderPSPath, &PS);


				m_PSO.BindVertexShader(VS);
				m_PSO.BindPixelShader(PS);
				m_PSO.BindRootSignature(m_RootSignature.GetD3DRootSignature());

				m_PSO.SetRenderTargetFormats({ 0 }, { DXGI_FORMAT_R32_FLOAT });
				m_PSO.SetDepthStencilState(DepthStencilState::NoneDepth());

				m_PSO.Finalize();
			}
			{
				m_Viewport.Set((float)width, (float)height);
				m_ScissorRect.Set(width, height);
			}
		}
		void ShadowMapping::Resize(uint32_t width, uint32_t height)
		{

		}
		bool operator<(Light const &l1, Light const &l2)
		{
			return l1.GetType() < l2.GetType();
		}
		void ShadowMapping::Execute(GraphicsCommander* commander, const PassCB& passcb)
		{
			commander->SetViewport(m_Viewport);
			commander->SetScissorRect(m_ScissorRect);
			commander->SetRootSignature(m_RootSignature);
			commander->SetPipelineState(m_PSO);
			ShadowMappingCB ShadowCB;
			{
				ShadowCB.ShadowSize = Light::DepthSize;
				ShadowCB.CamInvViewProj = passcb.InvViewProj;
				ShadowCB.LightCount = 0;
			}
			{
				commander->ClearColor(*m_Rendertarget.GetTexture(RenderTarget::Slot0));
				commander->SetRenderTarget(m_Rendertarget.GetTexture(RenderTarget::Slot0)->GetRTV());
			}
			// 데이터 바인딩
			{
				std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> srvs;
				std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> cubesrvs;
				std::vector<PassCB> pass;
				std::vector<LightCB> lightcb;
	
				while (!m_LightQue.empty())
				{
					Light* l = m_LightQue.front(); m_LightQue.pop();
					lightcb.push_back(l->m_Data);
					switch(l->m_Data.Type)
					{
					case LightCB::Directional:
					case LightCB::Spot:
						srvs.push_back(l->GetTexture()->GetSRV());
						break;
					case LightCB::Point:
						cubesrvs.push_back(l->GetTexture()->GetSRV());
						break;
					}
					pass.push_back(l->GetPassCB());
					ShadowCB.LightCount++;
				}
	

		
				commander->SetDynamicConstantBuffer(G_ShadowMapping_RootParam_ShadowMappingCB, sizeof(ShadowMappingCB), &ShadowCB);
				commander->SetDynamicStructuredBuffer(G_ShadowMapping_RootParam_LightCB, lightcb.size(), sizeof(LightCB), lightcb.data());
				commander->SetDynamicStructuredBuffer(G_ShadowMapping_RootParam_LightPassCB, pass.size(), sizeof(PassCB), pass.data());
				commander->SetSRVDescriptorTable(G_ShadowMapping_RootParam_DepthTexture, 1, &m_DepthTexture.GetSRV());
				commander->SetSRVDescriptorTable(G_ShadowMapping_RootParam_LightDepthCubeTexture, cubesrvs.size(), cubesrvs.data());
				commander->SetSRVDescriptorTable(G_ShadowMapping_RootParam_LightDepthTexture, srvs.size(), srvs.data());
			}
			commander->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			commander->Draw(6);

		}
		void ShadowMapping::PushLight(Light* l)
		{
			m_LightQue.push(l);
		}
		void ShadowMapping::BindDepthTexture(Texture& texture)
		{
			m_DepthTexture = texture;
		}
		Texture* ShadowMapping::GetTexture()
		{
			return m_Rendertarget.GetTexture(RenderTarget::Slot0);
		}
		const Texture* ShadowMapping::GetTexture() const
		{
			return m_Rendertarget.GetTexture(RenderTarget::Slot0);
		}

	}
}