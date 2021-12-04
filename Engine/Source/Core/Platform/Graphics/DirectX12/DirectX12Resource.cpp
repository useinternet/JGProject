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


	bool DirectX12VertexBuffer::SetData(const void* datas, u64 elementSize, u64 elementCount)
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
				auto commandList = DirectX12API::GetCopyCommandList(MAIN_GRAPHICS_COMMAND_ID);
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
		auto commandList = DirectX12API::GetGraphicsCommandList(commandID);


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

	bool DirectX12IndexBuffer::SetData(const u32* datas, u64 count)
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
				auto commandList = DirectX12API::GetCopyCommandList(MAIN_GRAPHICS_COMMAND_ID);
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
		auto commandList = DirectX12API::GetGraphicsCommandList(commandID);
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



	DirectX12ReadWriteBuffer::~DirectX12ReadWriteBuffer()
	{
		Reset();
	}
	bool DirectX12ReadWriteBuffer::IsValid() const
	{
		return mD3DResource != nullptr;
	}
	bool DirectX12ReadWriteBuffer::SetData(u64 btSize)
	{
		u64 originBtSize = mDataSize;
		mDataSize = btSize;

		if (mD3DResource != nullptr && originBtSize != btSize)
		{
			Reset();
		}

		mD3DResource = DirectX12API::CreateCommittedResource(
			GetName(),
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(btSize, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS),
			D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
			nullptr);

		return mD3DResource != nullptr;
	}
	u64 DirectX12ReadWriteBuffer::GetDataSize() const
	{
		return mDataSize;
	}
	BufferID DirectX12ReadWriteBuffer::GetBufferID() const
	{
		if (IsValid() == true)
		{
			return mD3DResource->GetGPUVirtualAddress();
		}
		return 0;
	}

	void DirectX12ReadWriteBuffer::Reset()
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

	bool DirectX12ReadBackBuffer::Read(SharedPtr<IReadWriteBuffer> readWriteBuffer)
	{
		if (readWriteBuffer == nullptr || readWriteBuffer->IsValid() == false || mState == EReadBackBufferState::Reading)
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

		auto dxRWBuffer = static_cast<DirectX12ReadWriteBuffer*>(readWriteBuffer.get());
		auto commandList = DirectX12API::GetComputeCommandList(MAIN_GRAPHICS_COMMAND_ID);
		commandList->CopyResource(Get(), dxRWBuffer->Get());

		mState = EReadBackBufferState::Reading;

		Scheduler::GetInstance().ScheduleOnceByFrame(DirectX12API::GetFrameBufferCount() + 1, 0, [&]()->EScheduleResult
		{
			mState = EReadBackBufferState::ReadCompelete;
			return EScheduleResult::Break;
		});

		return IsValid();
	}

	bool DirectX12ReadBackBuffer::GetData(void* out_data,u64 out_data_size)
	{
		if (IsValid() == false || mState == EReadBackBufferState::Reading)
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

	EReadBackBufferState DirectX12ReadBackBuffer::GetState() const
	{
		return mState;
	}

	void DirectX12ReadBackBuffer::Reset()
	{
		if (mD3DResource)
		{
			mD3DResource->Unmap(0, nullptr);
			mCPU = nullptr;
			mBufferSize = 0;
			mState = EReadBackBufferState::ReadCompelete;
			DirectX12API::DestroyCommittedResource(mD3DResource);
			mD3DResource.Reset();
			mD3DResource = nullptr;
		}
	}
	bool DirectX12Computer::SetFloat(const String& name, float value)
	{
		return mShaderData->SetFloat(name, value);
	}
	bool DirectX12Computer::SetFloat2(const String& name, const JVector2& value)
	{
		return  mShaderData->SetFloat2(name, value);
	}
	bool DirectX12Computer::SetFloat3(const String& name, const JVector3& value)
	{
		return  mShaderData->SetFloat3(name, value);
	}
	bool DirectX12Computer::SetFloat4(const String& name, const JVector4& value)
	{
		return  mShaderData->SetFloat4(name, value);
	}
	bool DirectX12Computer::SetInt(const String& name, i32 value)
	{
		return mShaderData->SetInt(name, value);
	}
	bool DirectX12Computer::SetInt2(const String& name, const JVector2Int& value)
	{
		return mShaderData->SetInt2(name, value);
	}
	bool DirectX12Computer::SetInt3(const String& name, const JVector3Int& value)
	{
		return mShaderData->SetInt3(name, value);
	}
	bool DirectX12Computer::SetInt4(const String& name, const JVector4Int& value)
	{
		return mShaderData->SetInt4(name, value);
	}
	bool DirectX12Computer::SetUint(const String& name, u32 value)
	{
		return mShaderData->SetUint(name, value);
	}
	bool DirectX12Computer::SetUint2(const String& name, const JVector2Uint& value)
	{
		return mShaderData->SetUint2(name, value);
	}
	bool DirectX12Computer::SetUint3(const String& name, const JVector3Uint& value)
	{
		return mShaderData->SetUint3(name, value);
	}
	bool DirectX12Computer::SetUint4(const String& name, const JVector4Uint& value)
	{
		return mShaderData->SetUint4(name, value);
	}
	bool DirectX12Computer::SetFloat4x4(const String& name, const JMatrix& value)
	{
		return mShaderData->SetFloat4x4(name, value);
	}

	bool DirectX12Computer::SetTexture(const String& name, u32 textureSlot, SharedPtr<ITexture> texture)
	{
		return mShaderData->SetTexture(name, textureSlot, texture);
	}

	bool DirectX12Computer::SetFloatArray(const String& name, const List<float>& value)
	{
		return mShaderData->SetFloatArray(name, value);
	}

	bool DirectX12Computer::SetFloat2Array(const String& name, const List<JVector2>& value)
	{
		return mShaderData->SetFloat2Array(name, value);
	}

	bool DirectX12Computer::SetFloat3Array(const String& name, const List<JVector3>& value)
	{
		return mShaderData->SetFloat3Array(name, value);
	}

	bool DirectX12Computer::SetFloat4Array(const String& name, const List<JVector4>& value)
	{
		return mShaderData->SetFloat4Array(name, value);
	}

	bool DirectX12Computer::SetIntArray(const String& name, const List<i32>& value)
	{
		return mShaderData->SetIntArray(name, value);
	}

	bool DirectX12Computer::SetInt2Array(const String& name, const List<JVector2Int>& value)
	{
		return mShaderData->SetInt2Array(name, value);
	}

	bool DirectX12Computer::SetInt3Array(const String& name, const List<JVector3Int>& value)
	{
		return mShaderData->SetInt3Array(name, value);
	}

	bool DirectX12Computer::SetInt4Array(const String& name, const List<JVector4Int>& value)
	{
		return mShaderData->SetInt4Array(name, value);
	}

	bool DirectX12Computer::SetUintArray(const String& name, const List<u32>& value)
	{
		return mShaderData->SetUintArray(name, value);
	}

	bool DirectX12Computer::SetUint2Array(const String& name, const List<JVector2Uint>& value)
	{
		return mShaderData->SetUint2Array(name, value);
	}

	bool DirectX12Computer::SetUint3Array(const String& name, const List<JVector3Uint>& value)
	{
		return mShaderData->SetUint3Array(name, value);
	}

	bool DirectX12Computer::SetUint4Array(const String& name, const List<JVector4Uint>& value)
	{
		return mShaderData->SetUint4Array(name, value);
	}

	bool DirectX12Computer::SetFloat4x4Array(const String& name, const List<JMatrix>& value)
	{
		return mShaderData->SetFloat4x4Array(name, value);
	}

	bool DirectX12Computer::SetStructDataArray(const String& name, void* datas, u64 elementCount, u64 elementSize)
	{
		return mShaderData->SetStructDataArray(name, datas, elementCount, elementSize);
	}

	bool DirectX12Computer::GetFloat(const String& name, float* out_value)
	{
		return mShaderData->GetFloat(name, out_value);
	}

	bool DirectX12Computer::GetFloat2(const String& name, JVector2* out_value)
	{
		return mShaderData->GetFloat2(name, out_value);
	}

	bool DirectX12Computer::GetFloat3(const String& name, JVector3* out_value)
	{
		return mShaderData->GetFloat3(name, out_value);
	}

	bool DirectX12Computer::GetFloat4(const String& name, JVector4* out_value)
	{
		return mShaderData->GetFloat4(name, out_value);
	}

	bool DirectX12Computer::GetInt(const String& name, i32* out_value)
	{
		return mShaderData->GetInt(name, out_value);
	}

	bool DirectX12Computer::GetInt2(const String& name, JVector2Int* out_value)
	{
		return mShaderData->GetInt2(name, out_value);
	}

	bool DirectX12Computer::GetInt3(const String& name, JVector3Int* out_value)
	{
		return mShaderData->GetInt3(name, out_value);
	}

	bool DirectX12Computer::GetInt4(const String& name, JVector4Int* out_value)
	{
		return mShaderData->GetInt4(name, out_value);
	}

	bool DirectX12Computer::GetUint(const String& name, u32* out_value)
	{
		return mShaderData->GetUint(name, out_value);
	}

	bool DirectX12Computer::GetUint2(const String& name, JVector2Uint* out_value)
	{
		return mShaderData->GetUint2(name, out_value);
	}

	bool DirectX12Computer::GetUint3(const String& name, JVector3Uint* out_value)
	{
		return mShaderData->GetUint3(name, out_value);
	}

	bool DirectX12Computer::GetUint4(const String& name, JVector4Uint* out_value)
	{
		return mShaderData->GetUint4(name, out_value);
	}

	bool DirectX12Computer::GetFloat4x4(const String& name, JMatrix* out_value)
	{
		return mShaderData->GetFloat4x4(name, out_value);
	}

	bool DirectX12Computer::GetTexture(const String& name, u32 textureSlot, SharedPtr<ITexture>* out_value)
	{
		return mShaderData->GetTexture(name, textureSlot, out_value);
	}

	SharedPtr<IReadWriteBuffer> DirectX12Computer::GetRWBuffer(const String& name)
	{
		return mShaderData->GetRWData(name);
	}

	void DirectX12Computer::Init(SharedPtr<IShader> shader)
	{
		mShaderData = CreateUniquePtr<ShaderData>(shader);
	}
	const String& DirectX12Computer::GetName() const
	{
		return mName;
	}
	void DirectX12Computer::SetName(const String& name)
	{
		mName = name;
	}

	EComputerState DirectX12Computer::GetState() const
	{
		return mState;
	}

	bool DirectX12Computer::Dispatch(u32 groupX, u32 groupY, u32 groupZ)
	{
		if (mState == EComputerState::Run)
		{
			return false;
		}
		if (mShaderData->Bind(MAIN_GRAPHICS_COMMAND_ID) == false)
		{
			return false;
		}
		auto PSO = DirectX12API::GetComputePipelineState(MAIN_GRAPHICS_COMMAND_ID);
		if (PSO->Finalize() == false)
		{
			return false;
		}





		mState = EComputerState::Run;
		auto commandList = DirectX12API::GetComputeCommandList(MAIN_GRAPHICS_COMMAND_ID);
		commandList->BindPipelineState(PSO);
		commandList->Dispatch(groupX, groupY, groupZ);


		if (mScheduleHandle && mScheduleHandle->IsValid())
		{
			mScheduleHandle->Reset();
		}
		mScheduleHandle = Scheduler::GetInstance().ScheduleByFrame(
			DirectX12API::GetFrameBufferCount() + 1, 0, 1, 0,
			[&]() -> EScheduleResult
		{
			mState = EComputerState::Compelete;
			return EScheduleResult::Break;
		});

		return true;
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
			mD3DResource->SetName(s2ws(name).c_str());
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
			mD3DResource->SetName(s2ws(GetName()).c_str());
		}
	}
	void DirectX12Texture::SetTextureMemory(const byte* pixels, i32 width, i32 height, i32 channels, u32 pixelPerUnit)
	{
		TextureInfo info;
		info.ArraySize = 1;
		info.Flags = ETextureFlags::None;
		info.MipLevel = 1;
		info.Width = (u32)width;
		info.Height = (u32)height;
		info.Format = ETextureFormat::R8G8B8A8_Unorm;
		info.PixelPerUnit = pixelPerUnit;
		SetTextureInfo(info);
		auto commandList = DirectX12API::GetCopyCommandList(MAIN_GRAPHICS_COMMAND_ID);
		commandList->CopyTextrueFromMemory(Get(), pixels, width, height, channels);
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
		return nullptr;
	}

	SharedPtr<D3D12_UNORDERED_ACCESS_VIEW_DESC> DirectX12Texture::CreateUAVDesc(ETextureFlags flag) const
	{
		return nullptr;
	}
	











}
