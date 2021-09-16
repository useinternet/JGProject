#include "pch.h"
#include "Resource.h"
#include "Application.h"


#include "Platform/Graphics/DirectX12/DirectX12Resource.h"
#include "Class/Asset/Asset.h"

namespace JG
{
	SharedPtr<IVertexBuffer> IVertexBuffer::Create(String name, EBufferLoadMethod method)
	{
		auto api = Application::GetInstance().GetGraphicsAPI();
		JGASSERT_IF(api != nullptr, "GraphicsApi is nullptr");

		return api->CreateVertexBuffer(name, method);
	}

	SharedPtr<IIndexBuffer> IIndexBuffer::Create(String name, EBufferLoadMethod method)
	{
		auto api = Application::GetInstance().GetGraphicsAPI();
		JGASSERT_IF(api != nullptr, "GraphicsApi is nullptr");

		return api->CreateIndexBuffer(name, method);
	}
	SharedPtr<IComputer> IComputer::Create(const String& name, SharedPtr<IShader> shader)
	{
		auto api = Application::GetInstance().GetGraphicsAPI();
		JGASSERT_IF(api != nullptr, "GraphicsApi is nullptr");

		return api->CreateComputer(name, shader);
	}
	SharedPtr<IComputeBuffer> IComputeBuffer::Create(const String& name, u64 btSize)
	{
		auto api = Application::GetInstance().GetGraphicsAPI();
		JGASSERT_IF(api != nullptr, "GraphicsApi is nullptr");
		return api->CreateComputeBuffer(name, btSize);
	}
	SharedPtr<ITexture> ITexture::Create(const String& name)
	{
		auto api = Application::GetInstance().GetGraphicsAPI();
		JGASSERT_IF(api != nullptr, "GraphicsApi is nullptr");
		return api->CreateTexture(name);
	}
	SharedPtr<ITexture> ITexture::Create(const String& name, const TextureInfo& info)
	{
		auto api = Application::GetInstance().GetGraphicsAPI();
		JGASSERT_IF(api != nullptr, "GraphicsApi is nullptr");


		return api->CreateTexture(name, info);
	}

	SharedPtr<ITexture> ITexture::Create(const TextureAssetStock& stock)
	{
		auto api = Application::GetInstance().GetGraphicsAPI();
		JGASSERT_IF(api != nullptr, "GraphicsApi is nullptr");

		return api->CreateTexture(stock);
	}
	static SharedPtr<ITexture> gNullTexture = nullptr;
	SharedPtr<ITexture> ITexture::NullTexture()
	{
		return gNullTexture;
	}


	void ITexture::CreateNullTexture()
	{
		TextureInfo textureInfo;
		textureInfo.Width = 1; textureInfo.Height = 1; 	textureInfo.MipLevel = 1; 	textureInfo.ArraySize = 1;
		textureInfo.ClearColor = Color::White();
		textureInfo.Format = ETextureFormat::R8G8B8A8_Unorm; textureInfo.Flags = ETextureFlags::Allow_RenderTarget;
		gNullTexture = ITexture::Create("WhiteTexture", textureInfo);

		auto api = Application::GetInstance().GetGraphicsAPI();
		JGASSERT_IF(api != nullptr, "GraphicsApi is nullptr");
		api->ClearRenderTarget(MAIN_GRAPHICS_COMMAND_ID, { gNullTexture }, nullptr);
	}

	void ITexture::DestroyNullTexture()
	{
		gNullTexture.reset();
		gNullTexture = nullptr;
	}





}
