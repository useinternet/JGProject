#include "pch.h"
#include "TextureManager.h"
#include "Resource.h"
#include "DirectXTex/DirectXTex.h"
#include "Temp/DDSTextureLoader.h"
#include "CommandList.h"
#include "RenderDevice.h"
#include "CommandQueue.h"
using namespace DirectX;
using namespace std;
namespace RE
{
	TextureManager* g_TextureManager = nullptr;
	TextureManager::TextureManager(const std::string& name) :
		DxObject(name)
	{
		if (g_TextureManager == nullptr)
		{
			g_TextureManager = this;
		}
	
	}
	void TextureManager::InitLoad(const std::string& path)
	{
		auto directQueue = GetRenderDevice()->GetDirectCmdQueue();
		auto cmdList = directQueue->GetCommandList();
		auto directory = fs::directory_iterator(path);

		for (auto& iter : directory)
		{
			if (fs::is_directory(iter.status()))
			{
				InitLoad(iter.path().string());
			}
			else
			{
				Texture t(iter.path().filename().string());
				if (TextureLoad(t, iter.path(), cmdList))
				{
					m_TextureCahce[t.GetName()] = make_shared<Texture>(t);
				}
				else
				{
					RE_LOG_ERROR("Failed Load Texture : {0}", iter.path().string());
				}
			}

			
		}

		directQueue->ExcuteCommandList({ cmdList });
		directQueue->Flush();
	}

	const Texture& TextureManager::GetTexture(const std::string& name)
	{
		static Texture* pT = nullptr;
		auto& cahce = g_TextureManager->m_TextureCahce;
		auto renderDevice = g_TextureManager->GetRenderDevice();
		if (cahce.find(name) == cahce.end())
		{
			cahce[name] = make_shared<Texture>(name);
			pT = cahce[name].get();
			renderDevice->SubmitToCopy(0, [&](CommandList* cmdList) {
	
				if (!g_TextureManager->TextureLoad(*pT, pT->GetName(), cmdList))
				{
					RE_LOG_ERROR("Failed Load Texture : {0}", pT->GetName());
				}
			});
		}
		return *cahce[name];
	}
	bool TextureManager::TextureLoad(Texture& t, const fs::path& p, CommandList* cmdList)
	{
		HRESULT hr = S_OK;


		TexMetadata metadata;
		ScratchImage scratchImage;

		if (p.extension() == ".dds")
		{
			hr = LoadFromDDSFile(p.wstring().c_str(),
				DDS_FLAGS_NONE,
				&metadata,
				scratchImage);
			
		}
		else if (p.extension() == ".hdf")
		{
			hr = LoadFromHDRFile(p.wstring().c_str(),
				&metadata,
				scratchImage);
		}
		else if (p.extension() == ".tga")
		{
			hr = LoadFromTGAFile(p.wstring().c_str(),
				&metadata,
				scratchImage);
		}
		else
		{
			CoInitialize(nullptr);
			hr = LoadFromWICFile(p.wstring().c_str(),
				DDS_FLAGS_NONE,
				&metadata,
				scratchImage);
		}
		if (FAILED(hr))
		{
			return false;
		}
		D3D12_RESOURCE_DESC desc = {};

		switch (metadata.dimension)
		{
		case TEX_DIMENSION_TEXTURE1D:
			desc = CD3DX12_RESOURCE_DESC::Tex1D(
				metadata.format,
				static_cast<UINT64>(metadata.width),
				static_cast<UINT16>(metadata.arraySize));
			break;
		case TEX_DIMENSION_TEXTURE2D:
			desc = CD3DX12_RESOURCE_DESC::Tex2D(
				metadata.format,
				static_cast<UINT64>(metadata.width),
				static_cast<UINT>(metadata.height),
				static_cast<UINT16>(metadata.arraySize));
			break;
		case TEX_DIMENSION_TEXTURE3D:
			desc = CD3DX12_RESOURCE_DESC::Tex3D(
				metadata.format,
				static_cast<UINT64>(metadata.width),
				static_cast<UINT>(metadata.height),
				static_cast<UINT16>(metadata.depth));
			break;
		default:
			return false;
		}
		desc.MipLevels = scratchImage.GetImageCount() / metadata.arraySize;
		t.CreateResource(desc);

		std::vector<D3D12_SUBRESOURCE_DATA> subresources(scratchImage.GetImageCount());

		const Image* pImages = scratchImage.GetImages();
		for (int i = 0; i < scratchImage.GetImageCount(); ++i)
		{
			auto& subresource = subresources[i];
			subresource.RowPitch = pImages[i].rowPitch;
			subresource.SlicePitch = pImages[i].slicePitch;
			subresource.pData = pImages[i].pixels;
		}
	
		cmdList->CopyTextureSubresource(
			t,
			0,
			static_cast<uint32_t>(subresources.size()),
			subresources.data());

		if (!t.IsVaild())
		{
			return false;
		}
		return true;
	}
}

