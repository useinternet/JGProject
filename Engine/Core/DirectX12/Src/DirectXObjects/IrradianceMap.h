#pragma once
#include"CubeMap.h"
#include"DirectXToolKit/PSO.h"
#include"DirectXToolKit/Viewport.h"
#include"DirectXToolKit/ScissorRect.h"
namespace Dx12
{
	class Dx12Object;
	class IrradianceMap : public CubeMap
	{
	private:
		RenderTarget m_RenderTarget;
		Viewport     m_Viewport;
		ScissorRect  m_ScissorRect;
		std::unique_ptr<Dx12Object> m_Object;
	public:
		IrradianceMap(
			CommandList* commandList, 
			uint32_t width, uint32_t height,
			float farZ, float NearZ);
		void Draw(CommandList* commandList, const Texture& CubeTexture);
		virtual const Texture& GetTexture() const override;
	protected:
		virtual void Build() override;
	};
}