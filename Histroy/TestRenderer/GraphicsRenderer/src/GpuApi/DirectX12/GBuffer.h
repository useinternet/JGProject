#pragma once



#include"GraphicsPass.h"


namespace GR
{
	namespace Dx12
	{
		class RenderObject;
		class GraphicsDevice;
		class GraphicsCommander;
		enum class EGBufferSlot
		{
			Albedo,
			Normal,
			Ambient,
			MaterialProperty,
			Depth,
			DepthStencil,
			Num_Slot
		};
		class GBuffer : public GraphicsPass
		{
		private:
			static DXGI_FORMAT ms_GBufferFormats[(int)EGBufferSlot::Num_Slot];
		private:
			uint32_t m_Width;
			uint32_t m_Height;
			RenderTarget  m_Rendertarget;
			RootSignature m_RootSignature;
			Viewport      m_Viewport;
			ScissorRect   m_ScissorRect;
			std::queue<RenderObject*> m_ObjQueue;
		public:
			GBuffer()  = default;
			~GBuffer() = default;
		public:
			virtual void Init(uint32_t width, uint32_t height) override;
			virtual void Resize(uint32_t width, uint32_t height) override;
			void PushObject(RenderObject* obj);
			virtual void Execute(GraphicsCommander* commander, const PassCB& passcb) override;
			Texture* GetTexture(EGBufferSlot slot);
			DepthTexture* GetDepthStencilTexture();
		public:
			static GraphicsPSO GetPSO();
		};
	}
}