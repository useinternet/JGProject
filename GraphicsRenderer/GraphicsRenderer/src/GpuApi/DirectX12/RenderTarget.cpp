#include"pch.h"
#include"RenderTarget.h"
using namespace std;

namespace GR
{
	namespace Dx12
	{
		void RenderTarget::AttachTexture(ESlot slot, const ColorTexture& texture)
		{
			m_ColorTexture[slot] = texture;
			m_NumRtvs++;
		}
		void RenderTarget::AttachTexture(ESlot slot, const DepthTexture& texture)
		{
			m_DepthTexture = texture;
		}

		const ColorTexture* RenderTarget::GetTexture(ESlot slot) const
		{
			if (!m_ColorTexture[slot].IsValid() || slot == DepthStencil || slot == NumRenderTarget)
			{
				return nullptr;
			}

			return &m_ColorTexture[slot];
		}
		ColorTexture* RenderTarget::GetTexture(ESlot slot)
		{
			if (!m_ColorTexture[slot].IsValid() || slot == DepthStencil || slot == NumRenderTarget)
			{
				return nullptr;
			}

			return &m_ColorTexture[slot];
		}
		DepthTexture* RenderTarget::GetDepthTexture()
		{
			if (m_DepthTexture.IsValid())
			{
				return &m_DepthTexture;
			}
			return nullptr;
		}

		uint32_t RenderTarget::GetNumRTVs() const
		{
			return m_NumRtvs;
		}
		std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> RenderTarget::GetRTVs()
		{
			vector<D3D12_CPU_DESCRIPTOR_HANDLE> v;

			for (int i = 0; i < NumRenderTarget; ++i)
			{
				if(m_ColorTexture[i].IsValid())
				{ 
					v.push_back(m_ColorTexture[i].GetRTV());
				}
			}
			return v;
		}
		D3D12_CPU_DESCRIPTOR_HANDLE RenderTarget::GetDSV()
		{
			if (m_DepthTexture.IsValid())
			{
				return m_DepthTexture.GetDSV();
			}
			return { 0 };
		}
	}
}