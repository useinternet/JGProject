#include "pch.h"
#include "GraphicsAPI.h"
#include "Application.h"
#include "Platform/Graphics/DirectX12/DirectX12API.h"
#include "Renderer.h"

namespace JG
{
	void IGraphicsAPI::ClearTexture(SharedPtr<ITexture> texture)
	{
		ClearRenderTarget({ texture }, nullptr);
	}
	UniquePtr<IGraphicsAPI> IGraphicsAPI::Create(EGraphicsAPI api)
	{
		switch(api)
		{
		case EGraphicsAPI::DirectX12:
			return CreateUniquePtr<DirectX12API>();
		default:
			JGASSERT("not support GraphicsAPI");
			return nullptr;
		}
		
	}
}


