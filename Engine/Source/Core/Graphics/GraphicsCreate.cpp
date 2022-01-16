#include "pch.h"
#include "JGGraphics.h"
#include "GraphicsAPI.h"
#include "RootSignature.h"
#include "FrameBuffer.h"
#include "Material.h"
#include "Resource.h"
#include "Shader.h"


#include "stb/stb_image.h"
#include "Graphics/RayTracing/RayTracingPipeline.h"
#include "Graphics/RayTracing/BottomLevelAccelerationStructure.h"
#include "Graphics/RayTracing/TopLevelAccelerationStructure.h"

#include "Class/Asset/Asset.h"
#include "Class/Asset/AssetImporter.h"

#include "Application.h"
namespace JG
{
	// ********************   FrameBuffer.h   ******************** //
	SharedPtr<IFrameBuffer> IFrameBuffer::Create(const FrameBufferInfo& info)
	{
		JGASSERT_IF(info.Handle != 0, TT("Data is Null"));
		auto api = JGGraphics::GetInstance().GetGraphicsAPI();
		JGASSERT_IF(api != nullptr, "GraphicsApi is nullptr");

		return api->CreateFrameBuffer(info);
	}


	// ********************   Resource.h   ******************** //
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


		if (isLoadWhiteTexture)
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









	// ********************   Material.h   ******************** //
	SharedPtr<IMaterial> IMaterial::Create(const String& name)
	{
		auto api = JGGraphics::GetInstance().GetGraphicsAPI();
		JGASSERT_IF(api != nullptr, "GraphicsApi is nullptr");
		return api->CreateMaterial(name);
	}
	SharedPtr<IMaterial> IMaterial::Create(const String& name, SharedPtr<IGraphicsShader> shader)
	{
		auto api = JGGraphics::GetInstance().GetGraphicsAPI();
		JGASSERT_IF(api != nullptr, "GraphicsApi is nullptr");
		return api->CreateMaterial(name, shader);
	}







	// ********************   Mesh.h   ******************** //
	SharedPtr<ISubMesh> ISubMesh::Create(const String& name)
	{
		auto api = JGGraphics::GetInstance().GetGraphicsAPI();
		JGASSERT_IF(api != nullptr, "GraphicsApi is nullptr");


		return api->CreateSubMesh(name);
	}
	SharedPtr<IMesh> IMesh::Create(const String& name)
	{
		auto api = JGGraphics::GetInstance().GetGraphicsAPI();
		JGASSERT_IF(api != nullptr, "GraphicsApi is nullptr");


		return api->CreateMesh(name);
	}
	void IMesh::SetMeshStock(const StaticMeshAssetStock& stock)
	{
		auto cnt = stock.SubMeshNames.size();
		SetInputLayout(JGVertex::GetInputLayout());
		for (u64 i = 0; i < cnt; ++i)
		{
			auto vBuffer = IVertexBuffer::Create(stock.SubMeshNames[i] + "_VBuffer", EBufferLoadMethod::CPULoad);
			auto iBuffer = IIndexBuffer::Create(stock.SubMeshNames[i] + "_IBuffer", EBufferLoadMethod::CPULoad);
			auto subMesh = ISubMesh::Create(stock.SubMeshNames[i]);

			vBuffer->SetData(stock.Vertices[i].data(), sizeof(JGVertex), stock.Vertices[i].size());
			iBuffer->SetData(stock.Indices[i].data(), stock.Indices[i].size());
			subMesh->SetVertexBuffer(vBuffer);
			subMesh->SetIndexBuffer(iBuffer);
			AddMesh(subMesh);
		}
		SetBoundingBox(stock.BoundingBox);
	}
	SharedPtr<IMesh> IMesh::Create(const StaticMeshAssetStock& stock)
	{
		SharedPtr<IMesh> mesh = IMesh::Create(stock.Name);
		mesh->SetMeshStock(stock);
		return mesh;
	}







	// ********************   RayTracing   ******************** //
	SharedPtr<IRootSignatureCreater> IRootSignatureCreater::Create()
	{
		auto api = JGGraphics::GetInstance().GetGraphicsAPI();
		return api->CreateRootSignatureCreater();
	}

	SharedPtr<IRayTracingPipeline> IRayTracingPipeline::Create()
	{
		auto api = JGGraphics::GetInstance().GetGraphicsAPI();
		return api->CreateRayTracingPipeline();
	}
	SharedPtr<IBottomLevelAccelerationStructure> IBottomLevelAccelerationStructure::Create()
	{
		auto api = JGGraphics::GetInstance().GetGraphicsAPI();
		return api->CreateBottomLevelAccelerationStructure();
	}

	SharedPtr<ITopLevelAccelerationStructure> ITopLevelAccelerationStructure::Create()
	{
		auto api = JGGraphics::GetInstance().GetGraphicsAPI();
		return api->CreateTopLevelAccelerationStructure();
	}
}