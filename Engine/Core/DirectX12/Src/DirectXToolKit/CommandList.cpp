#include<PCH.h>
#include"CommandList.h"
#include<DirectXTex/DirectXTex.h>
#include<Util/StringUtil.h>
#include"DirectXCommon/DDSTextureLoader.h"
#include<wincodec.h>
#include"DxUtil.h"
#include"RootSigature.h"
#include"UploadBuffer.h"
#include"PSO.h"
#include"DirectXCore.h"
#include"CommandQueue.h"
#include"VertexBuffer.h"
#include"IndexBuffer.h"
#include"Viewport.h"
#include"ScissorRect.h"
#include"Texture.h"
#include"DescriptorHeap.h"
#include"RenderTarget.h"
#include"ResourceStateMap.h"
#include"GenerateMipMapsCS.h"
using namespace Dx12;
using namespace std;
using namespace DirectX;

std::unordered_map<std::string, ComPtr<ID3D12Resource>> CommandList::ms_GlobalTextures;
std::mutex CommandList::ms_GlobalTexturesMutex;

CommandList::CommandList(D3D12_COMMAND_LIST_TYPE type)
{
	m_CmdListType = type;
	m_UploadBuffer = make_unique<UploadBuffer>();


	m_D3D_CommandAllocator = DxUtil::CreateCommandAllocator(type);
	m_D3D_CommandList = DxUtil::CreateCommandList(type, m_D3D_CommandAllocator);

	m_DescriptorHeap = make_unique<DescriptorHeap>(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
		m_NumDescriptor, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);


	m_DescriptorOffset = 0;
	SetDescriptorHeap(*m_DescriptorHeap);
}


void CommandList::CopyBuffer(
	Buffer& buffer,
	uint32_t numElements,
	uint32_t elementSize,
	const void* bufferData,
	D3D12_RESOURCE_FLAGS flags)
{
	auto device = DxDevice::GetDevice();
	uint32_t bufferSize = numElements * elementSize;



	if (bufferSize == 0)
		return;

	Resource resource(CD3DX12_RESOURCE_DESC::Buffer(bufferSize, flags));

	if (bufferData != nullptr)
	{
		Resource uploadResource(
			CD3DX12_RESOURCE_DESC::Buffer(bufferSize),
			nullptr, "UploadBuffer",
			D3D12_RESOURCE_STATE_GENERIC_READ,
			D3D12_HEAP_TYPE_UPLOAD);

		D3D12_SUBRESOURCE_DATA subResourceData = {};
		subResourceData.pData = bufferData;
		subResourceData.RowPitch = bufferSize;
		subResourceData.SlicePitch = subResourceData.RowPitch;

		TransitionBarrier(resource, D3D12_RESOURCE_STATE_COPY_DEST);
		
		UpdateSubresources(m_D3D_CommandList.Get(), resource.Get(),
			uploadResource.Get(), 0, 0, 1, &subResourceData);


		TrackResource(uploadResource);
		// 변환
	}
	TrackResource(resource);
	buffer.Set(resource);
	buffer.CreateViews(numElements, elementSize);
}
void CommandList::CopyResource(const Resource& destResource, const Resource& srcResource)
{
	TransitionBarrier(destResource, D3D12_RESOURCE_STATE_COPY_DEST);
	TransitionBarrier(srcResource, D3D12_RESOURCE_STATE_COPY_SOURCE);


	m_D3D_CommandList->CopyResource(destResource.Get(), srcResource.Get());


	TrackResource(destResource);
	TrackResource(srcResource);
}
void CommandList::ClearRenderTarget(const RenderTarget& rendertarget)
{
	for (int i = 0; i < RtvSlot::DepthStencil; ++i)
	{
		if (rendertarget.GetTexture(i).IsValid())
		{
			TransitionBarrier(rendertarget.GetTexture(i), D3D12_RESOURCE_STATE_RENDER_TARGET);
		
			auto dxf4_clearColor = rendertarget.GetRenderTargetClearColor(i);
			float clearColor[4];
			clearColor[0] = dxf4_clearColor.x;
			clearColor[1] = dxf4_clearColor.y;
			clearColor[2] = dxf4_clearColor.z;
			clearColor[3] = dxf4_clearColor.w;
			
			m_D3D_CommandList->ClearRenderTargetView(
				rendertarget.GetRenderTargetView(i), clearColor, 0, nullptr);
		}
	}

	if (rendertarget.GetTexture(RtvSlot::DepthStencil).IsValid())
	{
		TransitionBarrier(rendertarget.GetTexture(RtvSlot::DepthStencil), D3D12_RESOURCE_STATE_DEPTH_WRITE);

		m_D3D_CommandList->ClearDepthStencilView(
			rendertarget.GetDepthStencilView(),
			rendertarget.GetDepthStencilClearFlags(),
			rendertarget.GetDepthStencilClearDepth(),
			rendertarget.GetDepthStencilClearStencil(),
			0,
			nullptr);
	}

}

void CommandList::TransitionBarrier(const Resource& resource, D3D12_RESOURCE_STATES afterstate)
{
	if (!resource.IsValid())
	{
		DX12_LOG_ERROR("%s is not exsit D3DResource", resource.GetName().c_str());
		return;
	}
	auto currState = ResourceStateMap::GetResourceState(resource.Get());

	if (currState == afterstate)
		return;

	ResourceStateMap::TransitionResourceState(resource.Get(), afterstate);
	m_D3D_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
		resource.Get(), currState, afterstate));
}

void CommandList::UAVBarrier(const Resource& resource)
{
	m_D3D_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::UAV(resource.Get()));
}
void CommandList::AliasingBarrier(const Resource& beforeResource, const Resource& afterResource)
{
	m_D3D_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Aliasing(
		beforeResource.Get(), afterResource.Get()));
}

void CommandList::GenerateMipMaps(const Texture& texture, bool isCubeMap)
{
	auto resourceDesc = texture.GetDesc();

	if (resourceDesc.MipLevels == 1)
		return;

	if  (resourceDesc.Dimension != D3D12_RESOURCE_DIMENSION_TEXTURE2D ||
		(resourceDesc.DepthOrArraySize != 1 && !isCubeMap) ||
		(resourceDesc.DepthOrArraySize == 1 && isCubeMap))
	{
		DX12_LOG_ERROR("Generate Mips only supports 2D Textures.");
		return;
	}
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));

	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = resourceDesc.Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
	srvDesc.Texture2DArray.ArraySize = 1;
	srvDesc.Texture2DArray.MipLevels = resourceDesc.MipLevels;
	srvDesc.Texture2DArray.MostDetailedMip = 0;
	srvDesc.Texture2DArray.ResourceMinLODClamp = 0.0f;
	srvDesc.Texture2DArray.PlaneSlice = 0;
	
	if (Texture::IsUAVCompatibleFormat(resourceDesc.Format))
	{
		if (isCubeMap)
		{
			for (int i = 0; i < 6; ++i)
			{
				srvDesc.Texture2DArray.FirstArraySlice = i;
				GenerateMipMaps_UAV(texture, &srvDesc);
			}
		}
		else
		{
			GenerateMipMaps_UAV(texture, nullptr);
		}
	
	}
	else if (Texture::IsBGRFormat(resourceDesc.Format))
	{
		if (isCubeMap)
		{
			for (int i = 0; i < 6; ++i)
			{
				srvDesc.Texture2DArray.FirstArraySlice = i;
				GenerateMipMaps_BGR(texture, &srvDesc);
			}
		}
		else
		{
			GenerateMipMaps_BGR(texture, nullptr);
		}
	}
	else if (Texture::IsSRGBFormat(resourceDesc.Format))
	{
		if (isCubeMap)
		{
			for (int i = 0; i < 6; ++i)
			{
				srvDesc.Texture2DArray.FirstArraySlice = i;
				GenerateMipMaps_SRGB(texture, &srvDesc);
			}
		}
		else
		{
			GenerateMipMaps_SRGB(texture, nullptr);
		}
	}
	else
	{
		DX12_LOG_ERROR("Unsupported texture format for mipmap generation");
	}
	
}
void CommandList::SetGraphicsConstants(uint32_t rootparam, uint32_t num32BitValues,const void* srcData)
{
	m_D3D_CommandList->SetGraphicsRoot32BitConstants(rootparam, num32BitValues, srcData, 0);
}

void CommandList::SetGraphicsConstantBufferView(uint32_t rootparam, uint32_t sizeInbyte,const void* data)
{
	auto alloc = m_UploadBuffer->Allocate(sizeInbyte,D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);
	memcpy(alloc.CPU , data, sizeInbyte);
	m_D3D_CommandList->SetGraphicsRootConstantBufferView(rootparam, alloc.GPU);
}
void CommandList::SetGraphicsConstantBufferView(uint32_t rootparam, const Resource& resource)
{
	m_D3D_CommandList->SetGraphicsRootConstantBufferView(rootparam, resource.Get()->GetGPUVirtualAddress());
}
void CommandList::SetGraphicsDescriptorTable(uint32_t rootparam, D3D12_CPU_DESCRIPTOR_HANDLE handle)
{
	if (m_DescriptorOffset >= m_NumDescriptor)
	{
		DX12_LOG_ERROR("SetGraphicsDescriptorTable Ignore because DescriptorOffset > NumDescriptor");
		return;
	}
	auto device = DxDevice::GetDevice();
	device->CopyDescriptorsSimple(1,
		m_DescriptorHeap->GetCPUHeapHandle(m_DescriptorOffset),
		handle,
		D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	m_D3D_CommandList->SetGraphicsRootDescriptorTable(rootparam, m_DescriptorHeap->GetGPUHeapHandle(m_DescriptorOffset));
	m_DescriptorOffset++;
}
void CommandList::SetGraphicsDescriptorTables(uint32_t rootparam, uint32_t numHandles, const D3D12_CPU_DESCRIPTOR_HANDLE * handle)
{
	if (m_DescriptorOffset >= m_NumDescriptor)
	{
		DX12_LOG_ERROR("SetGraphicsDescriptorTable Ignore because DescriptorOffset > NumDescriptor");
		return;
	}

	auto device = DxDevice::GetDevice();


	std::vector<uint32_t> v(numHandles, 1);
	auto HeapHandle = m_DescriptorHeap->GetCPUHeapHandle(m_DescriptorOffset);
	device->CopyDescriptors(
		1,
		&HeapHandle,
		&numHandles,
		numHandles,
		handle, 
		v.data(),
		D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	m_D3D_CommandList->SetGraphicsRootDescriptorTable(rootparam, m_DescriptorHeap->GetGPUHeapHandle(m_DescriptorOffset));
	m_DescriptorOffset += numHandles;
}
void CommandList::SetGraphicsShaderResourceView(uint32_t rootparam, uint32_t numElement, uint32_t elementSize,const void* data)
{
	uint32_t bufferSize = numElement * elementSize;
	auto alloc = m_UploadBuffer->Allocate(bufferSize, elementSize);
	memcpy(alloc.CPU, data, bufferSize);
	m_D3D_CommandList->SetGraphicsRootShaderResourceView(rootparam, alloc.GPU);
}
void CommandList::SetGraphicsShaderResourceView(uint32_t rootparam, const Resource& resource)
{
	m_D3D_CommandList->SetGraphicsRootShaderResourceView(rootparam, resource.Get()->GetGPUVirtualAddress());
}
void CommandList::SetGraphicsUnorderedAccessView(uint32_t rootparam, const Resource& resource)
{
	m_D3D_CommandList->SetComputeRootUnorderedAccessView(rootparam, resource.Get()->GetGPUVirtualAddress());
}
void CommandList::SetComputeConstants(uint32_t rootparam, uint32_t num32BitValues,const void* srcData)
{
	m_D3D_CommandList->SetComputeRoot32BitConstants(rootparam, num32BitValues, srcData, 0);
}
void CommandList::SetComputeConstantBufferView(uint32_t rootparam, uint32_t sizeInbyte, const void* data)
{
	auto alloc = m_UploadBuffer->Allocate(sizeInbyte, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);
	memcpy(alloc.CPU, data, sizeInbyte);
	m_D3D_CommandList->SetComputeRootConstantBufferView(rootparam, alloc.GPU);
}
void CommandList::SetComputeConstantBufferView(uint32_t rootparam, const Resource& resource)
{
	m_D3D_CommandList->SetComputeRootConstantBufferView(rootparam, resource.Get()->GetGPUVirtualAddress());
}
void CommandList::SetComputeDescriptorTable(uint32_t rootparam, D3D12_CPU_DESCRIPTOR_HANDLE handle)
{
	if (m_DescriptorOffset >= m_NumDescriptor)
	{
		DX12_LOG_ERROR("SetGraphicsDescriptorTable Ignore because DescriptorOffset > NumDescriptor");
		return;
	}
	auto device = DxDevice::GetDevice();
	device->CopyDescriptorsSimple(1,
		m_DescriptorHeap->GetCPUHeapHandle(m_DescriptorOffset),
		handle,
		D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	m_D3D_CommandList->SetComputeRootDescriptorTable(rootparam, m_DescriptorHeap->GetGPUHeapHandle(m_DescriptorOffset));
	m_DescriptorOffset++;
}
void CommandList::SetComputeDescriptorTables(uint32_t rootparam, uint32_t numHandles, const D3D12_CPU_DESCRIPTOR_HANDLE* handle)
{
	if (m_DescriptorOffset >= m_NumDescriptor)
	{
		DX12_LOG_ERROR("SetGraphicsDescriptorTable Ignore because DescriptorOffset > NumDescriptor");
		return;
	}

	auto device = DxDevice::GetDevice();


	std::vector<uint32_t> v(numHandles, 1);
	auto HeapHandle = m_DescriptorHeap->GetCPUHeapHandle(m_DescriptorOffset);
	device->CopyDescriptors(
		1,
		&HeapHandle,
		&numHandles,
		numHandles,
		handle,
		v.data(),
		D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	m_D3D_CommandList->SetComputeRootDescriptorTable(rootparam, m_DescriptorHeap->GetGPUHeapHandle(m_DescriptorOffset));
	m_DescriptorOffset += numHandles;

}
void CommandList::SetComputeShaderResourceView(uint32_t rootparam, uint32_t numElement, uint32_t elementSize,const void* data)
{
	uint32_t bufferSize = numElement * elementSize;
	auto alloc = m_UploadBuffer->Allocate(bufferSize, elementSize);
	memcpy(alloc.CPU, data, bufferSize);

	m_D3D_CommandList->SetComputeRootShaderResourceView(rootparam, alloc.GPU);
}
void CommandList::SetComputeShaderResourceView(uint32_t rootparam, const Resource& resource)
{
	m_D3D_CommandList->SetComputeRootShaderResourceView(rootparam, resource.Get()->GetGPUVirtualAddress());
}
void CommandList::SetComputeUnorderedAccessView(uint32_t rootparam, const Resource& resource)
{
	m_D3D_CommandList->SetComputeRootUnorderedAccessView(rootparam, resource.Get()->GetGPUVirtualAddress());
}
void CommandList::SetViewport(const Viewport& viewport)
{
	m_D3D_CommandList->RSSetViewports(1, &viewport.Get());
}
void CommandList::SetScissorRect(const ScissorRect& rect)
{
	m_D3D_CommandList->RSSetScissorRects(1, &rect.Get());
}
void CommandList::SetPipelineState(const PSO& pso)
{
	m_D3D_CommandList->SetPipelineState(pso.Get());
}
void CommandList::SetRenderTarget(const RenderTarget& rtv)
{
	vector<D3D12_CPU_DESCRIPTOR_HANDLE> rtvHandles;
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = { 0 };


	for (uint32_t i = 0; i < RtvSlot::NumRtvSlot; ++i)
	{
		if (rtv.GetTexture(i).IsValid())
		{
			if (i == RtvSlot::DepthStencil)
			{
				TransitionBarrier(rtv.GetTexture(i), D3D12_RESOURCE_STATE_DEPTH_WRITE);
	
				dsvHandle = rtv.GetDepthStencilView();
			}
			else
			{
				TransitionBarrier(rtv.GetTexture(i), D3D12_RESOURCE_STATE_RENDER_TARGET);
				rtvHandles.push_back(rtv.GetRenderTargetView(i));
			}
		}
	}
	
	D3D12_CPU_DESCRIPTOR_HANDLE* pdsvHandle = nullptr;
	if (dsvHandle.ptr != 0)
		pdsvHandle = &dsvHandle;


	m_D3D_CommandList->OMSetRenderTargets((UINT)rtvHandles.size(), rtvHandles.data(),
		false, pdsvHandle);
}
void CommandList::SetGraphicsRootSignature(const RootSignature& rootSig)
{
	m_D3D_CommandList->SetGraphicsRootSignature(rootSig.Get());
}
void CommandList::SetComputeRootSignature(const RootSignature& rootSig)
{
	m_D3D_CommandList->SetComputeRootSignature(rootSig.Get());
}

void CommandList::SetVertexBuffer(const VertexBuffer& buffer)
{
	m_D3D_CommandList->IASetVertexBuffers(0, 1, &buffer.GetVertexBufferView());

}
void CommandList::SetIndexBuffer(const IndexBuffer& buffer)
{
	m_D3D_CommandList->IASetIndexBuffer(&buffer.GetIndexBufferView());
}
void CommandList::SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY type)
{
	m_D3D_CommandList->IASetPrimitiveTopology(type);
}

void CommandList::LoadTextureFromFile(const std::string& path, Texture& texture, ETextureUsage usage)
{
	fs::path ph(path);
	
	if (!fs::exists(ph))
	{
		DX12_LOG_ERROR("%s is not exsit.", path.c_str());
		return;
	}

	auto file_format = ph.extension();

	lock_guard<mutex> lock(ms_GlobalTexturesMutex);
	auto iter = ms_GlobalTextures.find(path);
	if (iter != ms_GlobalTextures.end())
	{
		auto d3dresource = iter->second;
		texture.SetD3DResource(d3dresource, D3D12_RESOURCE_STATE_COMMON);
		texture.SetName(path);
		texture.SetUsage(usage);

		TrackResource(d3dresource);
		return;
	}

	ComPtr<ID3D12Resource> d3dresource;
	ComPtr<ID3D12Resource> uploadresource;

	TexMetadata metadata;
	ScratchImage scratchImage;
	HRESULT hr = S_OK;

	if (file_format == ".dds")
	{
		hr = LoadFromDDSFile(ph.wstring().c_str(), DDS_FLAGS_NONE, &metadata, scratchImage);
	}
	else if (file_format == ".hdr")
	{
		hr = LoadFromHDRFile(ph.wstring().c_str(), &metadata, scratchImage);
	}
	else if (file_format == ".tga")
	{
		hr = LoadFromTGAFile(ph.wstring().c_str(), &metadata, scratchImage);
	}
	else
	{
		hr = LoadFromWICFile(ph.wstring().c_str(), WIC_FLAGS_NONE, &metadata, scratchImage);
	}
	if (FAILED(hr))
	{
		DX12_LOG_ERROR("failed load texture : %s ", path.c_str());
		return;
	}


	if (usage == TextureUsage::Albedo)
	{
		metadata.format = MakeSRGB(metadata.format);
	}


	D3D12_RESOURCE_DESC textureDesc = {};
	switch (metadata.dimension)
	{
	case TEX_DIMENSION_TEXTURE1D:
		textureDesc = CD3DX12_RESOURCE_DESC::Tex1D(metadata.format, static_cast<UINT64>(metadata.width), static_cast<UINT16>(metadata.arraySize));
		break;
	case TEX_DIMENSION_TEXTURE2D:
		textureDesc = CD3DX12_RESOURCE_DESC::Tex2D(metadata.format, static_cast<UINT64>(metadata.width), static_cast<UINT>(metadata.height), static_cast<UINT16>(metadata.arraySize));
		break;
	case TEX_DIMENSION_TEXTURE3D:
		textureDesc = CD3DX12_RESOURCE_DESC::Tex3D(metadata.format, static_cast<UINT64>(metadata.width), static_cast<UINT>(metadata.height), static_cast<UINT16>(metadata.depth));
		break;
	default:
		throw std::exception("Invalid texture dimension.");
		break;
	}
	ThrowIfFailed(DxDevice::GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&textureDesc,
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		IID_PPV_ARGS(&d3dresource)));

	texture.SetD3DResource(d3dresource, D3D12_RESOURCE_STATE_COMMON);
	texture.SetUsage(usage);
	texture.SetName(path);


	std::vector<D3D12_SUBRESOURCE_DATA> subresources(scratchImage.GetImageCount());


	const Image* pImages = scratchImage.GetImages();
	for (int i = 0; i < scratchImage.GetImageCount(); ++i)
	{
		auto& subresource = subresources[i];
		subresource.RowPitch = pImages[i].rowPitch;
		subresource.SlicePitch = pImages[i].slicePitch;
		subresource.pData = pImages[i].pixels;
	}


	if (texture.IsValid())
	{
		TransitionBarrier(texture, D3D12_RESOURCE_STATE_COPY_DEST);

		uint64_t requiredSize = GetRequiredIntermediateSize(texture.Get(),
			0, (uint32_t)subresources.size());

		ThrowIfFailed(DxDevice::GetDevice()->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(requiredSize),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&uploadresource)));

		UpdateSubresources(m_D3D_CommandList.Get(), texture.Get(), uploadresource.Get(),
			0, 0, (uint32_t)subresources.size(), subresources.data());
	}
	TrackResource(d3dresource);
	TrackResource(uploadresource);

	ms_GlobalTextures[path] = ComPtr<ID3D12Resource>(texture.Get());

}
void CommandList::Close()
{
	m_D3D_CommandList->Close();
}


void CommandList::Reset()
{
	ThrowIfFailed(m_D3D_CommandAllocator->Reset());
	ThrowIfFailed(m_D3D_CommandList->Reset(m_D3D_CommandAllocator.Get(), nullptr));

	m_UploadBuffer->Reset();
	m_TrackObjects.clear();

	m_DescriptorOffset = 0;
	SetDescriptorHeap(*m_DescriptorHeap);
}
void CommandList::FlushTrackObject()
{
	m_TrackObjects.clear();
}
void CommandList::Draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t startVertexLocation, uint32_t startInstanceLocation)
{
	m_D3D_CommandList->DrawInstanced(vertexCount, instanceCount, startVertexLocation, startInstanceLocation);
}
void CommandList::DrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t startIndexLocation,
	int baseVertexLocatoin, uint32_t startInstanceLocation)
{
	m_D3D_CommandList->DrawIndexedInstanced(indexCount, instanceCount, startIndexLocation, baseVertexLocatoin, startInstanceLocation);
}
void CommandList::Dispatch(uint32_t thread_group_x, uint32_t thread_group_y, uint32_t thread_group_z)
{
	m_D3D_CommandList->Dispatch(thread_group_x, thread_group_y, thread_group_z);
}
void CommandList::SetDescriptorHeap(const DescriptorHeap& heap)
{
	m_D3D_CommandList->SetDescriptorHeaps(1, heap.GetComPtr().GetAddressOf());
}
void CommandList::TrackResource(const Resource& resource)
{
	m_TrackObjects.push_back(resource.Get());
}
void CommandList::TrackResource(const ComPtr<ID3D12Object>& object)
{
	m_TrackObjects.push_back(object);
}

void CommandList::GenerateMipMaps_UAV(const Texture& texture, D3D12_SHADER_RESOURCE_VIEW_DESC* srvDesc)
{
	if (!m_GenerateMipMapsCS)
	{
		m_GenerateMipMapsCS = make_unique<GenerateMipMapsCS>();
	}

	auto device = DxDevice::GetDevice();
	auto resourceDesc = texture.GetDesc();


	Texture stagingTexture;

	if (!(resourceDesc.Flags & D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS))
	{
		auto stagingDesc = resourceDesc;
		stagingDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

		stagingTexture = Texture(
			texture.GetUsage(),
			stagingDesc,
			nullptr,
			"StagingTexture",
			D3D12_RESOURCE_STATE_COPY_DEST);

		CopyResource(stagingTexture, texture);
	}
	else
	{
		stagingTexture.SetD3DResource(texture.Get(), texture.GetState());
	}


	SetComputeRootSignature(m_GenerateMipMapsCS->GetRootSig());
	SetPipelineState(m_GenerateMipMapsCS->GetPSO());


	GenerateMipsCB mipCB;
	for (uint32_t srcMip = 0;  srcMip < resourceDesc.MipLevels - 1u;)
	{
		uint64_t srcWidth = resourceDesc.Width >> srcMip;
		uint32_t srcHeight = resourceDesc.Height >> srcMip;
		uint32_t dstWidth = static_cast<uint32_t>(srcWidth >> 1);
		uint32_t dstHeight = srcHeight >> 1;

		mipCB.SrcDimension = (srcHeight & 1) << 1 | (srcWidth & 1);

		DWORD mipCount;

		_BitScanForward(&mipCount, (dstWidth == 1 ? dstHeight : dstWidth) |
			(dstHeight == 1 ? dstWidth : dstHeight));

		mipCount = std::min<DWORD>(4, mipCount + 1);
		mipCount = (srcMip + mipCount) > resourceDesc.MipLevels ?
			resourceDesc.MipLevels - srcMip : mipCount;

		dstWidth = std::max<DWORD>(1, dstWidth);
		dstHeight = std::max<DWORD>(1, dstHeight);

		mipCB.SrcMipLevel = srcMip;
		mipCB.NumMipLevels = mipCount;
		mipCB.TexelSize.x = 1.0f / (float)dstWidth;
		mipCB.TexelSize.y = 1.0f / (float)dstHeight;

		SetComputeConstants(GenerateMips::GenerateMipsCB, mipCB);
		SetComputeDescriptorTable(GenerateMips::SrcMip, stagingTexture.GetShaderResourceView(srvDesc));

		std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> outputHandles(4);
		for (uint32_t mip = 0; mip < mipCount; ++mip)
		{
			D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
			uavDesc.Format = resourceDesc.Format;
			uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
			uavDesc.Texture2D.MipSlice = srcMip + mip + 1;
			outputHandles[mip] = stagingTexture.GetUnorderedAccessView(&uavDesc);
		}
		for (uint32_t mip = mipCount; mip < 4; ++mip)
		{
			outputHandles[mip] = m_GenerateMipMapsCS->GetUAV(mip);
		}

		SetComputeDescriptorTables(GenerateMips::OutMip, 4, outputHandles.data());


		Dispatch(Math::DivideByMultiple(dstWidth, 8), Math::DivideByMultiple(dstHeight, 8));

		UAVBarrier(stagingTexture);

		srcMip += mipCount;
	}
	if (stagingTexture.Get() != texture.Get())
	{
		CopyResource(texture, stagingTexture);
	}
	
}
void CommandList::GenerateMipMaps_BGR(const Texture& texture, D3D12_SHADER_RESOURCE_VIEW_DESC* srvDesc)
{
	auto device = DxDevice::GetDevice();

	auto resourceDesc = texture.GetDesc();
	auto copyDesc = resourceDesc;

	copyDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	copyDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

	auto allocationInfo = device->GetResourceAllocationInfo(0, 1, &resourceDesc);
	auto bufferSize = GetRequiredIntermediateSize(texture.Get(), 0, resourceDesc.MipLevels);

	D3D12_HEAP_DESC heapDesc = { };
	heapDesc.SizeInBytes = allocationInfo.SizeInBytes;
	heapDesc.Flags = D3D12_HEAP_FLAG_ALLOW_ONLY_NON_RT_DS_TEXTURES;
	heapDesc.Properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapDesc.Properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapDesc.Properties.Type = D3D12_HEAP_TYPE_DEFAULT;

	ComPtr<ID3D12Heap> heap;
	ThrowIfFailed(device->CreateHeap(&heapDesc, IID_PPV_ARGS(&heap)));


	ComPtr<ID3D12Resource> resourceCopy;
	ThrowIfFailed(device->CreatePlacedResource(
		heap.Get(),
		0,
		&copyDesc,
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		IID_PPV_ARGS(&resourceCopy)));

	Texture copyTexture;
	copyTexture.SetD3DResource(resourceCopy, D3D12_RESOURCE_STATE_COMMON);
	

	auto aliasDesc = resourceDesc;
	aliasDesc.Format = (resourceDesc.Format == DXGI_FORMAT_B8G8R8X8_UNORM ||
		resourceDesc.Format == DXGI_FORMAT_B8G8R8X8_UNORM_SRGB) ?
		DXGI_FORMAT_B8G8R8X8_UNORM : DXGI_FORMAT_B8G8R8A8_UNORM;

	ComPtr<ID3D12Resource> aliasCopy;
	ThrowIfFailed(device->CreatePlacedResource(
		heap.Get(),
		0,
		&aliasDesc,
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		IID_PPV_ARGS(&aliasCopy)
	));


	Texture aliasTexture;
	aliasTexture.SetD3DResource(aliasCopy, D3D12_RESOURCE_STATE_COMMON);

	CopyResource(aliasTexture, texture);
	AliasingBarrier(aliasTexture, copyTexture);


	// 추가한 부분
	if (srvDesc)
		srvDesc->Format = copyDesc.Format;
	//




	GenerateMipMaps_UAV(copyTexture, srvDesc);

	AliasingBarrier(copyTexture, aliasTexture);
	CopyResource(texture, aliasTexture);

	TrackResource(heap);
	TrackResource(copyTexture);
	TrackResource(aliasTexture);
	TrackResource(texture);
}
void CommandList::GenerateMipMaps_SRGB(const Texture& texture, D3D12_SHADER_RESOURCE_VIEW_DESC* srvDesc)
{
	auto device = DxDevice::GetDevice();
	auto resourceDesc = texture.GetDesc();

	auto copyDesc = resourceDesc;
	copyDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	copyDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;


	auto allocationInfo = device->GetResourceAllocationInfo(0, 1, &resourceDesc);
	auto bufferSize = GetRequiredIntermediateSize(texture.Get(), 0, resourceDesc.MipLevels);

	D3D12_HEAP_DESC heapDesc = {};
	heapDesc.SizeInBytes = allocationInfo.SizeInBytes;
	heapDesc.Flags = D3D12_HEAP_FLAG_ALLOW_ONLY_NON_RT_DS_TEXTURES;
	heapDesc.Properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapDesc.Properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapDesc.Properties.Type = D3D12_HEAP_TYPE_DEFAULT;


	ComPtr<ID3D12Heap> heap;
	ThrowIfFailed(device->CreateHeap(&heapDesc, IID_PPV_ARGS(&heap)));

	ComPtr<ID3D12Resource> resourceCopy;
	ThrowIfFailed(device->CreatePlacedResource(
		heap.Get(),
		0,
		&copyDesc,
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		IID_PPV_ARGS(&resourceCopy)
	));


	Texture copyTexture;
	copyTexture.SetD3DResource(resourceCopy, D3D12_RESOURCE_STATE_COMMON);


	auto aliasDesc = resourceDesc;

	ComPtr<ID3D12Resource> aliasCopy;
	ThrowIfFailed(device->CreatePlacedResource(
		heap.Get(),
		0,
		&aliasDesc,
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		IID_PPV_ARGS(&aliasCopy)
	));

	Texture aliasTexture;
	aliasTexture.SetD3DResource(aliasCopy, D3D12_RESOURCE_STATE_COMMON);
	AliasingBarrier(Texture(), aliasTexture);

	CopyResource(aliasTexture, texture);

	AliasingBarrier(aliasTexture, copyTexture);


	// 추가한 부분
	if (srvDesc)
		srvDesc->Format = copyDesc.Format;
	//

	GenerateMipMaps_UAV(copyTexture, srvDesc);

	AliasingBarrier(copyTexture, aliasTexture);
	CopyResource(texture, aliasTexture);

	TrackResource(heap);
	TrackResource(copyTexture);
	TrackResource(aliasTexture);
	TrackResource(texture);
}