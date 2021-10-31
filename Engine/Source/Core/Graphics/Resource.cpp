#include "pch.h"
#include "Resource.h"
#include "stb/stb_image.h"
#include "Application.h"
#include "JGGraphics.h"
#include "Platform/Graphics/DirectX12/DirectX12Resource.h"
#include "Class/Asset/Asset.h"

namespace JG
{
	SharedPtr<IVertexBuffer> IVertexBuffer::Create(String name, EBufferLoadMethod method)
	{
		auto api = JGGraphics::GetInstance().GetGraphicsAPI();
		JGASSERT_IF(api != nullptr, "GraphicsApi is nullptr");

		return api->CreateVertexBuffer(name, method);
	}

	SharedPtr<IIndexBuffer> IIndexBuffer::Create(String name, EBufferLoadMethod method)
	{
		auto api = JGGraphics::GetInstance().GetGraphicsAPI();
		JGASSERT_IF(api != nullptr, "GraphicsApi is nullptr");

		return api->CreateIndexBuffer(name, method);
	}
	SharedPtr<IComputer> IComputer::Create(const String& name, SharedPtr<IShader> shader)
	{
		auto api = JGGraphics::GetInstance().GetGraphicsAPI();
		JGASSERT_IF(api != nullptr, "GraphicsApi is nullptr");

		return api->CreateComputer(name, shader);
	}
	SharedPtr<IComputeBuffer> IComputeBuffer::Create(const String& name, u64 btSize)
	{
		auto api = JGGraphics::GetInstance().GetGraphicsAPI();
		JGASSERT_IF(api != nullptr, "GraphicsApi is nullptr");
		return api->CreateComputeBuffer(name, btSize);
	}
	SharedPtr<ITexture> ITexture::Create(const String& name)
	{
		auto api = JGGraphics::GetInstance().GetGraphicsAPI();
		JGASSERT_IF(api != nullptr, "GraphicsApi is nullptr");
		return api->CreateTexture(name);
	}
	SharedPtr<ITexture> ITexture::Create(const String& name, const TextureInfo& info)
	{
		auto api = JGGraphics::GetInstance().GetGraphicsAPI();
		JGASSERT_IF(api != nullptr, "GraphicsApi is nullptr");


		return api->CreateTexture(name, info);
	}

	SharedPtr<ITexture> ITexture::Create(const TextureAssetStock& stock)
	{
		auto api = JGGraphics::GetInstance().GetGraphicsAPI();
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
		bool isLoadWhiteTexture = false;
		auto nullTexturePath = CombinePath(Application::GetTexturePath(), "NullTexture.jgasset");
		if (fs::exists(nullTexturePath) == true)
		{
			auto json = CreateSharedPtr<Json>();
			if (AssetDataBase::GetInstance().ReadAsset(nullTexturePath, nullptr, &json) == true)
			{
				auto val = json->GetMember(JG_ASSET_KEY);
				if (val)
				{
					TextureAssetStock stock;
					stock.LoadJson(val);

					gNullTexture = ITexture::Create(nullTexturePath);
					gNullTexture->SetTextureMemory((const byte*)stock.Pixels.data(), stock.Width, stock.Height, stock.Channels);
				}
				else
				{
					isLoadWhiteTexture = true;
				}

			}
			else
			{
				isLoadWhiteTexture = true;
			}


		}
		else
		{
			isLoadWhiteTexture = true;
		}


		if(isLoadWhiteTexture)
		{
			TextureInfo textureInfo;
			textureInfo.Width = 1; textureInfo.Height = 1; 	textureInfo.MipLevel = 1; 	textureInfo.ArraySize = 1;
			textureInfo.ClearColor = Color::White();
			textureInfo.Format = ETextureFormat::R8G8B8A8_Unorm; textureInfo.Flags = ETextureFlags::Allow_RenderTarget;
			gNullTexture = ITexture::Create("WhiteTexture", textureInfo);

			auto api = JGGraphics::GetInstance().GetGraphicsAPI();
			JGASSERT_IF(api != nullptr, "GraphicsApi is nullptr");
			api->ClearRenderTarget(MAIN_GRAPHICS_COMMAND_ID, { gNullTexture }, nullptr);
		}
	}

	void ITexture::DestroyNullTexture()
	{
		gNullTexture.reset();
		gNullTexture = nullptr;
	}





}
