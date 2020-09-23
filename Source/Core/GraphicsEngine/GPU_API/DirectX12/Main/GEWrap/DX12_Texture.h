#pragma once
#include "Texture.h"
#include "DirectXApi.h"

namespace DX12
{
	REGISTER_GRAPHICS_INTERFACE_TEXTURE(DX12_Texture)
	class DX12_Texture : public GE::Texture
	{
	public:

		virtual void* GetUserTextureData() const override;
		virtual const GE::TextureInfo& GetInfo() const override;
		virtual bool  IsValid() const override;
		virtual ~DX12_Texture() override {}
	private:
		DX12::Texture   m_Texture;
		mutable GE::TextureInfo m_Info;
	};

}