#pragma once
#include "GraphicsDevice.h"
#include "Texture.h"

namespace DX12
{
	
	REGISTER_GRAPHICS_INTERFACE_GRAPHICSDEVICE(DX12_GraphicsDevice)
		
	class DX12_GraphicsDevice : public GE::GraphicsDevice
	{

	public:
		virtual void Create(void* config_user_data) override;
		virtual void BeginFrame() override;
		virtual void EndFrame() override;
		virtual void Destroy() override;


		virtual bool LoadTextures(const std::vector<std::wstring>& paths, std::vector<GE::Texture*>& tArray) override;
	private:
	};
}

