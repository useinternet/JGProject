#include"pch.h"
#include"ToneMapping.h"
#include"Commander.h"
#include"GraphicsDevice.h"
#include"VertexCollection.h"
#include"RootSignatureCache.h"
#include"Commander.h"
#include"DepthStencilState.h"
using namespace std;

namespace GR
{
	namespace Dx12
	{
		std::wstring ToneMapping::m_ShaderVSPath = L"QuadVS.hlsl";
		std::wstring ToneMapping::m_ShaderPSPath = L"ToneMappingPS.hlsl";

		void ToneMapping::SetShaderVSPath(const std::wstring& path)
		{
			m_ShaderVSPath = path;
		}
		void ToneMapping::SetShaderPSPath(const std::wstring& path)
		{
			m_ShaderPSPath = path;
		}


		void ToneMapping::Init(uint32_t width, uint32_t height)
		{
			
			auto device = GraphicsDevice::GetPointer();


			GPUResource resource = device->CreateRenderTargetGPUResource(
				DXGI_FORMAT_R8G8B8A8_UNORM, width, height, 1);
			ColorTexture texture = device->CreateColorTexture(resource);

			m_RenderTarget.AttachTexture(RenderTarget::Slot0, texture);
			auto sig = device->GetRootSignatureFromCache(ERootSignature::G_Common);
			{
				m_Viewport.Set((float)width, (float)height);
				m_ScissorRect.Set(width, height);
			}
			{
				m_PSO.BindRootSignature(sig);
				m_PSO.SetRenderTargetFormats({ 0 }, { DXGI_FORMAT_R8G8B8A8_UNORM });
				m_PSO.SetDepthStencilState(DepthStencilState::NoneDepth());
				
				VertexShader VS;
				PixelShader PS;
				std::wstring dir = device->GetGraphicsShaderDirPath();
				device->CreateShader(dir + m_ShaderVSPath, &VS);
				device->CreateShader(dir + m_ShaderPSPath, &PS);

				m_PSO.BindVertexShader(VS);
				m_PSO.BindPixelShader(PS);
				m_PSO.Finalize();
			}
		}
		void ToneMapping::BindTexture(const Texture& texture)
		{
			m_Texture = texture;
			m_Texture.SetSRVDesc(nullptr);
		}
		void ToneMapping::Resize(uint32_t width, uint32_t height)
		{
			assert(false && "tonemapping not imp resize");
		}
		void ToneMapping::Execute(GraphicsCommander* commander, const PassCB& passcb)
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

			commander->SetSRVDescriptorTable(G_Common_RootParam_Texture, 1, &m_Texture.GetSRV());
			commander->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			commander->Draw(6);
		}
		const ColorTexture* ToneMapping::GetTexture() const
		{
			return m_RenderTarget.GetTexture(RenderTarget::Slot0);
		}
		ColorTexture* ToneMapping::GetTexture()
		{
			return m_RenderTarget.GetTexture(RenderTarget::Slot0);
		}
	}
}