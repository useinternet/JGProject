#include "DirectX12API.h"
#include "pch.h"

#include "DirectX12API.h"
#include "DirectX12FrameBuffer.h"
#include "DirectX12Resource.h"
#include "DirectX12Shader.h"
#include "DirectX12Material.h"
#include "DirectX12Mesh.h"
#include "DirectX12RootSignature.h"
#include "RayTracing/DirectX12RayTracingPipeline.h"
#include "RayTracing/DirectX12BottomLevelAccelerationStructure.h"
#include "RayTracing/DirectX12TopLevelAccelerationStructure.h"
#include "RayTracing/DirectX12RaytracingShaderResourceTable.h"


#include "Utill/DirectX12Helper.h"
#include "Utill/DescriptorAllocator.h"
#include "Utill/CommandQueue.h"
#include "Utill/RootSignature.h"
#include "Utill/PipelineState.h"
#include "Utill/CommandList.h"
#include "Utill/ResourceStateTracker.h"
#include "Class/Asset/Asset.h"
#include "Graphics/JGGraphics.h"
#include "Graphics/RayTracing/Raytracer.h"
namespace JG
{
	DirectX12API* DirectX12API::sm_DirectX12API = nullptr;
	EGraphicsAPI DirectX12API::GetAPI() const
	{
		return EGraphicsAPI::DirectX12;
	}
	u64 DirectX12API::GetBufferCount() const
	{
		return mFrameBufferCount;
	}
	u64 DirectX12API::GetBufferIndex() const 
	{
		return mFrameBufferIndex;
	}
	DirectX12API* DirectX12API::GetInstance()
	{
		return sm_DirectX12API;
	}
	IDXGIFactory4* DirectX12API::GetDXGIFactory()
	{
		return GetInstance()->mFactory.Get();
	}

	ID3D12Device5* DirectX12API::GetD3DDevice()
	{
		return GetInstance()->mDevice.Get();
	}
	CommandQueue* DirectX12API::GetGraphicsCommandQueue()
	{
		return GetInstance()->mGraphicsCommandQueue.get();
	}
	CommandQueue* DirectX12API::GetComputeCommandQueue()
	{
		return GetInstance()->mComputeCommandQueue.get();
	}
	CommandQueue* DirectX12API::GetCopyCommandQueue()
	{
		return GetInstance()->mCopyCommandQueue.get();
	}
	DescriptorAllocation DirectX12API::RTVAllocate()
	{
		return GetInstance()->mRTVAllocator->Allocate();
	}
	DescriptorAllocation DirectX12API::DSVAllocate()
	{
		return GetInstance()->mDSVAllocator->Allocate();
	}
	DescriptorAllocation DirectX12API::CSUAllocate()
	{
		return GetInstance()->mCSUAllocator->Allocate();
	}

	GraphicsCommandList* DirectX12API::GetGraphicsCommandList()
	{
		return static_cast<GraphicsCommandList*>(GetGraphicsCommandQueue()->RequestCommandList());
	}
	ComputeCommandList* DirectX12API::GetComputeCommandList()
	{
		return static_cast<ComputeCommandList*>(GetComputeCommandQueue()->RequestCommandList());
	}
	CopyCommandList* DirectX12API::GetCopyCommandList()
	{
		return static_cast<CopyCommandList*>(GetCopyCommandQueue()->RequestCommandList());
	}

	SharedPtr<GraphicsPipelineState> DirectX12API::GetGraphicsPipelineState()
	{
		DirectX12API* instance = GetInstance();

		std::lock_guard<std::mutex> lock(instance->mGraphicsPSOMutex);

		std::thread::id curr_thread_id = std::this_thread::get_id();
		if (instance->mGraphicsPSOs[curr_thread_id] == nullptr)
		{
			instance->mGraphicsPSOs[curr_thread_id] = CreateSharedPtr<GraphicsPipelineState>();
		}
		return instance->mGraphicsPSOs[curr_thread_id];
	}

	SharedPtr<ComputePipelineState> DirectX12API::GetComputePipelineState()
	{
		DirectX12API* instance = GetInstance();
		std::lock_guard<std::mutex> lock(instance->mComputePSOMutex);

		std::thread::id curr_thread_id = std::this_thread::get_id();
		if (instance->mComputePSOs[curr_thread_id] == nullptr)
		{
			instance->mComputePSOs[curr_thread_id] = CreateSharedPtr<ComputePipelineState>();
		}
		return instance->mComputePSOs[curr_thread_id];
	}

	void DirectX12API::GetDepthStencilDesc(EDepthStencilStateTemplate _template, D3D12_DEPTH_STENCIL_DESC* out)
	{
		auto desc = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		switch (_template)
		{
		case EDepthStencilStateTemplate::NoDepth:
			desc.DepthEnable = false;
			break;
		case EDepthStencilStateTemplate::LessEqual:
			desc.DepthEnable    = true;
			desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
			desc.DepthFunc      = D3D12_COMPARISON_FUNC_LESS_EQUAL;
			break;
		default:
			break;
		}
		if (out != nullptr)
		{
			*out = desc;
		}
	}

	void DirectX12API::GetBlendDesc(EBlendStateTemplate _template, D3D12_RENDER_TARGET_BLEND_DESC* out)
	{
		auto desc = D3D12_RENDER_TARGET_BLEND_DESC
		{
			FALSE,FALSE,
			D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
			D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
			D3D12_LOGIC_OP_NOOP,
			D3D12_COLOR_WRITE_ENABLE_ALL,
		};


		switch (_template)
		{
		case EBlendStateTemplate::Transparent_Default:
			desc.BlendEnable    = true;
			desc.SrcBlend       = D3D12_BLEND_SRC_ALPHA;
			desc.DestBlend      = D3D12_BLEND_INV_SRC_ALPHA;
			desc.BlendOp        = D3D12_BLEND_OP_ADD;
			desc.SrcBlendAlpha  = D3D12_BLEND_ONE;
			desc.DestBlendAlpha = D3D12_BLEND_ZERO;
			desc.BlendOpAlpha   = D3D12_BLEND_OP_ADD;
			desc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL & ~D3D12_COLOR_WRITE_ENABLE_ALPHA;
			break;
		}

		if (out != nullptr)
		{
			*out = desc;
		}
	}

	void DirectX12API::GetRasterizerDesc(ERasterizerStateTemplate _template, D3D12_RASTERIZER_DESC* out)
	{
		auto desc = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);

		switch (_template)
		{
		case ERasterizerStateTemplate::Cull_None:
			desc.CullMode = D3D12_CULL_MODE_NONE;
			break;
		}

		if (out != nullptr)
		{
			*out = desc;
		}
	}

	Microsoft::WRL::ComPtr<ID3D12Resource> DirectX12API::CreateCommittedResource(
		const String& name,
		const D3D12_HEAP_PROPERTIES* pHeapProperties, 
		D3D12_HEAP_FLAGS HeapFlags, 
		const D3D12_RESOURCE_DESC* pDesc,
		D3D12_RESOURCE_STATES InitialResourceState, 
		const D3D12_CLEAR_VALUE* pOptimizedClearValue)
	{
		DirectX12API* instance = GetInstance();
		Microsoft::WRL::ComPtr<ID3D12Resource> resultResource;
		HRESULT hResult = S_OK;
		{
			std::lock_guard<std::mutex> lock(instance->mDeviceMutex);
			hResult = instance->mDevice->CreateCommittedResource(pHeapProperties, HeapFlags, pDesc, InitialResourceState, pOptimizedClearValue, IID_PPV_ARGS(resultResource.GetAddressOf()));
		}
		if (SUCCEEDED(hResult))
		{
			ResourceStateTracker::RegisterResource(name, resultResource.Get(), InitialResourceState);
		}
		return resultResource;
	}

	void DirectX12API::DestroyCommittedResource(Microsoft::WRL::ComPtr<ID3D12Resource> resource)
	{
		if (resource == nullptr)
		{
			return;
		}
		ResourceStateTracker::UnRegisterResource(resource.Get());
	}

	bool DirectX12API::Create()
	{
		if (sm_DirectX12API != nullptr)
		{
			return false;
		}
		sm_DirectX12API = this;
		JG_LOG_INFO("DirectX12 Init Start");

		//
		mFactory = CreateDXGIFactory();
		if (mFactory)
		{
			JG_LOG_INFO("Success Create DXGIFactroy");
		}
		else
		{
			JG_LOG_ERROR("Failed Create DXGIFactory");
			return false;
		}
		DXGI_ADAPTER_DESC1 adapterDesc = {};
		mDevice = CreateD3DDevice(mFactory, false, &adapterDesc, &mIsSupportedRayTracing);

		if (mDevice)
		{
			JG_LOG_INFO("Success Create D3D12Device");
			JG_LOG_TRACE("Description : " + StringHelper::ws2s(adapterDesc.Description));
			JG_LOG_TRACE("VideoMemory : {0}  MB", adapterDesc.DedicatedVideoMemory / 1024 / 1024);
		}
		else
		{
			JG_LOG_ERROR("Failed Create D3D12Device");
			return false;
		}

		// NOTE
		// DescriptiorAllcator 생성
		JG_LOG_INFO("Create DescriptorAllocator...");
		mCSUAllocator = CreateUniquePtr<DescriptorAllocator>(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		mRTVAllocator = CreateUniquePtr<DescriptorAllocator>(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		mDSVAllocator = CreateUniquePtr<DescriptorAllocator>(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);


		JG_LOG_INFO("Create CommandQueue...");
		mGraphicsCommandQueue = CreateUniquePtr<CommandQueue>(mFrameBufferCount, D3D12_COMMAND_LIST_TYPE_DIRECT);
		mComputeCommandQueue  = CreateUniquePtr<CommandQueue>(mFrameBufferCount, D3D12_COMMAND_LIST_TYPE_COMPUTE);
		mCopyCommandQueue     = CreateUniquePtr<CommandQueue>(mFrameBufferCount, D3D12_COMMAND_LIST_TYPE_COPY);


		JG_LOG_INFO("DirectX12 Init End");
		return true;
	}


	void DirectX12API::Destroy()
	{
		Flush();

		mGraphicsPSOs.clear();
		mComputePSOs.clear();
		for (auto& _pair : mFrameBuffers)
		{
			_pair.second->Reset();
		}
		mFrameBuffers.clear();

		mCSUAllocator.reset();
		mRTVAllocator.reset();
		mDSVAllocator.reset();
		
		mGraphicsCommandQueue.reset();
		mComputeCommandQueue.reset();
		mCopyCommandQueue.reset();


		PipelineState::ClearCache();
		RootSignature::ClearCache();
		ResourceStateTracker::ClearCache();

		mDevice.Reset();
		mFactory.Reset();
	}


	void DirectX12API::BeginFrame()
	{
		mGraphicsCommandQueue->Begin();
		mComputeCommandQueue->Begin();
		mCopyCommandQueue->Begin();

		mGraphicsContextDic.clear();
		mComputeContextDic.clear();
		
	}
	void DirectX12API::EndFrame()
	{
		// TODO
		// FrameBuffer Update
		for (auto& iter : mFrameBuffers)
		{
			iter.second->Update();
		}

		mGraphicsCommandQueue->End();
		mComputeCommandQueue->End();
		mCopyCommandQueue->End();


		// TODO
		// SwapBuffer
		for (auto& iter : mFrameBuffers)
		{
			iter.second->Present();
		}


		mFrameBufferIndex = (mFrameBufferIndex + 1) % mFrameBufferCount;
		mCSUAllocator->UpdatePage();
		mRTVAllocator->UpdatePage();
		mRTVAllocator->UpdatePage();
	}
	bool DirectX12API::IsSupportedRayTracing() const
	{
		return mIsSupportedRayTracing;
	}
	void DirectX12API::Flush()
	{
		mGraphicsCommandQueue->Flush();
		mComputeCommandQueue->Flush();
		mCopyCommandQueue->Flush();
	}

	SharedPtr<IFrameBuffer> DirectX12API::CreateFrameBuffer(const FrameBufferInfo& info)
	{
		if (info.Handle == 0) return nullptr;
		auto iter = mFrameBuffers.find(info.Handle);
		if (iter != mFrameBuffers.end())
		{
			return mFrameBuffers[info.Handle];
		}

		auto buffer = CreateSharedPtr<DirectX12FrameBuffer>();
		if (!buffer->Init(info))
		{
			JG_LOG_ERROR("Failed Create DirectX12RenderContext");
			return nullptr;
		}
		mFrameBuffers.emplace(info.Handle, buffer);
		return buffer;
	}
	SharedPtr<IVertexBuffer> DirectX12API::CreateVertexBuffer(const String& name, EBufferLoadMethod method)
	{
		auto vBuffer = CreateSharedPtr<DirectX12VertexBuffer>();
		vBuffer->SetName(name);
		vBuffer->SetBufferLoadMethod(method);
		return vBuffer;
	}
	SharedPtr<IIndexBuffer> DirectX12API::CreateIndexBuffer(const String& name, EBufferLoadMethod method)
	{
		auto iBuffer = CreateSharedPtr<DirectX12IndexBuffer>();
		iBuffer->SetName(name);
		iBuffer->SetBufferLoadMethod(method);
		return iBuffer;
	}
	SharedPtr<IStructuredBuffer> DirectX12API::CreateStrucuredBuffer(const String& name, u64 elementSize, u64 elementCount, EBufferLoadMethod method)
	{
		auto sBuffer = CreateSharedPtr<DirectX12StructuredBuffer>();
		sBuffer->SetName(name);
		sBuffer->SetBufferLoadMethod(method);
		sBuffer->SetData(elementSize, elementCount);
		return sBuffer;
	}
	SharedPtr<IByteAddressBuffer> DirectX12API::CreateByteAddressBuffer(const String& name, u64 elementCount)
	{
		auto btAddrBuffer = CreateSharedPtr<DirectX12ByteAddressBuffer>();
		btAddrBuffer->SetName(name);
		btAddrBuffer->SetData(elementCount);
		return btAddrBuffer;
	}
	SharedPtr<IReadBackBuffer> DirectX12API::CreateReadBackBuffer(const String& name)
	{
		auto rbBuffer = CreateSharedPtr<DirectX12ReadBackBuffer>();
		rbBuffer->SetName(name);
	
		return rbBuffer;
	}
	SharedPtr<IGraphicsShader> DirectX12API::CreateGraphicsShader(const String& name, const String& sourceCode, EShaderFlags flags, const List<SharedPtr<IShaderScript>>& scriptList)
	{
		String errorCode;
		auto shader = CreateSharedPtr<DirectX12GraphicsShader>();
		shader->SetName(name);
		if (shader->Compile(sourceCode, scriptList, flags, &errorCode) == false)
		{
			JG_LOG_ERROR("Failed Compile Shader : {0}  Error : {1} ", name, errorCode);
			return nullptr;
		}


		return shader;
	}
	SharedPtr<IComputeShader> DirectX12API::CreateComputeShader(const String& name, const String& sourceCode)
	{
		String errorCode;
		auto shader = CreateSharedPtr<DirectX12ComputeShader>();
		shader->SetName(name);
		if (shader->Compile(sourceCode, &errorCode) == false)
		{
			errorCode = StringHelper::ReplaceAll(errorCode,"\n", "");
			JG_LOG_ERROR("Failed Compile Shader : {0} Error : {1}", name, errorCode);
			return nullptr;
		}


		return shader;
	}

	SharedPtr<IClosestHitShader> DirectX12API::CreateClosestHitShader(const String& name, const String& sourceCode, const SharedPtr<IShaderScript> script)
	{
		auto shader = CreateSharedPtr<DirectX12ClosestHitShader>();
		shader->SetName(name);
		shader->Init(sourceCode, script);
		return shader;
	}
	SharedPtr<IMaterial> DirectX12API::CreateMaterial(const String& name)
	{
		auto material = CreateSharedPtr<DirectX12Material>();
		material->SetName(name);
		return material;
	}
	SharedPtr<IMaterial> DirectX12API::CreateMaterial(const String& name, SharedPtr<IGraphicsShader> shader)
	{
		if (shader == nullptr)
		{
			return nullptr;
		}
		auto material = CreateSharedPtr<DirectX12Material>();
		material->SetName(name);
		material->Init(shader);
		return material;
	}

	SharedPtr<IMesh> DirectX12API::CreateMesh(const String& name)
	{
		auto mesh = CreateSharedPtr<DirectX12Mesh>();
		mesh->SetName(name);
		return mesh;
	}
	SharedPtr<ISubMesh> DirectX12API::CreateSubMesh(const String& name)
	{
		auto mesh = CreateSharedPtr<DirectX12SubMesh>();
		mesh->SetName(name);
		return mesh;
	}
	SharedPtr<ITexture> DirectX12API::CreateTexture(const String& name)
	{
		auto texture = CreateSharedPtr<DirectX12Texture>();
		texture->SetName(name);
		return texture;
	}
	SharedPtr<ITexture> DirectX12API::CreateTexture(const String& name, const TextureInfo& info)
	{

		auto texture = CreateSharedPtr<DirectX12Texture>();
		texture->Create(name, info);

		return texture;
	}
	SharedPtr<IRootSignatureCreater> DirectX12API::CreateRootSignatureCreater()
	{
		return CreateSharedPtr<DirectX12RootSignatureCreater>();
	}
	SharedPtr<IRayTracingPipeline> DirectX12API::CreateRayTracingPipeline()
	{
		return CreateSharedPtr<DirectX12RayTracingPipeline>();
	}
	SharedPtr<IRayTracingShaderResourceTable> DirectX12API::CreateRayTracingShaderResourceTable()
	{
		return CreateSharedPtr<DirectX12RayTracingShaderResourceTable>();
	}
	SharedPtr<ITopLevelAccelerationStructure> DirectX12API::CreateTopLevelAccelerationStructure()
	{
		return CreateSharedPtr<DirectX12TopLevelAccelerationStructure>();
	}
	SharedPtr<IBottomLevelAccelerationStructure> DirectX12API::CreateBottomLevelAccelerationStructure()
	{
		return CreateSharedPtr<DirectX12BottomLevelAccelerationsStructure>();
	}
	SharedPtr<IGraphicsContext> DirectX12API::GetGraphicsContext()
	{
		std::lock_guard<std::mutex> lock(mGraphicsContextMutex);

		std::thread::id curr_thread_id = std::this_thread::get_id();

		if (mGraphicsContextDic.find(curr_thread_id) == mGraphicsContextDic.end())
		{
			SharedPtr<DirectX12GraphicsContext> graphicsContext = CreateSharedPtr<DirectX12GraphicsContext>();
			graphicsContext->Reset();
			mGraphicsContextDic[curr_thread_id] = graphicsContext;
			return graphicsContext;
		}

		return mGraphicsContextDic[curr_thread_id];
	}
	SharedPtr<IComputeContext> DirectX12API::GetComputeContext()
	{
		std::lock_guard<std::mutex> lock(mComputeContextMutex);

		std::thread::id curr_thread_id = std::this_thread::get_id();

		if (mComputeContextDic.find(curr_thread_id) == mComputeContextDic.end())
		{
			SharedPtr<DirectX12ComputeContext> computeContext = CreateSharedPtr<DirectX12ComputeContext>();
			computeContext->Reset();
			mComputeContextDic[curr_thread_id] = computeContext;
			return computeContext;
		}

		return mComputeContextDic[curr_thread_id];
	}

















	// 초기 셋팅 함수
	void DirectX12GraphicsContext::ClearRenderTarget(const List<SharedPtr<ITexture>>& rtTextures, SharedPtr<ITexture> depthTexture)
	{
		if (mCommandList == nullptr)
		{
			return;
		}

		for (auto& texture : rtTextures)
		{
			if (texture == nullptr || texture->IsValid() == false) continue;
			auto handle = static_cast<DirectX12Texture*>(texture.get())->GetRTV();
			if (handle.ptr == 0) continue;


			TextureInfo info = texture->GetTextureInfo();

			mCommandList->ClearRenderTargetTexture(static_cast<DirectX12Texture*>(texture.get())->Get(), handle, info.ClearColor);
		}
		if (depthTexture && depthTexture->IsValid())
		{
			auto handle = static_cast<DirectX12Texture*>(depthTexture.get())->GetDSV();

			if (handle.ptr != 0)
			{
				TextureInfo info = depthTexture->GetTextureInfo();

				mCommandList->ClearDepthTexture(static_cast<DirectX12Texture*>(depthTexture.get())->Get(),
					handle, info.ClearDepth, info.ClearStencil);
			}
		}
	}
	void DirectX12GraphicsContext::SetRenderTarget(const List<SharedPtr<ITexture>>& rtTextures, SharedPtr<ITexture> depthTexture)
	{
		if (mCommandList == nullptr)
		{
			return;
		}

		auto pso = DirectX12API::GetGraphicsPipelineState();
		List<DXGI_FORMAT> rtFormats;
		DXGI_FORMAT dsFormat = DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;


		List<ID3D12Resource*> d3dRTResources;
		List<D3D12_CPU_DESCRIPTOR_HANDLE> rtvHandles;

		ID3D12Resource* d3dDSResource = nullptr;
		UniquePtr<D3D12_CPU_DESCRIPTOR_HANDLE> dsvHandle = nullptr;

		for (auto& texture : rtTextures)
		{
			if (texture == nullptr || texture->IsValid() == false) continue;
			auto handle = static_cast<DirectX12Texture*>(texture.get())->GetRTV();
			if (handle.ptr == 0) continue;


			rtFormats.push_back(ConvertDXGIFormat(texture->GetTextureInfo().Format));

			d3dRTResources.push_back(static_cast<DirectX12Texture*>(texture.get())->Get());
			rtvHandles.push_back(handle);


		}
		if (depthTexture && depthTexture->IsValid())
		{
			auto handle = static_cast<DirectX12Texture*>(depthTexture.get())->GetDSV();
			if (handle.ptr != 0)
			{
				dsFormat = ConvertDXGIFormat(depthTexture->GetTextureInfo().Format);
				d3dDSResource = static_cast<DirectX12Texture*>(depthTexture.get())->Get();
				dsvHandle = CreateUniquePtr<D3D12_CPU_DESCRIPTOR_HANDLE>();
				*dsvHandle = handle;
			}
		}

		pso->BindRenderTarget(rtFormats, dsFormat);
		mCommandList->SetRenderTarget(d3dRTResources.data(), rtvHandles.data(), d3dRTResources.size(), d3dDSResource, dsvHandle.get());
	}


	void DirectX12GraphicsContext::SetViewports(const List<Viewport>& viewPorts)
	{
		mCommandList->SetViewports(viewPorts);
	}
	void DirectX12GraphicsContext::SetScissorRects(const List<ScissorRect>& scissorRects)
	{
		mCommandList->SetScissorRects(scissorRects);
	}
	void DirectX12GraphicsContext::BindRootSignature(SharedPtr<IRootSignature> rootSig)
	{
		if (rootSig == nullptr)
		{
			return;
		}

		DirectX12RootSignature* dx12RootSig = static_cast<DirectX12RootSignature*>(rootSig.get());

		mCommandList->BindRootSignature(dx12RootSig->Get());
		mBindedRootSignature = rootSig;


		auto pso = DirectX12API::GetGraphicsPipelineState();
		pso->BindRootSignature(*(dx12RootSig->Get()));


	}
	void DirectX12GraphicsContext::BindShader(SharedPtr<IGraphicsShader> shader)
	{
		DirectX12GraphicsShader* pDX12Shader = static_cast<DirectX12GraphicsShader*>(shader.get());
		auto pso = DirectX12API::GetGraphicsPipelineState();
		pso->BindShader(*pDX12Shader);
	}
	// Bind 함수
	void DirectX12GraphicsContext::BindTextures(u32 rootParam, const List<SharedPtr<ITexture>>& textures)
	{
		if (textures.empty() || mCommandList == nullptr)
		{
			return;
		}
		DirectX12RootSignature* dx12RootSig = static_cast<DirectX12RootSignature*>(mBindedRootSignature.get());
		
		EDescriptorTableRangeType type = dx12RootSig->GetDescriptorRangeType(rootParam);
		List<D3D12_CPU_DESCRIPTOR_HANDLE> handles;

		for (auto& tex : textures)
		{
			DirectX12Texture* dx12Tex = nullptr;
			if (tex == nullptr || tex->IsValid() == false)
			{
				dx12Tex = static_cast<DirectX12Texture*>(ITexture::NullTexture().get());
			}
			else
			{
				dx12Tex = static_cast<DirectX12Texture*>(tex.get());
			}
		
			switch (type)
			{
			case EDescriptorTableRangeType::SRV:
				mCommandList->TransitionBarrier(dx12Tex->Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
				handles.push_back(dx12Tex->GetSRV());
				break;
			case EDescriptorTableRangeType::UAV:
				mCommandList->TransitionBarrier(dx12Tex->Get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
				handles.push_back(dx12Tex->GetUAV());
				break;
			}

		}
		mCommandList->FlushResourceBarrier();
		mCommandList->BindTextures(rootParam, handles);
	}
	void DirectX12GraphicsContext::BindConstantBuffer(u32 rootParam, const void* data, u32 dataSize)
	{
		if (mCommandList == nullptr)
		{
			return;
		}

		mCommandList->BindConstantBuffer(rootParam, data, dataSize);
	}
	void DirectX12GraphicsContext::BindSturcturedBuffer(u32 rootParam, SharedPtr<IStructuredBuffer> sb)
	{
		if (mCommandList == nullptr || sb == nullptr)
		{
			return;
		}
		mCommandList->BindStructuredBuffer(rootParam, sb->GetBufferID(), static_cast<DirectX12StructuredBuffer*>(sb.get())->Get());
	}
	void DirectX12GraphicsContext::BindSturcturedBuffer(u32 rootParam, const void* data, u32 elementSize, u32 elementCount)
	{
		if (mCommandList == nullptr || data == nullptr)
		{
			return;
		}
		mCommandList->BindStructuredBuffer(rootParam, data, elementCount, elementSize);

	}
	void DirectX12GraphicsContext::BindByteAddressBuffer(u32 rootParam, SharedPtr<IByteAddressBuffer> bab)
	{
		if (bab == nullptr || bab->IsValid() || mCommandList == nullptr)
		{
			return;
		}
		DirectX12RootSignature* dx12RootSig = static_cast<DirectX12RootSignature*>(mBindedRootSignature.get());

		EDescriptorTableRangeType type = dx12RootSig->GetDescriptorRangeType(rootParam);
		List<D3D12_CPU_DESCRIPTOR_HANDLE> handles;

		DirectX12ByteAddressBuffer* dx12byteAddressBuffer = static_cast<DirectX12ByteAddressBuffer*>(bab.get());;
		switch (type)
		{
		case EDescriptorTableRangeType::SRV:
			handles.push_back(dx12byteAddressBuffer->GetSRV());
			break;
		case EDescriptorTableRangeType::UAV:
			handles.push_back(dx12byteAddressBuffer->GetUAV());
			break;
		}
		mCommandList->BindTextures(rootParam, handles);
	}
	void DirectX12GraphicsContext::BindVertexAndIndexBuffer(SharedPtr<IVertexBuffer> vertexBuffer, SharedPtr<IIndexBuffer> indexBuffer)
	{
		if (mCommandList == nullptr || vertexBuffer == nullptr || indexBuffer == nullptr)
		{
			return;
		}
		if (vertexBuffer->IsValid() == false || indexBuffer->IsValid() == false)
		{
			return;
		}

		mCommandList->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		{
			auto dx12VBuffer = static_cast<DirectX12VertexBuffer*>(vertexBuffer.get());
			D3D12_VERTEX_BUFFER_VIEW View = {};
			View.BufferLocation = dx12VBuffer->Get()->GetGPUVirtualAddress();
			View.SizeInBytes = (u32)dx12VBuffer->GetElementSize() * (u32)dx12VBuffer->GetElementCount();
			View.StrideInBytes = (u32)dx12VBuffer->GetElementSize();
			mCommandList->BindVertexBuffer(View);

		}
		{
			auto dx12IBuffer = static_cast<DirectX12IndexBuffer*>(indexBuffer.get());
			D3D12_INDEX_BUFFER_VIEW View;
			View.BufferLocation = dx12IBuffer->Get()->GetGPUVirtualAddress();
			View.Format = DXGI_FORMAT_R32_UINT;
			View.SizeInBytes = sizeof(u32) * dx12IBuffer->GetIndexCount();

			mCommandList->BindIndexBuffer(View);
		}


	}
	void DirectX12GraphicsContext::DrawIndexedAfterBindMeshAndMaterial(SharedPtr<IMesh> mesh, const List<SharedPtr<IMaterial>>& materialList)
	{

		if (mCommandList == nullptr || mesh == nullptr || materialList.empty())
		{
			return;
		}


		// InputLayout
		SetInputLayout(mesh->GetInputLayout());

		u32 subMeshCount = mesh->GetSubMeshCount();
		for (i32 i = 0; i < subMeshCount; ++i)
		{
			SharedPtr<ISubMesh> subMesh = mesh->GetSubMesh(i);
			BindVertexAndIndexBuffer(subMesh->GetVertexBuffer(), subMesh->GetIndexBuffer());

			SharedPtr<IMaterial> material = nullptr;
			if (materialList.size() <= i)
			{
				material = materialList[0];
			}
			else
			{
				material = materialList[i];
			}
			if (material == nullptr || material->Bind() == false)
			{
				continue;
			}

			material->Bind();

			List<jbyte> materialCB = material->GetMaterialPropertyByteData();

			BindShader(material->GetShader());
			BindTextures((u32)Renderer::ERootParam::Texture2D, material->GetTextureList());
	
			BindConstantBuffer((u32)Renderer::ERootParam::MaterialCB, materialCB.data(), materialCB.size());
			DrawIndexed(subMesh->GetIndexCount(), subMesh->GetInstanceCount(), 0, 0, 0);
		}
	}
	// 셋팅 함수
	void DirectX12GraphicsContext::SetDepthStencilState(EDepthStencilStateTemplate _template)
	{
	
		D3D12_DEPTH_STENCIL_DESC desc;
		DirectX12API::GetDepthStencilDesc(_template, &desc);

		auto pso = DirectX12API::GetGraphicsPipelineState();
		pso->SetDepthStencilState(desc);

	}
	void DirectX12GraphicsContext::SetBlendState(u32 renderTargetSlot, EBlendStateTemplate _template)
	{

		D3D12_BLEND_DESC blendDesc = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		DirectX12API::GetBlendDesc(_template, &blendDesc.RenderTarget[renderTargetSlot]);
		auto pso = DirectX12API::GetGraphicsPipelineState();
		pso->SetBlendState(blendDesc);
	}
	void DirectX12GraphicsContext::SetRasterizerState(ERasterizerStateTemplate _template)
	{
		D3D12_RASTERIZER_DESC      desc;
		DirectX12API::GetRasterizerDesc(_template, &desc);
		auto pso = DirectX12API::GetGraphicsPipelineState();
		pso->SetRasterizerState(desc);


	}
	void DirectX12GraphicsContext::SetInputLayout(SharedPtr<InputLayout> inputLayout)
	{
		if (inputLayout == nullptr)
		{
			return;
		}
		auto pso = DirectX12API::GetGraphicsPipelineState();
		pso->BindInputLayout(*inputLayout);
		pso->SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	}

	// 그리기 함수
	void DirectX12GraphicsContext::DrawIndexed(u32 indexCount, u32 instancedCount, u32 startIndexLocation, u32 startVertexLocation, u32 startInstanceLocation)
	{
		if (mCommandList == nullptr)
		{
			return;
		}

		auto pso = DirectX12API::GetGraphicsPipelineState();
		if (pso->Finalize() == false)
		{
			JG_LOG_ERROR("Failed Create Graphcis PipelineState");
			return;
		}
		mCommandList->BindPipelineState(pso);
		mCommandList->DrawIndexed(indexCount, instancedCount, startIndexLocation, startVertexLocation, startIndexLocation);
	}
	void DirectX12GraphicsContext::Draw(u32 vertexCount, u32 instanceCount, u32 startVertexLocation, u32 startInstanceLocation)
	{
		if (mCommandList == nullptr)
		{
			return;
		}
		auto pso = DirectX12API::GetGraphicsPipelineState();
		if (pso->Finalize() == false)
		{
			JG_LOG_ERROR("Failed Create Graphcis PipelineState");
			return;
		}
		mCommandList->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		mCommandList->BindPipelineState(pso);
		mCommandList->Draw(vertexCount, instanceCount, startVertexLocation, startInstanceLocation);
	}
	void DirectX12GraphicsContext::TransitionBarrier(const List<SharedPtr<ITexture>>& textures, const List<EResourceState>& states)
	{
		if (mCommandList == nullptr)
		{
			return;
		}

		if (textures.size() != states.size())
		{
			JG_LOG_ERROR("Not Match TextureCount And ResourceStateCount");
			return;
		}
		u32 count = textures.size();

		for (u32 i = 0; i < count; ++i)
		{
			DirectX12Texture* dx12Tex = static_cast<DirectX12Texture*>(textures[i].get());
			D3D12_RESOURCE_STATES dx12State = ConvertDX12ResourceState(states[i]);
			mCommandList->TransitionBarrier(dx12Tex->Get(), dx12State);
		}

		mCommandList->FlushResourceBarrier();
	}
	void DirectX12GraphicsContext::UAVBarrier(const List<SharedPtr<ITexture>>& textures)
	{
		if (mCommandList == nullptr)
		{
			return;
		}

		u32 count = textures.size();

		for (u32 i = 0; i < count; ++i)
		{
			DirectX12Texture* dx12Tex = static_cast<DirectX12Texture*>(textures[i].get());
			mCommandList->UAVBarrier(dx12Tex->Get());
		}
		mCommandList->FlushResourceBarrier();
	}
	// 인터페이스 변경 함수
	SharedPtr<IComputeContext> DirectX12GraphicsContext::QueryInterfaceAsComputeContext() const
	{
		if (mCacheComputeCommandList == nullptr)
		{
			mCacheComputeCommandList = mCommandList->QueryInterfaceAsComputeCommandList();
		}
		

		SharedPtr<DirectX12ComputeContext> computeContex = CreateSharedPtr<DirectX12ComputeContext>();
		computeContex->mCommandList = mCacheComputeCommandList.get();


		return computeContex;
	}
	SharedPtr<ICopyContext>	   DirectX12GraphicsContext::QueryInterfaceAsCopyContext() const
	{
		SharedPtr<DirectX12CopyContext> copyContext = CreateSharedPtr<DirectX12CopyContext>();
		copyContext->mCommandList = mCommandList;

		return copyContext;
	}


	void DirectX12GraphicsContext::Reset()
	{
		mCommandList			 = nullptr;
		mBindedRootSignature	 = nullptr;
		mCacheComputeCommandList = nullptr;

		mCommandList = DirectX12API::GetGraphicsCommandList();
	}



	void DirectX12ComputeContext::ClearUAVUint(SharedPtr<IByteAddressBuffer> buffer)
	{
		if (mCommandList == nullptr || buffer == nullptr || buffer->IsValid() == false)
		{
			return;
		}

		DirectX12ByteAddressBuffer* dx12byteAddressBuffer = static_cast<DirectX12ByteAddressBuffer*>(buffer.get());

		mCommandList->ClearUAVUint(dx12byteAddressBuffer->GetUAV(), dx12byteAddressBuffer->Get());



	}
	void DirectX12ComputeContext::ClearUAVFloat(SharedPtr<IByteAddressBuffer> buffer)
	{
		if (mCommandList == nullptr || buffer == nullptr || buffer->IsValid() == false)
		{
			return;
		}

		DirectX12ByteAddressBuffer* dx12byteAddressBuffer = static_cast<DirectX12ByteAddressBuffer*>(buffer.get());

		mCommandList->ClearUAVFloat(dx12byteAddressBuffer->GetUAV(), dx12byteAddressBuffer->Get());
	}
	void DirectX12ComputeContext::BindRootSignature(SharedPtr<IRootSignature> rootSig)
	{
		if (rootSig == nullptr)
		{
			return;
		}

		DirectX12RootSignature* dx12RootSig = static_cast<DirectX12RootSignature*>(rootSig.get());

		mCommandList->BindRootSignature(dx12RootSig->Get());
		mBindedRootSignature = rootSig;


		auto pso = DirectX12API::GetComputePipelineState();
		pso->BindRootSignature(*(dx12RootSig->Get()));
	}
	void DirectX12ComputeContext::BindTextures(u32 rootParam, const List<SharedPtr<ITexture>>& textures)
	{
		if (textures.empty() || mCommandList == nullptr)
		{
			return;
		}
		DirectX12RootSignature* dx12RootSig = static_cast<DirectX12RootSignature*>(mBindedRootSignature.get());

		EDescriptorTableRangeType type = dx12RootSig->GetDescriptorRangeType(rootParam);
		List<D3D12_CPU_DESCRIPTOR_HANDLE> handles;

		for (auto& tex : textures)
		{
			DirectX12Texture* dx12Tex = nullptr;
			if (tex == nullptr || tex->IsValid() == false)
			{
				dx12Tex = static_cast<DirectX12Texture*>(ITexture::NullTexture().get());
			}
			else
			{
				dx12Tex = static_cast<DirectX12Texture*>(tex.get());
			}
			switch (type)
			{
			case EDescriptorTableRangeType::SRV:
				mCommandList->TransitionBarrier(dx12Tex->Get(), D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
				handles.push_back(dx12Tex->GetSRV());
				break;
			case EDescriptorTableRangeType::UAV:
				mCommandList->TransitionBarrier(dx12Tex->Get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
				handles.push_back(dx12Tex->GetUAV());
				break;
			}
		}
		mCommandList->FlushResourceBarrier();
		mCommandList->BindTextures(rootParam, handles);
	}


	void DirectX12ComputeContext::BindConstantBuffer(u32 rootParam, const void* data, u32 dataSize)
	{
		if (mCommandList == nullptr)
		{
			return;
		}

		mCommandList->BindConstantBuffer(rootParam, data, dataSize);
	}
	void DirectX12ComputeContext::BindSturcturedBuffer(u32 rootParam, SharedPtr<IStructuredBuffer> sb)
	{
		if (mCommandList == nullptr || sb == nullptr)
		{
			return;
		}
		mCommandList->BindStructuredBuffer(rootParam, sb->GetBufferID(), static_cast<DirectX12StructuredBuffer*>(sb.get())->Get());
	}
	void DirectX12ComputeContext::BindSturcturedBuffer(u32 rootParam, const void* data, u32 elementSize, u32 elementCount)
	{
		if (mCommandList == nullptr || data == nullptr)
		{
			return;
		}
		mCommandList->BindStructuredBuffer(rootParam, data, elementCount, elementSize);
	}
	void DirectX12ComputeContext::BindByteAddressBuffer(u32 rootParam, SharedPtr<IByteAddressBuffer> bab)
	{
		if (bab == nullptr || bab->IsValid() || mCommandList == nullptr)
		{
			return;
		}
		DirectX12RootSignature* dx12RootSig = static_cast<DirectX12RootSignature*>(mBindedRootSignature.get());

		EDescriptorTableRangeType type = dx12RootSig->GetDescriptorRangeType(rootParam);
		List<D3D12_CPU_DESCRIPTOR_HANDLE> handles;

		DirectX12ByteAddressBuffer* dx12byteAddressBuffer = static_cast<DirectX12ByteAddressBuffer*>(bab.get());
		switch (type)
		{
		case EDescriptorTableRangeType::SRV:
			handles.push_back(dx12byteAddressBuffer->GetSRV());
			break;
		case EDescriptorTableRangeType::UAV:
			handles.push_back(dx12byteAddressBuffer->GetUAV());
			break;
		}
		mCommandList->BindTextures(rootParam, handles);
	}
	void DirectX12ComputeContext::BindAccelerationStructure(u32 rootParam, SharedPtr<ITopLevelAccelerationStructure> as)
	{
		if (mCommandList == nullptr || as == nullptr)
		{
			return;
		}
		auto dx12AS = static_cast<DirectX12TopLevelAccelerationStructure*>(as.get());
		mCommandList->BindStructuredBuffer(rootParam, dx12AS->GetResult()->GetGPUVirtualAddress());
	}
	void DirectX12ComputeContext::Dispatch1D(u32 ThreadCountX, u32 GroupSizeX)
	{

		Dispatch(Math::DivideByMultiple(ThreadCountX, GroupSizeX), 1, 1);
	}
	void DirectX12ComputeContext::Dispatch2D(u32 ThreadCountX, u32 ThreadCountY, u32 GroupSizeX,  u32 GroupSizeY)
	{
		Dispatch(Math::DivideByMultiple(ThreadCountX, GroupSizeX), Math::DivideByMultiple(ThreadCountY, GroupSizeY), 1);
	}
	void DirectX12ComputeContext::Dispatch3D(u32 ThreadCountX, u32 ThreadCountY, u32 ThreadCountZ, u32 GroupSizeX, u32 GroupSizeY, u32 GroupSizeZ)
	{
		Dispatch(Math::DivideByMultiple(ThreadCountX, GroupSizeX), Math::DivideByMultiple(ThreadCountY, GroupSizeY), Math::DivideByMultiple(ThreadCountZ, GroupSizeZ));
	}
	void DirectX12ComputeContext::BindShader(SharedPtr<IComputeShader> shader)
	{
		auto dx12Shader = static_cast<DirectX12ComputeShader*>(shader.get());
		auto PSO = DirectX12API::GetComputePipelineState();
		PSO->BindShader(*dx12Shader);
	}
	void DirectX12ComputeContext::Dispatch(u32 groupX, u32 groupY, u32 groupZ)
	{
		if (mCommandList == nullptr)
		{
			return;
		}
		auto pso = DirectX12API::GetComputePipelineState();
		if (pso->Finalize() == false)
		{
			JG_LOG_ERROR("Failed Create Compute PipelineState");
			return;
		}
		mCommandList->BindPipelineState(pso);
		mCommandList->Dispatch(groupX, groupY, groupZ);
	}
	void DirectX12ComputeContext::DispatchRay(u32 width, u32 height, u32 depth, SharedPtr<IRayTracingPipeline> pipeline, SharedPtr<IRayTracingShaderResourceTable> srt)
	{
		if (DirectX12API::GetInstance()->IsSupportedRayTracing() == false)
		{
			return;
		}
		if (pipeline == nullptr || mCommandList == nullptr)
		{
			return;
		}

		DirectX12RayTracingPipeline* dx12Pipeline = static_cast<DirectX12RayTracingPipeline*>(pipeline.get());
		mCommandList->BindPipelineState(dx12Pipeline->GetPipelineState());


		DirectX12RayTracingShaderResourceTable* dx12SRT = static_cast<DirectX12RayTracingShaderResourceTable*>(srt.get());
		dx12SRT->Generate(mCommandList, dx12Pipeline->GetPipelineProperties());

		//mCommandList->get

		D3D12_DISPATCH_RAYS_DESC dispatchRays{};

		dispatchRays.RayGenerationShaderRecord.StartAddress = dx12SRT->GetRayGenStartAddr();
		dispatchRays.RayGenerationShaderRecord.SizeInBytes = dx12SRT->GetRayGenSectionSize();

		dispatchRays.MissShaderTable.StartAddress = dx12SRT->GetMissStartAddr();
		dispatchRays.MissShaderTable.SizeInBytes = dx12SRT->GetMissSectionSize();
		dispatchRays.MissShaderTable.StrideInBytes = dx12SRT->GetMissEntrySize();


		dispatchRays.HitGroupTable.StartAddress = dx12SRT->GetHitGroupStartAddr();
		dispatchRays.HitGroupTable.SizeInBytes = dx12SRT->GetHitGroupSectionSize();
		dispatchRays.HitGroupTable.StrideInBytes = dx12SRT->GetHitGroupEntrySize();
		dispatchRays.Width = width;
		dispatchRays.Height = height;
		dispatchRays.Depth = 1;

		mCommandList->BackupResource(dx12SRT->GetRayGenSRT());
		mCommandList->BackupResource(dx12SRT->GetMissSRT());
		mCommandList->BackupResource(dx12SRT->GetHitSRT());
		mCommandList->DispatchRays(dispatchRays);
	}
	void DirectX12ComputeContext::Reset()
	{
		mCommandList = nullptr;
		mBindedRootSignature = nullptr;
		mCommandList = DirectX12API::GetComputeCommandList();
	}
	void DirectX12ComputeContext::TransitionBarrier(const List<SharedPtr<ITexture>>& textures, const List<EResourceState>& states)
	{
		if (mCommandList == nullptr)
		{
			return;
		}

		if (textures.size() != states.size())
		{
			JG_LOG_ERROR("Not Match TextureCount And ResourceStateCount");
			return;
		}
		u32 count = textures.size();

		for (u32 i = 0; i < count; ++i)
		{
			DirectX12Texture* dx12Tex = static_cast<DirectX12Texture*>(textures[i].get());
			D3D12_RESOURCE_STATES dx12State = ConvertDX12ResourceState(states[i]);
			mCommandList->TransitionBarrier(dx12Tex->Get(), dx12State);
		}

		mCommandList->FlushResourceBarrier();
	}
	void DirectX12ComputeContext::UAVBarrier(const List<SharedPtr<ITexture>>& textures)
	{
		if (mCommandList == nullptr)
		{
			return;
		}

		u32 count = textures.size();

		for (u32 i = 0; i < count; ++i)
		{
			DirectX12Texture* dx12Tex = static_cast<DirectX12Texture*>(textures[i].get());
			mCommandList->UAVBarrier(dx12Tex->Get());
		}
		mCommandList->FlushResourceBarrier();
	}
	SharedPtr<ICopyContext>	   DirectX12ComputeContext::QueryInterfaceAsCopyContext() const
	{
		SharedPtr<DirectX12CopyContext> copyContext = CreateSharedPtr<DirectX12CopyContext>();
		copyContext->mCommandList = mCommandList;

		return copyContext;
	}





	void DirectX12CopyContext::CopyTextureRegion(SharedPtr<ITexture> dest, SharedPtr<ITexture> src, const JRect& srcRect, EResourceState inDestState, EResourceState inSrcState)
	{
		if (mCommandList == nullptr || dest == nullptr || src == nullptr)
		{
			return;
		}

		DirectX12Texture* destDx12Tex = static_cast<DirectX12Texture*>(dest.get());
		DirectX12Texture* srcDx12Tex = static_cast<DirectX12Texture*>(src.get());
		D3D12_RESOURCE_STATES destState = ConvertDX12ResourceState(inDestState);
		D3D12_RESOURCE_STATES srcState = ConvertDX12ResourceState(inSrcState);

		mCommandList->CopyTextureRegion(destDx12Tex->Get(), srcDx12Tex->Get(), srcRect, destState, srcState);

	}
	void DirectX12CopyContext::CopyBuffer(SharedPtr<IStructuredBuffer> sb, const void* datas, u64 elementSize, u64 elementCount)
	{
		if (mCommandList == nullptr || datas == nullptr)
		{
			return;
		}

		DirectX12StructuredBuffer* dx12SB = static_cast<DirectX12StructuredBuffer*>(sb.get());

		mCommandList->CopyBuffer(dx12SB->Get(), datas, elementSize, elementCount);

	}
}
