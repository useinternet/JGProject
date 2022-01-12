#include "pch.h"

#include "DirectX12Resource.h"
#include "DirectX12API.h"
#include "DirectX12Shader.h"
#include "Utill/ResourceStateTracker.h"
#include "Utill/CommandQueue.h"
#include "Utill/DescriptorAllocator.h"
#include "Utill/CommandList.h"
#include "Utill/PipelineState.h"
#include "Class/Asset/Asset.h"
namespace JG
{
	DirectX12VertexBuffer::~DirectX12VertexBuffer()
	{
		Reset();
	}


	bool DirectX12VertexBuffer::SetData(const void* datas, u64 elementSize, u64 elementCount, u64 commandID)
	{
		u64 originBtSize = mElementSize * mElementCount;
		mElementSize = elementSize; mElementCount = elementCount;
		u64 btSize = mElementSize * mElementCount;

		// Create
		switch (mLoadMethod)
		{
		case EBufferLoadMethod::GPULoad:
		{
			Reset();
			mD3DResource = DirectX12API::CreateCommittedResource(
				GetName(),
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
				D3D12_HEAP_FLAG_NONE,
				&CD3DX12_RESOURCE_DESC::Buffer(btSize),
				D3D12_RESOURCE_STATE_COMMON,
				nullptr);

			if (mD3DResource)
			{
				auto commandList = DirectX12API::GetGraphicsCommandList();
				commandList->CopyBuffer(mD3DResource.Get(), datas, elementSize, elementCount);
			}
		}
		break;
		case EBufferLoadMethod::CPULoad:
			if (mD3DResource && (originBtSize != btSize || mCPUData == nullptr))
			{
				Reset();
			}
			if (mD3DResource == nullptr)
			{
				mD3DResource = DirectX12API::CreateCommittedResource(
					GetName(),
					&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
					D3D12_HEAP_FLAG_NONE,
					&CD3DX12_RESOURCE_DESC::Buffer(btSize),
					D3D12_RESOURCE_STATE_GENERIC_READ,
					nullptr
				);
				if (mD3DResource)
				{
					mD3DResource->Map(0, nullptr, &mCPUData);
				}
			}
			if (mCPUData != nullptr)
			{
				memcpy(mCPUData, datas, btSize);
			}
			else
			{
				JG_CORE_WARN("{0} Buffer CPU Data is nullptr", GetName());

			}
	
			break;
		}
		return true;
	}

	bool DirectX12VertexBuffer::IsValid() const
	{
		return mD3DResource != nullptr;
	}
	EBufferLoadMethod DirectX12VertexBuffer::GetBufferLoadMethod() const
	{
		return mLoadMethod;
	}
	void DirectX12VertexBuffer::SetBufferLoadMethod(EBufferLoadMethod method)
	{
		mLoadMethod = method;
	}
	void DirectX12VertexBuffer::Bind(u64 commandID)
	{
		if (IsValid() == false)
		{
			return;
		}
		auto commandList = DirectX12API::GetGraphicsCommandList();


		D3D12_VERTEX_BUFFER_VIEW View = {};
		View.BufferLocation = mD3DResource->GetGPUVirtualAddress();
		View.SizeInBytes = (u32)mElementSize * (u32)mElementCount;
		View.StrideInBytes = (u32)mElementSize;
		commandList->BindVertexBuffer(View, false);
	}

	void DirectX12VertexBuffer::Reset()
	{
		if (mD3DResource == nullptr)
		{
			return;
		}
		if (mLoadMethod == EBufferLoadMethod::CPULoad)
		{
			mD3DResource->Unmap(0, nullptr);
			mCPUData = nullptr;
		}
		DirectX12API::DestroyCommittedResource(mD3DResource);
		mD3DResource.Reset(); mD3DResource = nullptr;
	}




	DirectX12IndexBuffer::~DirectX12IndexBuffer()
	{
		Reset();
	}

	bool DirectX12IndexBuffer::SetData(const u32* datas, u64 count, u64 commandID)
	{
		u64 originBtSize = sizeof(u32) * mIndexCount;
		mIndexCount = count;
		u64 btSize = sizeof(u32) * mIndexCount;

		// Create
		switch (mLoadMethod)
		{
		case EBufferLoadMethod::GPULoad:
		{
			Reset();
			mD3DResource = DirectX12API::CreateCommittedResource(
				GetName(),
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
				D3D12_HEAP_FLAG_NONE,
				&CD3DX12_RESOURCE_DESC::Buffer(btSize),
				D3D12_RESOURCE_STATE_COMMON,
				nullptr
			);
			if (mD3DResource != nullptr)
			{
				auto commandList = DirectX12API::GetGraphicsCommandList();
				commandList->CopyBuffer(mD3DResource.Get(), datas, sizeof(u32), mIndexCount);
			}
		}
		break;
		case EBufferLoadMethod::CPULoad:
			if (mD3DResource && (originBtSize != btSize || mCPUData == nullptr))
			{
				Reset();
			}
			if (mD3DResource == nullptr)
			{
				mD3DResource = DirectX12API::CreateCommittedResource(
					GetName(),
					&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
					D3D12_HEAP_FLAG_NONE,
					&CD3DX12_RESOURCE_DESC::Buffer(btSize),
					D3D12_RESOURCE_STATE_GENERIC_READ,
					nullptr
				);

				if (mD3DResource)
				{
					auto hResult = mD3DResource->Map(0, nullptr, (void**)&mCPUData);
					if (FAILED(hResult))
					{
						JG_CORE_WARN("{0} Buffer Fail Map", GetName());
					}
				}
			}
			if (mCPUData != nullptr)
			{
				memcpy(mCPUData, datas, btSize);
			}
			else
			{
				JG_CORE_WARN("{0} Buffer CPU Data is nullptr", GetName());
			}
			break;
		}
		return true;
	}

	bool DirectX12IndexBuffer::IsValid() const
	{
		return mD3DResource != nullptr;
	}
	EBufferLoadMethod DirectX12IndexBuffer::GetBufferLoadMethod() const
	{
		return mLoadMethod;
	}
	void DirectX12IndexBuffer::SetBufferLoadMethod(EBufferLoadMethod method)
	{
		mLoadMethod = method;
	}

	void DirectX12IndexBuffer::Bind(u64 commandID)
	{
		if (IsValid() == false)
		{
			return;
		}
		auto commandList = DirectX12API::GetGraphicsCommandList();
		D3D12_INDEX_BUFFER_VIEW View;
		View.BufferLocation = mD3DResource->GetGPUVirtualAddress();
		View.Format = DXGI_FORMAT_R32_UINT;
		View.SizeInBytes = sizeof(u32) * mIndexCount;

		commandList->BindIndexBuffer(View);
	}

	void DirectX12IndexBuffer::Reset()
	{
		if (mD3DResource == nullptr)
		{
			return;
		}
		if (mLoadMethod == EBufferLoadMethod::CPULoad)
		{
			mD3DResource->Unmap(0, nullptr);
			mCPUData = nullptr;
		}
		DirectX12API::DestroyCommittedResource(mD3DResource);
		mD3DResource.Reset(); mD3DResource = nullptr;
	}

	bool DirectX12ByteAddressBuffer::IsValid() const
	{
		return mD3DResource != nullptr;
	}

	bool DirectX12ByteAddressBuffer::SetData(u64 elementCount, const void* initDatas, u64 commandID)
	{
		u64 originBtSize = mElementSize * mElementCount;
		mElementCount    = elementCount;
		u64 btSize       = mElementSize * mElementCount;


		if (mD3DResource && (originBtSize != btSize))
		{
			Reset();
		}

		if (mD3DResource == nullptr)
		{
			mD3DResource = DirectX12API::CreateCommittedResource(
				GetName(),
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
				D3D12_HEAP_FLAG_NONE,
				&CD3DX12_RESOURCE_DESC::Buffer(btSize, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS),
				D3D12_RESOURCE_STATE_COMMON,
				nullptr);
			CreateViews();
		}
		if (mD3DResource && initDatas)
		{
			auto commandList = DirectX12API::GetGraphicsCommandList();
			commandList->CopyBuffer(mD3DResource.Get(), initDatas, mElementSize, elementCount);
		}
		return IsValid();
	}
	D3D12_CPU_DESCRIPTOR_HANDLE DirectX12ByteAddressBuffer::GetSRV() const
	{
		return mSRV->CPU();
	}

	D3D12_CPU_DESCRIPTOR_HANDLE DirectX12ByteAddressBuffer::GetUAV() const
	{
		return mUAV->CPU();
	}

	ID3D12Resource* DirectX12ByteAddressBuffer::Get() const
	{
		return mD3DResource.Get();
	}

	void DirectX12ByteAddressBuffer::CreateViews()
	{
		mUAV = CreateUniquePtr<DescriptorAllocation>();
		mSRV = CreateUniquePtr<DescriptorAllocation>();

		u64 bufferSize = mElementCount * mElementSize;


		D3D12_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
		SRVDesc.ViewDimension			= D3D12_SRV_DIMENSION_BUFFER;
		SRVDesc.Format					= DXGI_FORMAT_R32_TYPELESS;
		SRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		SRVDesc.Buffer.NumElements		= (UINT)bufferSize / 4;
		SRVDesc.Buffer.Flags			= D3D12_BUFFER_SRV_FLAG_RAW;

		*mSRV = std::move(DirectX12API::CSUAllocate());
		DirectX12API::GetD3DDevice()->CreateShaderResourceView(Get(), &SRVDesc, mSRV->CPU());



		D3D12_UNORDERED_ACCESS_VIEW_DESC UAVDesc = {};
		UAVDesc.ViewDimension		= D3D12_UAV_DIMENSION_BUFFER;
		UAVDesc.Format				= DXGI_FORMAT_R32_TYPELESS;
		UAVDesc.Buffer.NumElements	= (UINT)bufferSize / 4;
		UAVDesc.Buffer.Flags		= D3D12_BUFFER_UAV_FLAG_RAW;

		*mUAV = std::move(DirectX12API::CSUAllocate());
		DirectX12API::GetD3DDevice()->CreateUnorderedAccessView(Get(), nullptr, &UAVDesc, mUAV->CPU());
	}

	void DirectX12ByteAddressBuffer::Reset()
	{
		if (mD3DResource == nullptr)
		{
			return;
		}
		DirectX12API::DestroyCommittedResource(mD3DResource);
		mD3DResource.Reset(); mD3DResource = nullptr;
	}


	bool DirectX12StructuredBuffer::IsValid() const
	{
		return mD3DResource != nullptr;
	}
	bool DirectX12StructuredBuffer::SetData(u64 elementSize, u64 elementCount, void* initDatas , u64 commandID)
	{
		u64 originBtSize = mElementSize * mElementCount;
		mElementSize     = elementSize;
		mElementCount    = elementCount;
		u64 btSize		 = mElementSize * mElementCount;


		if (mD3DResource && (originBtSize != btSize))
		{
			Reset();
		}

		if (mD3DResource == nullptr)
		{
			mD3DResource = DirectX12API::CreateCommittedResource(
				GetName(),
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
				D3D12_HEAP_FLAG_NONE,
				&CD3DX12_RESOURCE_DESC::Buffer(btSize, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS),
				D3D12_RESOURCE_STATE_COMMON,
				nullptr);
		}

		if (mD3DResource && initDatas)
		{
			auto commandList = DirectX12API::GetGraphicsCommandList();
			commandList->CopyBuffer(mD3DResource.Get(), initDatas, elementSize, elementCount);
		}
		return IsValid();
	}
	u64 DirectX12StructuredBuffer::GetDataSize() const
	{
		return mElementCount * mElementSize;
	}
	u64 DirectX12StructuredBuffer::GetElementCount() const
	{
		return mElementCount;
	}
	u64 DirectX12StructuredBuffer::GetElementSize() const
	{
		return mElementSize;
	}
	BufferID DirectX12StructuredBuffer::GetBufferID() const
	{
		if (IsValid() == true)
		{
			return mD3DResource->GetGPUVirtualAddress();
		}
		return 0;
	}

	ID3D12Resource* DirectX12StructuredBuffer::Get() const
	{
		return mD3DResource.Get();
	}

	void DirectX12StructuredBuffer::Reset()
	{
		if (mD3DResource == nullptr)
		{
			return;
		}
		DirectX12API::DestroyCommittedResource(mD3DResource);
		mD3DResource.Reset(); mD3DResource = nullptr;
	}



	DirectX12ReadBackBuffer::~DirectX12ReadBackBuffer()
	{
		Reset();
	}

	bool DirectX12ReadBackBuffer::IsValid() const
	{
		return mD3DResource != nullptr;
	}

	bool DirectX12ReadBackBuffer::Read(SharedPtr<IStructuredBuffer> readWriteBuffer, u64 commandID , bool asCompute)
	{
		if (readWriteBuffer == nullptr || readWriteBuffer->IsValid() == false)
		{
			return false;
		}
		auto originBtSize = mBufferSize;
		mBufferSize = readWriteBuffer->GetDataSize();
		if (originBtSize != mBufferSize)
		{
			Reset();
		}

		if (mD3DResource == nullptr)
		{
			mD3DResource = DirectX12API::CreateCommittedResource(GetName(),
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_READBACK),
				D3D12_HEAP_FLAG_NONE,
				&CD3DX12_RESOURCE_DESC::Buffer(mBufferSize),
				D3D12_RESOURCE_STATE_COPY_DEST,
				nullptr
			);
			if (mD3DResource != nullptr)
			{
				mD3DResource->Map(0, nullptr, &mCPU);
			}
		}

		auto dxRWBuffer = static_cast<DirectX12StructuredBuffer*>(readWriteBuffer.get());

		if (asCompute == true)
		{
			auto commandList = DirectX12API::GetComputeCommandList();
			commandList->CopyResource(Get(), dxRWBuffer->Get());
		}
		else
		{
			auto commandList = DirectX12API::GetGraphicsCommandList();
			commandList->CopyResource(Get(), dxRWBuffer->Get());
		}
		return IsValid();
	}

	bool DirectX12ReadBackBuffer::GetData(void* out_data,u64 out_data_size)
	{
		if (IsValid() == false)
		{
			return false;
		}
		out_data_size = Math::Min(out_data_size, mBufferSize);
		memcpy(out_data, mCPU, out_data_size);
		return true;
	}

	u64 DirectX12ReadBackBuffer::GetDataSize() const
	{
		return mBufferSize;
	}

	void DirectX12ReadBackBuffer::Reset()
	{
		if (mD3DResource)
		{
			mD3DResource->Unmap(0, nullptr);
			mCPU = nullptr;
			mBufferSize = 0;
			DirectX12API::DestroyCommittedResource(mD3DResource);
			mD3DResource.Reset();
			mD3DResource = nullptr;
		}
	}

	DirectX12Texture::~DirectX12Texture()
	{
		Reset();
	}

	void DirectX12Texture::SetName(const String& name)
	{
		ITexture::SetName(name);
		if (mD3DResource != nullptr)
		{
			mD3DResource->SetName(StringHelper::s2ws(name).c_str());
			ResourceStateTracker::SetResourceName(mD3DResource.Get(), name);
		}
	}

	TextureID DirectX12Texture::GetTextureID() const
	{
		return GetSRV().ptr;
	}

	const TextureInfo& DirectX12Texture::GetTextureInfo() const
	{
		return mTextureInfo;
	}
	void DirectX12Texture::SetTextureInfo(const TextureInfo& info)
	{
		Reset();

		mTextureInfo = info;
		auto flags = mTextureInfo.Flags;



		D3D12_RESOURCE_FLAGS d3dRscFlags = D3D12_RESOURCE_FLAG_NONE;

		if (flags & ETextureFlags::Allow_RenderTarget)
		{
			d3dRscFlags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
		}
		else if (flags & ETextureFlags::Allow_DepthStencil)
		{
			d3dRscFlags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
		}
		if (flags & ETextureFlags::Allow_UnorderedAccessView)
		{
			d3dRscFlags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
		}

		D3D12_RESOURCE_DESC rscDesc = CD3DX12_RESOURCE_DESC::Tex2D(
			ConvertDXGIFormat(mTextureInfo.Format), mTextureInfo.Width, mTextureInfo.Height,
			info.ArraySize, mTextureInfo.MipLevel, 1, 0,
			d3dRscFlags, D3D12_TEXTURE_LAYOUT_UNKNOWN, 0);
		

		SharedPtr<D3D12_CLEAR_VALUE> clearValue = nullptr;
		if (flags & ETextureFlags::Allow_RenderTarget)
		{
			clearValue = CreateSharedPtr< D3D12_CLEAR_VALUE>();
			clearValue->Color[0] = info.ClearColor.R;
			clearValue->Color[1] = info.ClearColor.G;
			clearValue->Color[2] = info.ClearColor.B;
			clearValue->Color[3] = info.ClearColor.A;
			clearValue->Format = ConvertDXGIFormat(info.Format);
		}
		else if (flags & ETextureFlags::Allow_DepthStencil)
		{
			clearValue = CreateSharedPtr< D3D12_CLEAR_VALUE>();
			clearValue->DepthStencil.Depth = info.ClearDepth;
			clearValue->DepthStencil.Stencil = info.ClearStencil;
			clearValue->Format = ConvertDXGIFormat(info.Format);
		}

		mD3DResource = DirectX12API::CreateCommittedResource(GetName(),
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE,
			&rscDesc,
			D3D12_RESOURCE_STATE_COMMON,
			clearValue.get());

		if (mD3DResource)
		{
			mD3DResource->SetName(StringHelper::s2ws(GetName()).c_str());
		}
	}
	void DirectX12Texture::SetTextureMemory(
		const byte* pixels, i32 width, i32 height, i32 channels, u32 pixelPerUnit, u32 arraySize, u32 mipLevel, ETextureFlags flags, ETextureFormat format)
	{
		TextureInfo info;
		info.ArraySize = arraySize;
		info.Flags = flags;
		info.MipLevel = mipLevel;
		info.Width = (u32)width;
		info.Height = (u32)height;
		info.Format = format;
		info.PixelPerUnit = pixelPerUnit;
		SetTextureInfo(info);
		auto commandList = DirectX12API::GetGraphicsCommandList();
		commandList->CopyTextrueFromMemory(Get(), pixels, width, height, channels, info.ArraySize);
	}

	void DirectX12Texture::SetClearColor(const Color& clearColor)
	{
		mTextureInfo.ClearColor = clearColor;
	}
	bool DirectX12Texture::IsValid() const
	{
		
		return mD3DResource != nullptr;
	}

	void DirectX12Texture::Create(const String& name, const TextureInfo& info)
	{
		SetName(name);
		SetTextureInfo(info);
	}

	void DirectX12Texture::CreateFromMemory(const String& name, const byte* pixels, i32 width, i32 height, i32 channels, u32 pixelPerUnit)
	{

		SetName(name);
		SetTextureMemory(pixels, width, height, channels, pixelPerUnit);
	}

	void DirectX12Texture::Reset()
	{
		if (mD3DResource == nullptr)
		{
			return;
		}
		mRTVs.clear();
		mDSVs.clear();
		mSRVs.clear();
		mUAVs.clear();
		DirectX12API::DestroyCommittedResource(mD3DResource);
		mD3DResource.Reset();
		mD3DResource = nullptr;
	}
	D3D12_CPU_DESCRIPTOR_HANDLE DirectX12Texture::GetRTV() const
	{
		if (IsValid() == false) return { 0 };
		// 렌더 타겟 텍스쳐가 아니라면 0을 뿜어냄
		if ((mTextureInfo.Flags & ETextureFlags::Allow_RenderTarget) == false)
		{
			JG_CORE_ERROR("{0} not supported RenderTarget because does not include  ETextureFlags::Allow_RenderTarget Flag", GetName());
			return { 0 };
		}

		SharedPtr<D3D12_RENDER_TARGET_VIEW_DESC> rtvDesc = CreateRTVDesc(mTextureInfo.Flags);


		u64 hash = 0;
		if (rtvDesc != nullptr)
		{
			hash = std::hash<D3D12_RENDER_TARGET_VIEW_DESC>{}(*rtvDesc);
		}

		D3D12_CPU_DESCRIPTOR_HANDLE handle = { 0 };
		bool isFind = false;
		{
			std::shared_lock<std::shared_mutex> lock(mRTVMutex);
			isFind = mRTVs.find(hash) != mRTVs.end();
			if (isFind == true)
			{
				handle = mRTVs[hash].CPU();
			}
		}
		if(isFind == false)
		{
			std::lock_guard<std::shared_mutex> lock(mRTVMutex);
			auto alloc = DirectX12API::RTVAllocate();
			DirectX12API::GetD3DDevice()->CreateRenderTargetView(Get(), rtvDesc.get(), alloc.CPU());
			mRTVs.emplace(hash, std::move(alloc));
			handle = mRTVs[hash].CPU();
		}

		return handle;
	}
	D3D12_CPU_DESCRIPTOR_HANDLE DirectX12Texture::GetDSV() const
	{
		if (IsValid() == false) return { 0 };
		if ((mTextureInfo.Flags & ETextureFlags::Allow_DepthStencil) == false)
		{
			JG_CORE_ERROR("{0} not supported DepthStencil because does not include  ETextureFlags::Allow_DepthStencil Flag", GetName());
			return { 0 };
		}

		SharedPtr<D3D12_DEPTH_STENCIL_VIEW_DESC> dsvDesc = CreateDSVDesc(mTextureInfo.Flags);

		u64 hash = 0;
		if (dsvDesc != nullptr)
		{
			hash = std::hash<D3D12_DEPTH_STENCIL_VIEW_DESC>{}(*dsvDesc);
		}


		D3D12_CPU_DESCRIPTOR_HANDLE handle = { 0 };
		bool isFind = false;
		{
			std::shared_lock<std::shared_mutex> lock(mDSVMutex);
			isFind = mDSVs.find(hash) != mDSVs.end();
			if (isFind == true)
			{
				handle = mDSVs[hash].CPU();
			}
		}
		if (isFind == false)
		{
			std::lock_guard<std::shared_mutex> lock(mDSVMutex);
			auto alloc = DirectX12API::DSVAllocate();
			DirectX12API::GetD3DDevice()->CreateDepthStencilView(Get(), dsvDesc.get(), alloc.CPU());
			mDSVs.emplace(hash, std::move(alloc));
			handle = mDSVs[hash].CPU();
		}

		return handle;
	}
	D3D12_CPU_DESCRIPTOR_HANDLE DirectX12Texture::GetSRV() const
	{
		if (IsValid() == false) return { 0 };

		D3D12_SHADER_RESOURCE_VIEW_DESC desc;


		SharedPtr<D3D12_SHADER_RESOURCE_VIEW_DESC> srvDesc = CreateSRVDesc(mTextureInfo.Flags);

		u64 hash = 0;
		if (srvDesc != nullptr)
		{
			hash = std::hash<D3D12_SHADER_RESOURCE_VIEW_DESC>{}(*srvDesc);
		}

		D3D12_CPU_DESCRIPTOR_HANDLE handle = { 0 };
		bool isFind = false;
		{
			std::shared_lock<std::shared_mutex> lock(mSRVMutex);
			isFind = mSRVs.find(hash) != mSRVs.end();
			if (isFind == true)
			{
				handle = mSRVs[hash].CPU();
			}
		}
		if (isFind == false)
		{
			std::lock_guard<std::shared_mutex> lock(mSRVMutex);
			auto alloc = DirectX12API::CSUAllocate();
			DirectX12API::GetD3DDevice()->CreateShaderResourceView(Get(), srvDesc.get(), alloc.CPU());
			mSRVs.emplace(hash, std::move(alloc));
			handle = mSRVs[hash].CPU();
		}

		return handle;
	}
	D3D12_CPU_DESCRIPTOR_HANDLE DirectX12Texture::GetUAV() const
	{
		if (IsValid() == false) return { 0 };

		SharedPtr<D3D12_UNORDERED_ACCESS_VIEW_DESC> uavDesc = CreateUAVDesc(mTextureInfo.Flags);
		u64 hash = 0;
		if (uavDesc != nullptr)
		{
			hash = std::hash<D3D12_UNORDERED_ACCESS_VIEW_DESC>{}(*uavDesc);
		}

		D3D12_CPU_DESCRIPTOR_HANDLE handle = { 0 };
		bool isFind = false;
		{
			std::shared_lock<std::shared_mutex> lock(mUAVMutex);
			isFind = mUAVs.find(hash) != mUAVs.end();
			if (isFind == true)
			{
				handle = mUAVs[hash].CPU();
			}
		}
		if (isFind == false)
		{
			std::lock_guard<std::shared_mutex> lock(mUAVMutex);
			auto alloc = DirectX12API::CSUAllocate();
			DirectX12API::GetD3DDevice()->CreateUnorderedAccessView(Get(), nullptr, uavDesc.get(), alloc.CPU());
			mUAVs.emplace(hash, std::move(alloc));
			handle = mUAVs[hash].CPU();
		}

		return handle;
	}

	SharedPtr<D3D12_RENDER_TARGET_VIEW_DESC> DirectX12Texture::CreateRTVDesc(ETextureFlags flag) const
	{
		return nullptr;
	}

	SharedPtr<D3D12_DEPTH_STENCIL_VIEW_DESC> DirectX12Texture::CreateDSVDesc(ETextureFlags flag) const
	{
		return nullptr;
	}

	SharedPtr<D3D12_SHADER_RESOURCE_VIEW_DESC> DirectX12Texture::CreateSRVDesc(ETextureFlags flag) const
	{
		SharedPtr<D3D12_SHADER_RESOURCE_VIEW_DESC> result;
		if (flag & ETextureFlags::SRV_TextureCube)
		{
			result = CreateSharedPtr<D3D12_SHADER_RESOURCE_VIEW_DESC>();
			auto desc = Get()->GetDesc();
			result->Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			result->ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
			result->TextureCube.MostDetailedMip = 0;
			result->TextureCube.MipLevels = desc.MipLevels;
			result->TextureCube.ResourceMinLODClamp = 0.0f;
			result->Format = desc.Format;
		}
		return result;
	}

	SharedPtr<D3D12_UNORDERED_ACCESS_VIEW_DESC> DirectX12Texture::CreateUAVDesc(ETextureFlags flag) const
	{
		return nullptr;
	}

}
