#include "pch.h"
#include "Resource.h"
#include "stb/stb_image.h"
#include "Application.h"
#include "JGGraphics.h"
#include "Platform/Graphics/DirectX12/DirectX12Resource.h"
#include "Class/Asset/Asset.h"


namespace JG
{
	SharedPtr<IVertexBuffer> IVertexBuffer::Create(const String& name, EBufferLoadMethod method)
	{
		auto api = JGGraphics::GetInstance().GetGraphicsAPI();
		JGASSERT_IF(api != nullptr, "GraphicsApi is nullptr");

		return api->CreateVertexBuffer(name, method);
	}

	SharedPtr<IIndexBuffer> IIndexBuffer::Create(const String& name, EBufferLoadMethod method)
	{
		auto api = JGGraphics::GetInstance().GetGraphicsAPI();
		JGASSERT_IF(api != nullptr, "GraphicsApi is nullptr");

		return api->CreateIndexBuffer(name, method);
	}
	SharedPtr<IByteAddressBuffer> IByteAddressBuffer::Create(const String& name, u64 elementCount)
	{
		auto api = JGGraphics::GetInstance().GetGraphicsAPI();
		JGASSERT_IF(api != nullptr, "GraphicsApi is nullptr");

		return api->CreateByteAddressBuffer(name, elementCount);
	}

	SharedPtr<IStructuredBuffer> IStructuredBuffer::Create(const String& name, u64 elementSize, u64 elementCount)
	{
		auto api = JGGraphics::GetInstance().GetGraphicsAPI();
		JGASSERT_IF(api != nullptr, "GraphicsApi is nullptr");

		return api->CreateStrucuredBuffer(name, elementSize, elementCount);
	}
	SharedPtr<IReadBackBuffer> IReadBackBuffer::Create(const String& name)
	{
		auto api = JGGraphics::GetInstance().GetGraphicsAPI();
		JGASSERT_IF(api != nullptr, "GraphicsApi is nullptr");

		return api->CreateReadBackBuffer(name);
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



	static SharedPtr<ITexture> gNullTexture = nullptr;
	SharedPtr<ITexture> ITexture::NullTexture()
	{
		return gNullTexture;
	}


	void ITexture::CreateNullTexture()
	{
		bool isLoadWhiteTexture = false;
		auto nullTexturePath = PathHelper::CombinePath(Application::GetTexturePath(), "NullTexture.jgasset");
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
					List<jbyte> uncom;
					u64 originSize = stock.OriginPixelSize;
					uncom.resize(originSize);

					
					if (uncompress((Bytef*)uncom.data(), (uLongf*)(&originSize), (const Bytef*)stock.Pixels.data(), stock.Pixels.size()) == Z_OK)
					{
						stock.Pixels = uncom;
						stock.Pixels.resize(originSize);
					}

					gNullTexture = ITexture::Create(nullTexturePath);
					gNullTexture->SetTextureMemory((const jbyte*)stock.Pixels.data(), stock.Width, stock.Height, stock.Channels, stock.PixelPerUnit);
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
			api->GetGraphicsContext()->ClearRenderTarget({ gNullTexture }, nullptr);
		}
	}

	void ITexture::DestroyNullTexture()
	{
		gNullTexture.reset();
		gNullTexture = nullptr;
	}












}
