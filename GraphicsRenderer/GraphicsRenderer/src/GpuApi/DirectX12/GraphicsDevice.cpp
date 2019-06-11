#include"pch.h"
#include"GraphicsDevice.h"
#include"CommandExecutor.h"
#include"DescriptorHeap.h"
#include"Dx12Helper.h"
#include"Fence.h"
#include"Renderer.h"
#include"Commander.h"
#include"VertexCollection.h"
#include"Mesh.h"
#include"WICTextureLoader12.h"
#include"DDSTextureLoader12.h"
#include"Image.h"
#include"PSOCache.h"
#include"RootSignatureCache.h"
#include"GPUAllocator.h"
using namespace std;
namespace GR
{
	namespace Dx12
	{
		uint64_t GraphicsDevice::ms_EngineFrame = 0;
		GraphicsDevice* GraphicsDevice::ms_App = nullptr;

		GraphicsDevice::GraphicsDevice()
		{
			m_FrameIndex = 0;
	
			for (int i = 0; i < ms_FrameCount; ++i)
			{
				m_FenceValue[i] = 0;
				m_FrameValue[i] = 0;
			}
			for (int i = 0; i < NUM_LOADER; ++i)
			{
				m_LoadCommander[i] = nullptr;
			}
			m_EngineShaderEntitys[Shader::VS] = "VS";
			m_EngineShaderEntitys[Shader::HS] = "HS";
			m_EngineShaderEntitys[Shader::DS] = "DS";
			m_EngineShaderEntitys[Shader::GS] = "GS";
			m_EngineShaderEntitys[Shader::PS] = "PS";
			m_EngineShaderEntitys[Shader::CS] = "CS";
			m_EngineShaderVersions[Shader::VS] = "vs_5_1";
			m_EngineShaderVersions[Shader::HS] = "hs_5_1";
			m_EngineShaderVersions[Shader::DS] = "ds_5_1";
			m_EngineShaderVersions[Shader::GS] = "gs_5_1";
			m_EngineShaderVersions[Shader::PS] = "ps_5_1";
			m_EngineShaderVersions[Shader::CS] = "cs_5_1";
			ms_App = this;

			// ui 관련 초기화

		}
		GraphicsDevice::~GraphicsDevice()
		{
			m_DestroyUIEvent();
			Flush();
		}

		void GraphicsDevice::Initialize(HWND hWnd, uint32_t width, uint32_t height, bool isUseWrap, UIEvents* e)
		{
			// 변수 저장
			{
				m_Width = width;
				m_Height = height;
				m_hWnd = hWnd;
			}
			// 중요 Dx 장치 및 클래스 초기화
			{
				m_Factory = GR::Dx12::CreateDXGIFactory();
				m_Device = CreateD3DDevice(m_Factory, isUseWrap);

				m_CommandExecutorManager = make_unique<CommandExecutorManager>();
				m_CommandExecutorManager->Initialize(m_Device);

				m_SwapChain = CreateDXGISwapChain(hWnd, m_Factory,
					m_CommandExecutorManager->GetGraphicsCommandExecutor()->GetD3DCommandQueue(),
					ms_BackBufferFormat, m_Width, m_Height, ms_FrameCount);

				m_PSOCache     = make_unique<PSOCache>();
				m_RootSigCache = make_unique<RootSignatureCache>();
			}
		

			// 중요 변수 설정 및 힙 할당자 생성
			{
				m_FrameIndex = m_SwapChain->GetCurrentBackBufferIndex();

				for (int i = 0; i < DH_NUM; ++i)
				{
					D3D12_DESCRIPTOR_HEAP_TYPE type;
					switch (i)
					{
					case SAMPLER:
						type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
						m_Increment_Sampler_DescriptorSize = m_Device->GetDescriptorHandleIncrementSize(type);
					case SRV_UAV_CBV:
						type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
						m_Increment_SrvUavCbv_DescriptorSize = m_Device->GetDescriptorHandleIncrementSize(type);
						break;
					case RTV:
						type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
						m_Increment_Rtv_DescriptorSize = m_Device->GetDescriptorHandleIncrementSize(type);
						break;
					case DSV:
						type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
						m_Increment_Dsv_DescriptorSize = m_Device->GetDescriptorHandleIncrementSize(type);
						break;
					}
					m_DescriptorAllocator[i] = make_shared<DescriptorAllocator>(m_Device.Get(), type);
				}
			}
	
			// 백버퍼 생성
			UpdateRenderTarget();
			
			if (e)
			{
				m_InitUIEvent = e->initEvent;
				m_DestroyUIEvent = e->destroyEvent;
				m_NewFrameUIEvent = e->newFrameEvent;
				m_DrawUIEvent = e->renderEvent;
			}
			else
			{
				m_NewFrameUIEvent = []() {};
				m_DrawUIEvent = [](GraphicsCommander* c) {};
				m_InitUIEvent = [](HWND, GraphicsDevice*) {};
				m_DestroyUIEvent = []() {};
			}

			
			// ui 관련 초기화
			m_UIGpuAllocator = make_shared<GPUAllocator>();
			m_UIGpuAllocator->Initialize(m_Device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1024 * 64);
			m_InitUIEvent(hWnd, this);
		}
		void GraphicsDevice::Resize(uint32_t width, uint32_t height)
		{
			//assert(false && "resize not imp");
			//
			if (m_Width == width && m_Height == height)
			{
				return;
			}

			Flush();

			for (int i = 0; i < ms_FrameCount; ++i)
			{
				m_BackBufferResource[i].m_Resource.Reset();
				m_BackBufferResource[i].m_UsageState = D3D12_RESOURCE_STATE_COMMON;
				m_BackBufferResource[i].m_RTVs.clear();
			}
			DXGI_SWAP_CHAIN_DESC desc;
			m_SwapChain->GetDesc(&desc);
			m_SwapChain->ResizeBuffers(ms_FrameCount, width, height, ms_BackBufferFormat,
				desc.Flags);
			UpdateRenderTarget();

			m_Width = width;
			m_Height = height;
		}
		void GraphicsDevice::NewFrame()
		{
			ms_EngineFrame++;


			for (int i = 0; i < DH_NUM; ++i)
			{
				m_DescriptorAllocator[i]->ReleaseStaleDescriptors(m_FrameValue[m_FrameIndex]);
			}
			m_NewFrameUIEvent();
		}
		void GraphicsDevice::Present(ColorTexture* texture)
		{
			Flush(GRAPHICS);

			auto commander = GetGraphicsCommander();
			m_FrameIndex = m_SwapChain->GetCurrentBackBufferIndex();
			auto& backBuffer = m_BackBufferResource[m_FrameIndex];



			// 백버퍼 클리어
			{

				commander->m_D3D_CommandList->ResourceBarrier(
					1, &CD3DX12_RESOURCE_BARRIER::Transition(backBuffer.GetResource(), backBuffer.m_UsageState,
						D3D12_RESOURCE_STATE_RENDER_TARGET));
				backBuffer.m_UsageState = D3D12_RESOURCE_STATE_RENDER_TARGET;

				auto color = backBuffer.GetClearColor();
				float clearValue[4];
				clearValue[0] = color.x;
				clearValue[1] = color.y;
				clearValue[2] = color.z;
				clearValue[3] = color.w;
				commander->m_D3D_CommandList->ClearRenderTargetView(backBuffer.GetRTV(), clearValue, 0, nullptr);
			}
			
			



			// 텍스쳐 복사
			if (texture)
			{
				D3D12_RESOURCE_BARRIER barrier[] = {
					CD3DX12_RESOURCE_BARRIER::Transition(backBuffer.GetResource(), backBuffer.m_UsageState,
						D3D12_RESOURCE_STATE_COPY_DEST),
					CD3DX12_RESOURCE_BARRIER::Transition(texture->GetResource(), backBuffer.m_UsageState,
						D3D12_RESOURCE_STATE_COPY_SOURCE)
				};

				commander->m_D3D_CommandList->ResourceBarrier(
					2, barrier);

				commander->m_D3D_CommandList->CopyResource(backBuffer.GetResource(), texture->GetResource());



				backBuffer.m_UsageState = D3D12_RESOURCE_STATE_COPY_DEST;
				texture->m_UsageState  = D3D12_RESOURCE_STATE_COPY_SOURCE;
				commander->BackUpObject(texture->GetResource());


				commander->m_D3D_CommandList->ResourceBarrier(
				1, &CD3DX12_RESOURCE_BARRIER::Transition(backBuffer.GetResource(), backBuffer.m_UsageState,
					D3D12_RESOURCE_STATE_RENDER_TARGET));
			backBuffer.m_UsageState = D3D12_RESOURCE_STATE_RENDER_TARGET;
			}

			// UI 에 그릴 렌더타겟 설정
			{
				commander->m_D3D_CommandList->OMSetRenderTargets(1, &backBuffer.GetRTV(), false, nullptr);
				commander->m_D3D_CommandList->SetDescriptorHeaps(1, m_UIGpuAllocator->GetD3DDescriptorHeap().GetAddressOf());

			}
			
			// UI 그리기
			m_DrawUIEvent(commander);


			// 푸시 커맨드
			commander->m_D3D_CommandList->ResourceBarrier(
				1, &CD3DX12_RESOURCE_BARRIER::Transition(backBuffer.GetResource(), backBuffer.m_UsageState,
					D3D12_RESOURCE_STATE_PRESENT));
			backBuffer.m_UsageState = D3D12_RESOURCE_STATE_PRESENT;

			PushCommander(commander);


			ExecuteCommanderWithFenceValue(GRAPHICS, &m_FenceValue[m_FrameIndex]);
			m_FrameValue[m_FrameIndex] = ms_EngineFrame;


			m_SwapChain->Present(ms_UseVsync, 0);
		}
		void GraphicsDevice::Flush()
		{
			m_CommandExecutorManager->GetGraphicsCommandExecutor()->Flush();
			m_CommandExecutorManager->GetComputeCommandExecutor()->Flush();
			m_CommandExecutorManager->GetCopyCommandExecutor()->Flush();
		}
		void GraphicsDevice::Flush(ECmdQueueType type)
		{
			m_CommandExecutorManager->GetCommandExecutor(type)->Flush();
		}
		DescriptorAllocation GraphicsDevice::Allocate(EDescritporHeapType type, uint32_t numDescriptor)
		{
			auto alloc = m_DescriptorAllocator[type]->Allocate(numDescriptor);
			return alloc;
		}
		void GraphicsDevice::ExecuteCommander(ECmdQueueType type, bool is_flush)
		{
			if (type == GRAPHICS)
			{
				for (int i = 0; i < NUM_LOADER; ++i)
				{
					if (m_LoadCommander[i])
					{
						PushCommander(m_LoadCommander[i]);
						m_LoadCommander[i] = nullptr;

					}
				}
			}
			auto que = m_CommandExecutorManager->GetCommandExecutor(type);
			if (!m_WaitingCommanders[type].empty())
			{
				auto size = m_WaitingCommanders[type].size();
				auto data = m_WaitingCommanders[type].data();
				que->ExcuteCommandLists((uint32_t)size, data);
				m_WaitingCommanders[type].clear();
				if (is_flush)
				{
					que->Flush();
				}
			}
		}
		void GraphicsDevice::ExecuteCommander(bool is_flush)
		{
			for (int i = 0; i < NUM_LOADER; ++i)
			{
				if (m_LoadCommander[i])
				{
					PushCommander(m_LoadCommander[i]);
					m_LoadCommander[i] = nullptr;

				}
			}
			for (int i = 0; i < CQ_NUM; ++i)
			{
				auto que = m_CommandExecutorManager->GetCommandExecutor(i);
				if (!m_WaitingCommanders[i].empty())
				{
					auto size = m_WaitingCommanders[i].size();
					auto data = m_WaitingCommanders[i].data();
					que->ExcuteCommandLists((uint32_t)size, data);
					m_WaitingCommanders[i].clear();
				}
			}
			if (is_flush)
			{
				Flush();
			}
		}
		void GraphicsDevice::ExecuteCommanderWithFenceValue(ECmdQueueType type, uint64_t* outValue)
		{
			auto que = m_CommandExecutorManager->GetCommandExecutor(type);
			if (!m_WaitingCommanders[type].empty())
			{
				auto size = m_WaitingCommanders[type].size();
				auto data = m_WaitingCommanders[type].data();
				que->ExcuteCommandLists((uint32_t)size, data);
				m_WaitingCommanders[type].clear();
			}
			*outValue = que->Signal();
			que->WaitForFenceValue(*outValue);
		}
		void GraphicsDevice::PushCommander(GraphicsCommander* commander)
		{
			std::lock_guard<mutex> lock(m_PushCommanderMutex);
			commander->Close();
			m_WaitingCommanders[GRAPHICS].push_back(commander);
		}
		void GraphicsDevice::PushCommander(ComputeCommander* commander)
		{
			std::lock_guard<mutex> lock(m_PushCommanderMutex);
			commander->Close();
			m_WaitingCommanders[COMPUTE].push_back(commander);
		}
		void GraphicsDevice::PushCommander(CopyCommander* commander)
		{
			std::lock_guard<mutex> lock(m_PushCommanderMutex);
			commander->Close();
			m_WaitingCommanders[COPY].push_back(commander);
		}
		GraphicsCommander* GraphicsDevice::GetGraphicsCommander()
		{
			return static_cast<GraphicsCommander*>(m_CommandExecutorManager->GetGraphicsCommandExecutor()->GetCommander());
		}
		ComputeCommander*  GraphicsDevice::GetComputeCommander()
		{
			return static_cast<ComputeCommander*>(m_CommandExecutorManager->GetComputeCommandExecutor()->GetCommander());
		}
		CopyCommander*     GraphicsDevice::GetCopyCommander()
		{
			return static_cast<CopyCommander*>(m_CommandExecutorManager->GetCopyCommandExecutor()->GetCommander());
		}
		GraphicsCommander* GraphicsDevice::GetLoadCommander(ELoadCommanderType type)
		{
			if (m_LoadCommander[type] == nullptr)
			{
				m_LoadCommander[type] = GetGraphicsCommander();
			}
			return m_LoadCommander[type];
		}
		Renderer*  GraphicsDevice::GetRenderer()
		{
			if (!m_GraphicsRenderer)
			{
				m_GraphicsRenderer = make_unique<Renderer>();
				m_GraphicsRenderer->m_GraphcisDevice = this;
			}
			return m_GraphicsRenderer.get();
		}
		GraphicsPSO GraphicsDevice::GetGraphicsPSOFromCache(RootSignature& rootSig, uint32_t enumPso, uint32_t macrooption)
		{
			return m_PSOCache->GetGraphicsPSO(rootSig, (PSOCache::EGraphicsPSO)enumPso, (PSOCache::EGraphicsMacroOption)macrooption);
		}
		ComputePSO  GraphicsDevice::GetComputePSOFromCache(RootSignature& rootSig, uint32_t enumPso, uint32_t macrooption)
		{
			return m_PSOCache->GetComputePSO(rootSig, (PSOCache::EComputePSO)enumPso, (PSOCache::EComputeMacroOption)macrooption);
		}
		RootSignature GraphicsDevice::GetRootSignatureFromCache(ERootSignature enumRootSig)
		{
			return m_RootSigCache->GetRootSignature(enumRootSig);
		}
		GPUAllocation GraphicsDevice::UIGPUAllcoate()
		{
			return m_UIGpuAllocator->Allocate();
		}
		GPUAllocation GraphicsDevice::UIGPUAllocateAndRegister(Texture* in_texture)
		{
			auto gpu = m_UIGpuAllocator->Allocate();
			m_Device->CreateShaderResourceView(in_texture->GetResource(), in_texture->m_CurrentSrvDesc.get(),
				gpu.CPU());
			return gpu;
		}
		void GraphicsDevice::SetShaderDirPath(const std::wstring& path)
		{
			m_PSOCache->BindShaderDirPath(path);
		}
		GPUResource GraphicsDevice::CreateGPUResource(
			const D3D12_RESOURCE_DESC& desc, D3D12_CLEAR_VALUE* clearValue,
			EHeapType type, D3D12_RESOURCE_STATES state, D3D12_HEAP_FLAGS flag)
		{
			GPUResource resource;

			
			D3D12_HEAP_TYPE d3dheaptype;
			switch (type)
			{
			case DEFAULT:
				d3dheaptype = D3D12_HEAP_TYPE_DEFAULT;
				break;
			case UPLOAD:
				d3dheaptype = D3D12_HEAP_TYPE_UPLOAD;
				break;
			case READBACK:
				d3dheaptype = D3D12_HEAP_TYPE_READBACK;
				break;
			case CUSTOM:
				d3dheaptype = D3D12_HEAP_TYPE_CUSTOM;
				break;
			}
			m_Device->CreateCommittedResource(
				&CD3DX12_HEAP_PROPERTIES(d3dheaptype),
				flag,
				&desc,
				state,
				clearValue,
				IID_PPV_ARGS(resource.m_Resource.GetAddressOf()));
			resource.m_UsageState = state;
			return resource;
		}
		GPUResource GraphicsDevice::CreateRenderTargetGPUResource(
			DXGI_FORMAT format,
			uint32_t width, uint32_t height,
			uint32_t mipmaps,
			uint32_t arraySize,
			const float4& clearColor)
		{
			D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Tex2D(format, width, height, arraySize, mipmaps,
				1, 0, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);
			D3D12_CLEAR_VALUE value;
			value.Format = format;
			value.Color[0] = clearColor.x; 	value.Color[1] = clearColor.y;
			value.Color[2] = clearColor.z; 	value.Color[3] = clearColor.w;

			return CreateGPUResource(desc, &value);
		}
		GPUResource GraphicsDevice::CreateDepthStencilGPUResource(
			DXGI_FORMAT format,
			uint32_t width, uint32_t height,
			uint32_t mipmaps,
			uint32_t arraySize,
			float cleardepth, uint8_t clearstencil)
		{
			D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Tex2D(format, width, height, arraySize, mipmaps,
				1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
			D3D12_CLEAR_VALUE value;
			value.Format = format;
			value.DepthStencil.Depth = cleardepth;
			value.DepthStencil.Stencil = clearstencil;

			return CreateGPUResource(desc, &value);
		}

		GPUResource GraphicsDevice::CreateDefaultGPUResource(
			DXGI_FORMAT format,
			uint32_t width, uint32_t height,
			uint32_t mipmaps,
			uint32_t arraySize)
		{
			D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Tex2D(format, width, height, arraySize, mipmaps);
			return CreateGPUResource(desc);
		}

		GPUResource GraphicsDevice::CreateUploadGPUResource(uint64_t bufferSize)
		{
			return CreateGPUResource(
				CD3DX12_RESOURCE_DESC::Buffer(bufferSize),
				nullptr,
				GraphicsDevice::UPLOAD,
				D3D12_RESOURCE_STATE_GENERIC_READ);

		}
		Texture GraphicsDevice::CreateTexture(const GPUResource& gpuresource)
		{
			Texture texture;
			texture.m_Resource = gpuresource.m_Resource;
			texture.m_UsageState = gpuresource.m_UsageState;
			texture.m_TransitioningState = gpuresource.m_TransitioningState;

			return texture;
		}
		ColorTexture GraphicsDevice::CreateColorTexture(const GPUResource& gpuresource, float4 clearColor)
		{
			ColorTexture colortexture;
			colortexture.m_Resource = gpuresource.m_Resource;
			colortexture.m_UsageState = gpuresource.m_UsageState;
			colortexture.m_TransitioningState = gpuresource.m_TransitioningState;
			colortexture.m_ClearColor = clearColor;
			return colortexture;
		}
		DepthTexture GraphicsDevice::CreateDepthTexture(const GPUResource& gpuresource, float cleardepth , uint8_t clearstencil )
		{
			DepthTexture depthtexture;
			depthtexture.m_Resource = gpuresource.m_Resource;
			depthtexture.m_UsageState = gpuresource.m_UsageState;
			depthtexture.m_TransitioningState = gpuresource.m_TransitioningState;
			depthtexture.m_ClearDepth = cleardepth;
			depthtexture.m_ClearStencil = clearstencil;
			return depthtexture;
		}

		void GraphicsDevice::CreateShader(const std::wstring& filename, Shader* shader, const std::vector<D3D_SHADER_MACRO>& macro)
		{
			assert(shader != nullptr);

			auto stage = shader->GetStage();
			uint32_t hash_code = 0;
			if (macro[0].Definition != nullptr && macro[0].Name != nullptr)
			{
				hash_code = Hash::D3D_Macro_Hash(macro);
			}
			auto find_iter = m_ShaderCache[stage].find(hash_code);
			
			// 처음 컴파일한 셰이더였을때 데이터 생성
			if (find_iter == m_ShaderCache[stage].end())
			{
				shader->m_Blob = CompileShader(
					filename, macro.data(),
					m_EngineShaderEntitys[stage],
					m_EngineShaderVersions[stage], &shader->m_ErrorString);
			}
			// 이미 존재하는경우 데이터 전달
			else
			{
				shader->m_Blob = m_ShaderCache[stage][hash_code];
			}
		}
		bool GraphicsDevice::CreateMeshDataFromFile(const std::string& filename, Mesh* mesh)
		{
			if (m_LoadCommander[MESH_LOADER] == nullptr)
				m_LoadCommander[MESH_LOADER] = GetGraphicsCommander();


			fs::path file_path(filename);
			auto file_format = file_path.extension();
			if (file_format != ".mesh")
				return false;

			ifstream fin;
			fin.open(filename, ios::binary);

			if (!fin.is_open())
				return false;

			// 메시 데이터 갯수
			uint32_t meshCount = 0;
			fin.read(
				reinterpret_cast<char*>(&meshCount),
				sizeof(uint32_t));

			std::vector<Vertex>  Vertices;
			std::vector<uint32_t> Indices;
			std::vector<std::string> MeshNames(meshCount);
			std::vector<uint32_t> VStarts(meshCount);
			std::vector<uint32_t> VEnds(meshCount);
			std::vector<uint32_t> IStarts(meshCount);
			std::vector<uint32_t> IEnds(meshCount);
			uint32_t VOffset = 0;
			uint32_t IOffset = 0;


			// 스켈레톤 메시 인가 아닌가.
			bool is_skeletal = false;
			fin.read(
				reinterpret_cast<char*>(&is_skeletal),
				sizeof(bool));

			for (uint32_t i = 0; i < meshCount; ++i)
			{
				// 메시 이름 길이
				uint32_t name_len = 0;
				fin.read(
					reinterpret_cast<char*>(&name_len),
					sizeof(uint32_t));

				// 메시 이름
				char* mesh_name_Cstr = new char[name_len];
				fin.read(
					reinterpret_cast<char*>(mesh_name_Cstr),
					sizeof(char) * name_len);
				std::string meshname = mesh_name_Cstr;
				MeshNames[i] = meshname;
				delete mesh_name_Cstr;


				// Vertex & Index 갯수
				uint32_t vertexCount = 0;
				uint32_t indexCount = 0;
				fin.read(
					reinterpret_cast<char*>(&vertexCount),
					sizeof(uint32_t));
				fin.read(
					reinterpret_cast<char*>(&indexCount),
					sizeof(uint32_t));

				if (is_skeletal)
				{
					assert(false && "current is not supported skeletalmesh");
				}
				else // 스태틱 메시 일경우
				{
					std::vector<Vertex> vertices;
					std::vector<uint32_t> indices;

					vertices.resize(vertexCount);
					indices.resize(indexCount);

					fin.read(
						reinterpret_cast<char*>(vertices.data()),
						sizeof(Vertex) * vertexCount);
					fin.read(
						reinterpret_cast<char*>(indices.data()),
						sizeof(uint32_t) * indexCount);

					VStarts[i] = VOffset;
					IStarts[i] = IOffset;
					VEnds[i] = VOffset + vertexCount;
					IEnds[i] = IOffset + indexCount;


					VOffset += vertexCount;
					IOffset += indexCount;

					Vertices.insert(Vertices.end(), vertices.begin(), vertices.end());
					Indices.insert(Indices.end(), indices.begin(), indices.end());
				}
			}

			mesh->CreateMesh(m_LoadCommander[MESH_LOADER], Vertices, Indices);

			for (uint32_t i = 0; i < meshCount; ++i)
			{
				mesh->Partion(
					MeshNames[i], VStarts[i], VEnds[i], IStarts[i], IEnds[i]);
			}

			fin.close();

			return true;
		}
		bool GraphicsDevice::CreateTextureFromFile(const std::string& filename, Texture* texture, bool is_cubmap, bool is_generate)
		{
			if (texture == nullptr)
				return false;
			if (m_LoadCommander[TEXTURE_LOADER] == nullptr)
				m_LoadCommander[TEXTURE_LOADER] = GetGraphicsCommander();


			fs::path ph(filename);

			if (!fs::exists(ph))
			{
				return false;
			}

			auto file_format = ph.extension();

			GPUResource resource;
			std::unique_ptr<uint8_t[]> imageData;
			std::vector<D3D12_SUBRESOURCE_DATA> subresources;
			std::shared_ptr<Image> image;
			if (file_format == ".dds")
			{
				HRESULT hr = DirectX::LoadDDSTextureFromFile(m_Device.Get(), ph.wstring().c_str(),
					resource.m_Resource.GetAddressOf(), imageData, subresources, 0, nullptr, &is_cubmap, false);
				if (FAILED(hr))
				{
					return false;
				}
				resource.m_UsageState = D3D12_RESOURCE_STATE_COPY_DEST;
			}
			else if (file_format == ".hdr")
			{
				subresources.push_back({});
				image = Image::FromFile(filename);

				resource = CreateDefaultGPUResource(DXGI_FORMAT_R32G32B32A32_FLOAT, image->Width(),
					image->Height());

				subresources[0].pData = image->Pixels<void>();
				subresources[0].RowPitch = image->Pitch();
				subresources[0].SlicePitch = image->DataSize();
			}
			else
			{
				subresources.push_back({});
				HRESULT hr = DirectX::LoadWICTextureFromFile(m_Device.Get(), ph.wstring().c_str(),
					resource.m_Resource.GetAddressOf(), imageData, subresources[0]);

				if (FAILED(hr))
				{
					return false;
				}
				resource.m_UsageState = D3D12_RESOURCE_STATE_COPY_DEST;
			}
			// 텍스쳐 데이터 복사
			{
				UINT64 RequiredSize = 0;
				m_Device->GetCopyableFootprints(&resource->GetDesc(), 0, 
					(UINT)subresources.size(), 0, nullptr, nullptr, nullptr, &RequiredSize);

				GPUResource upload = CreateUploadGPUResource(RequiredSize);

				
				m_LoadCommander[TEXTURE_LOADER]->TransitionBarrier(resource, D3D12_RESOURCE_STATE_COPY_DEST, true);

				UpdateSubresources(m_LoadCommander[TEXTURE_LOADER]->GetD3DCommandList(), resource.GetResource(),
					upload.GetResource(), 0, 0, (uint32_t)subresources.size(), subresources.data());
				m_LoadCommander[TEXTURE_LOADER]->BackUpObject(upload);

				m_LoadCommander[TEXTURE_LOADER]->TransitionBarrier(resource, D3D12_RESOURCE_STATE_GENERIC_READ, true);
			}


			texture->m_Resource = resource.m_Resource;
			texture->m_UsageState = resource.m_UsageState;



			if (is_cubmap && file_format == ".hdr")
			{

				GPUResource uavResource = CreateGPUResource(CD3DX12_RESOURCE_DESC::Tex2D(
					DXGI_FORMAT_R16G16B16A16_FLOAT, 1024, 1024, 6, 0, 1, 0,
					D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS),
					nullptr,
					GraphicsDevice::DEFAULT, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

				Texture uavTexture = CreateTexture(uavResource);

				D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
				uavDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
				uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
				uavDesc.Texture2DArray.ArraySize = 6;
				uavDesc.Texture2DArray.FirstArraySlice = 0;
				uavDesc.Texture2DArray.MipSlice = 0;
				uavDesc.Texture2DArray.PlaneSlice = 0;
				uavTexture.SetUAVDesc(&uavDesc);

				ComputeCommander* computeCommander = m_LoadCommander[TEXTURE_LOADER]->ConvertComputeCommander();
				auto rootSig = GetRootSignatureFromCache(ERootSignature::C_InputOutput);
				auto pso = GetComputePSOFromCache(rootSig, PSOCache::EQUIRECT2CUBE, PSOCache::USE_NULL);

				computeCommander->SetRootSignature(rootSig);
				computeCommander->SetPipelineState(pso);
				computeCommander->SetSRVDescriptorTable(C_InputOutput_RootParam_InputTexture, 1, &texture->GetSRV());
				computeCommander->SetUAVDescriptorTable(C_InputOutput_RootParam_OutputTexture, 1, &uavTexture.GetUAV());
				computeCommander->Dispatch(texture->GetResource()->GetDesc().Width / 32, texture->GetResource()->GetDesc().Height / 32, 6);
				computeCommander->UAVBarrier(uavTexture, true);

				texture->m_Resource = uavTexture.m_Resource;
				texture->m_UsageState = uavTexture.m_UsageState;
			}

			if (is_generate)
			{
				ComputeCommander* computeCommander = m_LoadCommander[TEXTURE_LOADER]->ConvertComputeCommander();
				computeCommander->GenerateMipMaps(*texture);
			}

			return true;
		}
		uint32_t GraphicsDevice::GetDescriptorIncrementSize(GraphicsDevice::EDescritporHeapType type) const
		{
			switch (type)
			{
			case SRV_UAV_CBV:
				return m_Increment_SrvUavCbv_DescriptorSize;
			case RTV:
				return m_Increment_Rtv_DescriptorSize;
			case DSV:
				return m_Increment_Dsv_DescriptorSize;
			}
			return 0;
		}


		void GraphicsDevice::UpdateRenderTarget()
		{
			for (int i = 0; i < ms_FrameCount; ++i)
			{
				ComPtr<ID3D12Resource> d3dresource;
				m_SwapChain->GetBuffer(i, IID_PPV_ARGS(d3dresource.GetAddressOf()));

				m_BackBufferResource[i].m_Resource = move(d3dresource);
				m_BackBufferResource[i]->SetName(L"BackBufferResource");
			}
			 
		}
	}
}



