#pragma once
#include"CubeMap.h"


namespace Dx12
{
	class Dx12Object;
	class PrefilterMap : CubeMap
	{
		static const int ms_MaxMipLevels = 5;
	private:
		RenderTarget m_RenderTarget;
		Viewport     m_Viewport;
		ScissorRect  m_ScissorRect;
		std::unique_ptr<Dx12Object> m_Object;
	public:
		PrefilterMap(
			CommandList* commandList,
			uint32_t width, uint32_t height,
			float farZ, float NearZ);
		void Draw(CommandList* commandList, const Texture& CubeTexture);
		virtual const Texture& GetTexture() const override;
	protected:
		virtual void Build() override;
	};
}