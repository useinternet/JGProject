#include "pch.h"
#include "GraphicsAPI.h"
#include "Application.h"
#include "Platform/Graphics/DirectX12/DirectX12API.h"
#include "Renderer.h"
#include "Mesh.h"
#include "Material.h"
namespace JG
{
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


