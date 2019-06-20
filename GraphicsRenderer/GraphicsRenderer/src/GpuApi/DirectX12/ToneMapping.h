#pragma once
#include"GraphicsPass.h"

namespace GR
{
	namespace Dx12
	{
		class ToneMapping : public GraphicsPass
		{
		private:
			static std::wstring m_ShaderVSPath;
			static std::wstring m_ShaderPSPath;
			static void SetShaderVSPath(const std::wstring& path);
			static void SetShaderPSPath(const std::wstring& path);
		private:
			RenderTarget m_RenderTarget;
			Viewport      m_Viewport;
			ScissorRect   m_ScissorRect;
			RootSignature m_RootSig;
			GraphicsPSO   m_PSO;

			Texture       m_Texture;
		public:
			virtual void Init(uint32_t width, uint32_t height) override;
			void BindTexture(const Texture& texture);
			virtual void Resize(uint32_t width, uint32_t height) override;
			virtual void Execute(GraphicsCommander* commander, const PassCB& passcb) override;
			const ColorTexture* GetTexture() const;
			ColorTexture* GetTexture();
		};


	}
}