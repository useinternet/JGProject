#include "pch.h"

#include "DirectX12API.h"
#include "DirectX12FrameBuffer.h"
#include "DirectX12Resource.h"
#include "DirectX12Shader.h"
#include "DirectX12Material.h"
#include "DirectX12Mesh.h"
#include "Utill/DirectX12Helper.h"
#include "Utill/DescriptorAllocator.h"
#include "Utill/CommandQueue.h"
#include "Utill/RootSignature.h"
#include "Utill/PipelineState.h"
#include "Utill/CommandList.h"
#include "Utill/ResourceStateTracker.h"
#include "Class/Asset/Asset.h"

namespace JG
{

	static ComPtr<IDXGIFactory4> gFactory;
	static ComPtr<ID3D12Device>  gDevice;
	static UniquePtr<DescriptorAllocator> gCSUAllocator;
	static UniquePtr<DescriptorAllocator> gRTVAllocator;
	static UniquePtr<DescriptorAllocator> gDSVAllocator;

	static UniquePtr<CommandQueue> gGraphicsCommandQueue;
	static UniquePtr<CommandQueue> gComputeCommandQueue;
	static UniquePtr<CommandQueue> gCopyCommandQueue;



	static Dictionary<handle, SharedPtr<DirectX12FrameBuffer>> gFrameBuffers;
	static const u64 gFrameBufferCount = 3;
	static u64       gFrameBufferIndex = 0;

	std::mutex gGraphicsPSOMutex;
	std::mutex gComputePSOMutex;
	std::mutex gRootSigMutex;


	static Dictionary<u64, SharedPtr<GraphicsPipelineState>> gGraphicsPSOs;
	static Dictionary<u64, SharedPtr<ComputePipelineState>>  gComputePSOs;
	static Dictionary<u64, SharedPtr<RootSignature>> gRootSignatures;
	//static SharedPtr<GraphicsPipelineState> gGracphisPSO;
	//static SharedPtr<ComputePipelineState>  gComputePSO;
	//static SharedPtr<RootSignature> gRootSignature;

	static std::mutex gDeviceMutex;

	EGraphicsAPI DirectX12API::GetAPI() const
	{
		return EGraphicsAPI::DirectX12;
	}
	u64 DirectX12API::GetBufferCount() const
	{
		return GetFrameBufferCount();
	}
	u64 DirectX12API::GetBufferIndex() const 
	{
		return GetFrameBufferIndex();
	}
	IDXGIFactory4* DirectX12API::GetDXGIFactory()
	{
		return gFactory.Get();
	}

	ID3D12Device* DirectX12API::GetD3DDevice()
	{
		return gDevice.Get();
	}
	CommandQueue* DirectX12API::GetGraphicsCommandQueue()
	{
		return gGraphicsCommandQueue.get();
	}
	CommandQueue* DirectX12API::GetComputeCommandQueue()
	{
		return gComputeCommandQueue.get();
	}
	CommandQueue* DirectX12API::GetCopyCommandQueue()
	{
		return gCopyCommandQueue.get();
	}
	u64	DirectX12API::GetFrameBufferCount()
	{
		return gFrameBufferCount;
	}
	u64 DirectX12API::GetFrameBufferIndex()
	{
		return gFrameBufferIndex;
	}

	DescriptorAllocation DirectX12API::RTVAllocate()
	{
		return gRTVAllocator->Allocate();
	}
	DescriptorAllocation DirectX12API::DSVAllocate()
	{
		return gDSVAllocator->Allocate();
	}
	DescriptorAllocation DirectX12API::CSUAllocate()
	{
		return gCSUAllocator->Allocate();
	}

	GraphicsCommandList* DirectX12API::GetGraphicsCommandList(u64 ID)
	{
		return static_cast<GraphicsCommandList*>(gGraphicsCommandQueue->RequestCommandList(ID));
	}
	ComputeCommandList* DirectX12API::GetComputeCommandList(u64 ID)
	{
		return static_cast<ComputeCommandList*>(gComputeCommandQueue->RequestCommandList(ID));
	}
	CopyCommandList* DirectX12API::GetCopyCommandList(u64 ID)
	{
		return static_cast<CopyCommandList*>(gCopyCommandQueue->RequestCommandList(ID));
	}

	SharedPtr<GraphicsPipelineState> DirectX12API::GetGraphicsPipelineState(u64 ID)
	{
		std::lock_guard<std::mutex> lock(gGraphicsPSOMutex);
		if (gGraphicsPSOs[ID] == nullptr)
		{
			gGraphicsPSOs[ID] = CreateSharedPtr<GraphicsPipelineState>();
		}
		return gGraphicsPSOs[ID];
	}

	SharedPtr<ComputePipelineState> DirectX12API::GetComputePipelineState(u64 ID)
	{
		std::lock_guard<std::mutex> lock(gComputePSOMutex);
		if (gComputePSOs[ID] == nullptr)
		{
			gComputePSOs[ID] = CreateSharedPtr<ComputePipelineState>();
		}
		return gComputePSOs[ID];
	}

	SharedPtr<RootSignature> DirectX12API::GetRootSignature(u64 ID)
	{
		std::lock_guard<std::mutex> lock(gRootSigMutex);
		if (gRootSignatures[ID] == nullptr)
		{
			gRootSignatures[ID] = CreateSharedPtr<RootSignature>();
		}
		return gRootSignatures[ID];
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
			desc.DestBlendAlpha = D3D12_BLEND_INV_DEST_ALPHA;
			desc.BlendOpAlpha   = D3D12_BLEND_OP_ADD;
			desc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL & ~D3D12_COLOR_WRITE_ENABLE_ALPHA;
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
		Microsoft::WRL::ComPtr<ID3D12Resource> resultResource;
		HRESULT hResult = S_OK;
		{
			std::lock_guard<std::mutex> lock(gDeviceMutex);
			hResult = gDevice->CreateCommittedResource(pHeapProperties, HeapFlags, pDesc, InitialResourceState, pOptimizedClearValue, IID_PPV_ARGS(resultResource.GetAddressOf()));
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
		JG_CORE_INFO("DirectX12 Init Start");

		//
		gFactory = CreateDXGIFactory();
		if (gFactory)
		{
			JG_CORE_INFO("Success Create DXGIFactroy");
		}
		else
		{
			JG_CORE_CRITICAL("Failed Create DXGIFactory");
			return false;
		}
		DXGI_ADAPTER_DESC1 adapterDesc = {};
		gDevice = CreateD3DDevice(gFactory, false, &adapterDesc);

		if (gDevice)
		{
			JG_CORE_INFO("Success Create D3D12Device");
			JG_CORE_TRACE("Description : " + StringExtend::ws2s(adapterDesc.Description));
			JG_CORE_TRACE("VideoMemory : {0}  MB", adapterDesc.DedicatedVideoMemory / 1024 / 1024);
		}
		else
		{
			JG_CORE_CRITICAL("Failed Create D3D12Device");
			return false;
		}

		// NOTE
		// DescriptiorAllcator »ý¼º
		JG_CORE_INFO("Create DescriptorAllocator...");
		gCSUAllocator = CreateUniquePtr<DescriptorAllocator>(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		gRTVAllocator = CreateUniquePtr<DescriptorAllocator>(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		gDSVAllocator = CreateUniquePtr<DescriptorAllocator>(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);


		JG_CORE_INFO("Create CommandQueue...");
		gGraphicsCommandQueue = CreateUniquePtr<CommandQueue>(gFrameBufferCount, D3D12_COMMAND_LIST_TYPE_DIRECT);
		gComputeCommandQueue  = CreateUniquePtr<CommandQueue>(gFrameBufferCount, D3D12_COMMAND_LIST_TYPE_COMPUTE);
		gCopyCommandQueue     = CreateUniquePtr<CommandQueue>(gFrameBufferCount, D3D12_COMMAND_LIST_TYPE_COPY);


		//gGracphisPSO   = CreateSharedPtr<GraphicsPipelineState>();
		//gComputePSO    = CreateSharedPtr<ComputePipelineState>();
		//gRootSignature = CreateSharedPtr<RootSignature>();

		JG_CORE_INFO("DirectX12 Init End");
		return true;
	}


	void DirectX12API::Destroy()
	{
		Flush();

		gGraphicsPSOs.clear();
		gComputePSOs.clear();
		gRootSignatures.clear();
		for (auto& _pair : gFrameBuffers)
		{
			_pair.second->Reset();
		}
		gFrameBuffers.clear();

		gCSUAllocator.reset();
		gRTVAllocator.reset();
		gDSVAllocator.reset();

		gGraphicsCommandQueue.reset();
		gComputeCommandQueue.reset();
		gCopyCommandQueue.reset();


		PipelineState::ClearCache();
		RootSignature::ClearCache();
		ResourceStateTracker::ClearCache();

		gDevice.Reset();
		gFactory.Reset();
	}


	void DirectX12API::Begin()
	{
		gGraphicsCommandQueue->Begin();
		gComputeCommandQueue->Begin();
		gCopyCommandQueue->Begin();
	}
	void DirectX12API::End()
	{
		// TODO
		// FrameBuffer Update
		for (auto& iter : gFrameBuffers)
		{
			iter.second->Update();
		}

		gGraphicsCommandQueue->End();
		gComputeCommandQueue->End();
		gCopyCommandQueue->End();


		// TODO
		// SwapBuffer
		for (auto& iter : gFrameBuffers)
		{
			iter.second->Present();
		}


		gFrameBufferIndex = (gFrameBufferIndex + 1) % gFrameBufferCount;
		gCSUAllocator->UpdatePage();
		gRTVAllocator->UpdatePage();
		gRTVAllocator->UpdatePage();
	}
	void DirectX12API::Flush()
	{
		gGraphicsCommandQueue->Flush();
		gComputeCommandQueue->Flush();
		gCopyCommandQueue->Flush();
	}

	void DirectX12API::SetViewports(u64 commandID, const List<Viewport>& viewPorts)
	{
		auto commandList = GetGraphicsCommandList(commandID);
		commandList->SetViewports(viewPorts);
	}

	void DirectX12API::SetScissorRects(u64 commandID, const List<ScissorRect>& scissorRects)
	{
		auto commandList = GetGraphicsCommandList(commandID);
		commandList->SetScissorRects(scissorRects);
	}

	void DirectX12API::ClearRenderTarget(u64 commandID, const List<SharedPtr<ITexture>>& rtTextures, SharedPtr<ITexture> depthTexture)
	{
		auto commandList = GetGraphicsCommandList(commandID);

		for (auto& texture : rtTextures)
		{
			if (texture == nullptr || texture->IsValid() == false) continue;
			auto handle = static_cast<DirectX12Texture*>(texture.get())->GetRTV();
			if (handle.ptr == 0) continue;


			TextureInfo info = texture->GetTextureInfo();

			commandList->ClearRenderTargetTexture(static_cast<DirectX12Texture*>(texture.get())->Get(), handle, info.ClearColor);
		}
		if (depthTexture && depthTexture->IsValid())
		{
			auto handle = static_cast<DirectX12Texture*>(depthTexture.get())->GetDSV();

			if (handle.ptr != 0)
			{
				TextureInfo info = depthTexture->GetTextureInfo();

				commandList->ClearDepthTexture(static_cast<DirectX12Texture*>(depthTexture.get())->Get(),
					handle, info.ClearDepth, info.ClearStencil);
			}
		}
	}

	void DirectX12API::SetRenderTarget(u64 commandID, const List<SharedPtr<ITexture>>& rtTextures, SharedPtr<ITexture> depthTexture)
	{
		auto commandList = GetGraphicsCommandList(commandID);
		auto pso = GetGraphicsPipelineState(commandID);
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

		commandList->SetRenderTarget(d3dRTResources.data(), rtvHandles.data(), d3dRTResources.size(), d3dDSResource, dsvHandle.get());
	}

	void DirectX12API::DrawIndexed(u64 commandID, u32 indexCount, u32 instancedCount, u32 startIndexLocation, u32 startVertexLocation, u32 startInstanceLocation)
	{
		auto commandList = GetGraphicsCommandList(commandID);
		auto pso = GetGraphicsPipelineState(commandID);
		if (pso->Finalize() == false)
		{
			JG_CORE_ERROR("Failed Create Graphcis PipelineState");
			return;
		}
		commandList->BindPipelineState(pso);
		commandList->DrawIndexed(indexCount, instancedCount, startIndexLocation, startVertexLocation, startIndexLocation);
	}

	void DirectX12API::SetDepthStencilState(u64 commandID, EDepthStencilStateTemplate _template)
	{
		auto pso = GetGraphicsPipelineState(commandID);
		D3D12_DEPTH_STENCIL_DESC desc = {};
		GetDepthStencilDesc(_template, &desc);
		pso->SetDepthStencilState(desc);
	}

	void DirectX12API::SetBlendState(u64 commandID, u32 renderTargetSlot, EBlendStateTemplate _template)
	{
		if (renderTargetSlot >= MAX_RENDERTARGET)
		{
			return;
		}
		auto pso = GetGraphicsPipelineState(commandID);
		auto blendDesc = pso->GetBlendDesc();
		D3D12_RENDER_TARGET_BLEND_DESC desc = {};

		GetBlendDesc(_template, &desc);
		blendDesc.RenderTarget[renderTargetSlot] = desc;
		pso->SetBlendState(blendDesc);
	}

	void DirectX12API::SetRasterizerState(u64 commandID, ERasterizerStateTemplate _template)
	{
		auto pso = GetGraphicsPipelineState(commandID);

		D3D12_RASTERIZER_DESC desc = {};
		GetRasterizerDesc(_template, &desc);

		pso->SetRasterizerState(desc);
	}
	SharedPtr<IFrameBuffer> DirectX12API::CreateFrameBuffer(const FrameBufferInfo& info)
	{
		if (info.Handle == 0) return nullptr;
		auto iter = gFrameBuffers.find(info.Handle);
		if (iter != gFrameBuffers.end())
		{
			return gFrameBuffers[info.Handle];
		}

		auto buffer = CreateSharedPtr<DirectX12FrameBuffer>();
		if (!buffer->Init(info))
		{
			JG_CORE_ERROR("Failed Create DirectX12RenderContext");
			return nullptr;
		}
		gFrameBuffers.emplace(info.Handle, buffer);
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
	SharedPtr<IReadWriteBuffer> DirectX12API::CreateReadWriteBuffer(const String& name, u64 btSize)
	{
		auto rwBuffer = CreateSharedPtr<DirectX12ReadWriteBuffer>();
		rwBuffer->SetName(name);
		rwBuffer->SetData(btSize);

		return rwBuffer;
	}
	SharedPtr<IReadBackBuffer> DirectX12API::CreateReadBackBuffer(const String& name)
	{
		auto rbBuffer = CreateSharedPtr<DirectX12ReadBackBuffer>();
		rbBuffer->SetName(name);
	
		return rbBuffer;
	}
	SharedPtr<IReadBackBuffer> DirectX12API::CreateReadBackBuffer(const String& name, SharedPtr<IReadWriteBuffer> readWriteBuffer)
	{
		auto rbBuffer = CreateSharedPtr<DirectX12ReadBackBuffer>();
		rbBuffer->SetName(name);
		rbBuffer->Read(readWriteBuffer);
		return rbBuffer;
	}
	//SharedPtr<IComputeBuffer> DirectX12API::CreateComputeBuffer(const String& name, u64 btSize)
	//{
	//	auto computeBuffer = CreateSharedPtr<DirectX12ComputeBuffer>();
	//	computeBuffer->SetName(name);
	//	computeBuffer->SetData(btSize);
	//	return computeBuffer;
	//}
	SharedPtr<IComputer> DirectX12API::CreateComputer(const String& name, SharedPtr<IShader> shader)
	{
		if (shader == nullptr)
		{
			return nullptr;
		}
		auto computer = CreateSharedPtr<DirectX12Computer>();
		computer->SetName(name);
		computer->Init(shader);
		return computer;
	}
	SharedPtr<IShader> DirectX12API::CreateShader(const String& name, const String& sourceCode, EShaderFlags flags, const List<SharedPtr<IShaderScript>>& scriptList)
	{
		String errorCode;
		auto shader = CreateSharedPtr<DirectX12Shader>();
		shader->SetName(name);
		if (shader->Compile(sourceCode, scriptList, flags,&errorCode) == false)
		{
			JG_CORE_ERROR("Failed Compile Shader \n Name : {0} Error : {1}  \n SourceCode : \n {2} ", name, errorCode, sourceCode);
			return nullptr;
		}
		return shader;
	}
	SharedPtr<IMaterial> DirectX12API::CreateMaterial(const String& name)
	{
		auto material = CreateSharedPtr<DirectX12Material>();
		material->SetName(name);
		return material;
	}
	SharedPtr<IMaterial> DirectX12API::CreateMaterial(const String& name, SharedPtr<IShader> shader)
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
	SharedPtr<ITexture> DirectX12API::CreateTexture(const TextureAssetStock& stock)
	{
		if (stock.Pixels.empty()) return nullptr;
		auto texture = CreateSharedPtr<DirectX12Texture>();
		texture->CreateFromMemory(stock.Name, (const byte*)stock.Pixels.data(), stock.Width, stock.Height, stock.Channels, stock.PixelPerUnit);
		return texture;
	}


}
