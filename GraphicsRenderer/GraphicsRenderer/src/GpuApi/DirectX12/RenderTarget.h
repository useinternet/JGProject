#pragma once



#include"Dx12Include.h"
#include"GraphicsResource.h"

namespace GR
{
	namespace Dx12
	{
		class RenderTarget
		{
		public:
			enum ESlot
			{
				Slot0 = 0,
				Slot1,
				Slot2,
				Slot3,
				Slot4,
				Slot5,
				Slot6,
				Slot7,
				NumRenderTarget,
				DepthStencil
			};

		private:
			ColorTexture m_ColorTexture[NumRenderTarget];
			DepthTexture m_DepthTexture;


			uint32_t m_NumRtvs;
		public:
			RenderTarget() : m_NumRtvs(0) {}
			void AttachTexture(ESlot slot, const ColorTexture& texture);
			void AttachTexture(ESlot slot, const DepthTexture& texture);

			ColorTexture* GetTexture(ESlot slot);
			DepthTexture* GetDepthTexture();

			uint32_t GetNumRTVs() const;
		    std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> GetRTVs();
			D3D12_CPU_DESCRIPTOR_HANDLE GetDSV();
		};


	}
}