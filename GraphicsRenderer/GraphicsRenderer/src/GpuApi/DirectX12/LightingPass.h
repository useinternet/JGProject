#pragma once
#include"GraphicsPass.h"
namespace GR
{
	namespace Dx12
	{
		class GraphcisCommander;
		enum class ELightingPassSlot
		{
			Albedo,
			Normal,
			Ambient,
			MaterialProperty,
			A0,
			Depth,
			Num_Slot
		};
		class LightingPass : public GraphicsPass
		{

			static std::wstring m_ShaderVSPath;
			static std::wstring m_ShaderPSPath;
			static void SetShaderVSPath(const std::wstring& path);
			static void SetShaderPSPath(const std::wstring& path);
		private:
			RenderTarget  m_RenderTarget;
			Viewport      m_Viewport;
			ScissorRect   m_ScissorRect;
			RootSignature m_RootSig;
			GraphicsPSO   m_PSO;

			Texture m_Textures[(int)ELightingPassSlot::Num_Slot];
			Texture m_SkyTexture;
		public:
			virtual void Init(uint32_t width, uint32_t height) override;
			void BindResource(ELightingPassSlot slot, const Texture& texture);
			void BindSkyTexture(Texture& texture);
			virtual void Resize(uint32_t width, uint32_t height) override;
			virtual void Execute(GraphicsCommander* commander, const PassCB& passcb) override;
			const ColorTexture* GetTexture() const;
			ColorTexture* GetTexture();
		};
	}
}