#include "pch.h"
#include "Develop.h"
#include "DevShader.h"
#define SHADER_PATH(x) PathHelper::CombinePath(PathHelper::CombinePath(Application::GetEnginePath(), "Shader/Raytracing"),x)
namespace JG
{
	void Develop::OpenImpl()
	{
		mRaytracingPipeline = CreateSharedPtr<DirectX12RayTracingPipeline>();
		Scheduler::GetInstance().Schedule(1.5f, 0.0f, -1, 0, [&]() -> EScheduleResult
		{
			try {
				Update();
			}
			catch (...) {
				OnError();
			}
		
			return EScheduleResult::Continue;
		});

	}

	void Develop::CloseImpl()
	{
		mRenderTextures.clear();
		mDepthTextures.clear();

	}
	void Develop::LoadImpl()
	{

	}
	void Develop::Update()
	{
		Init();
		Load();

		u32 index = DirectX12API::GetInstance()->GetBufferIndex(); 
		DirectX12Texture* targetTexture      = static_cast<DirectX12Texture*>(mRenderTextures[index].get());
		DirectX12Texture* targetDepthTexture = static_cast<DirectX12Texture*>(mDepthTextures[index].get());


		ComputeCommandList* cmdList = DirectX12API::GetComputeCommandList();
		
		List<ID3D12Resource*>             d3dRTResources;
		List<D3D12_CPU_DESCRIPTOR_HANDLE> rtvHandles;


		ID3D12Resource* d3dDSResource = nullptr;
		UniquePtr<D3D12_CPU_DESCRIPTOR_HANDLE> dsvHandle = nullptr;


		d3dRTResources.push_back(targetTexture->Get());
		rtvHandles.push_back(targetTexture->GetRTV());


		d3dDSResource = targetDepthTexture->Get();
		dsvHandle  = CreateUniquePtr<D3D12_CPU_DESCRIPTOR_HANDLE>();
		*dsvHandle = targetDepthTexture->GetDSV();


		cmdList->BindRootSignature(static_cast<DirectX12RootSignature*>(mGlobalRootSignature.get())->Get());
		cmdList->BindTextures(0, { targetTexture->GetUAV() });
		cmdList->BindStructuredBuffer(1, static_cast<DirectX12TopLevelAccelerationStructure*>(mSceneAS.get())->GetResult()->GetGPUVirtualAddress());
		cmdList->Get()->SetPipelineState1(static_cast<DirectX12RayTracingPipeline*>(mRaytracingPipeline.get())->GetPipelineState());


		D3D12_DISPATCH_RAYS_DESC dispatchRays{};
		dispatchRays.RayGenerationShaderRecord.StartAddress = mRaytracingPipeline->GetRayGenStartAddr();
		dispatchRays.RayGenerationShaderRecord.SizeInBytes = mRaytracingPipeline->GetRayGenSectionSize();

		dispatchRays.MissShaderTable.StartAddress = mRaytracingPipeline->GetMissStartAddr();
		dispatchRays.MissShaderTable.SizeInBytes = mRaytracingPipeline->GetMissSectionSize();
		dispatchRays.MissShaderTable.StrideInBytes = mRaytracingPipeline->GetMissEntrySize();


		dispatchRays.HitGroupTable.StartAddress = mRaytracingPipeline->GetHitGroupStartAddr();
		dispatchRays.HitGroupTable.SizeInBytes = mRaytracingPipeline->GetHitGroupSectionSize();
		dispatchRays.HitGroupTable.StrideInBytes = mRaytracingPipeline->GetHitGroupEntrySize();

		dispatchRays.Width = 1920;
		dispatchRays.Height = 1080;
		dispatchRays.Depth = 1;

		cmdList->DispatchRays(dispatchRays);
		mFrameBuffers[index]->SubmitTexture(mRenderTextures[index]);

	}
	void Develop::OnError()
	{
		JG_CORE_ERROR("Exception");
	}
	void Develop::Init()
	{
		static bool isInit = false;

		if (isInit == true)
		{
			return;
		}
		isInit = true;


		JVector2Uint size = JVector2Uint(1920, 1080);


		u32 buffCnt = DirectX12API::GetInstance()->GetBufferCount();
		mFrameBuffers.resize(buffCnt);
		mRenderTextures.resize(buffCnt);
		mDepthTextures.resize(buffCnt);

		FrameBufferInfo fbInfo;
		fbInfo.Width  = size.x;
		fbInfo.Height = size.y;

		fbInfo.ClearColor = Color::Red();
		fbInfo.Format	  = ETextureFormat::R8G8B8A8_Unorm;
		fbInfo.Handle	  = GetWindow()->GetHandle();
		for (u32 i = 0; i < buffCnt; ++i)
		{
			mFrameBuffers[i] = IFrameBuffer::Create(fbInfo);
		}
		TextureInfo texInfo;
		texInfo.Width      = size.x;
		texInfo.Height     = size.y;
		texInfo.Format     = ETextureFormat::R8G8B8A8_Unorm;
		texInfo.MipLevel   = 1;
		texInfo.Flags      = ETextureFlags::Allow_RenderTarget | ETextureFlags::Allow_UnorderedAccessView;
		texInfo.ArraySize = 1;
		for (u32 i = 0; i < buffCnt; ++i)
		{
			mRenderTextures[i] = ITexture::Create("ss", texInfo);
		}
		texInfo.Flags = ETextureFlags::Allow_DepthStencil;
		texInfo.Format = ETextureFormat::D24_Unorm_S8_Uint;
		for (u32 i = 0; i < buffCnt; ++i)
		{
			mDepthTextures[i] = ITexture::Create("ss", texInfo);
		}

	}
	void Develop::Load()
	{
		static bool isInit = false;

		if (isInit == true)
		{
			return;
		}
		isInit = true;
		mXBotMesh = AssetDataBase::GetInstance().LoadOriginAsset("Asset/Engine/Mesh/XBot.jgasset");
		mVertexBuffer = IVertexBuffer::Create("TestTri", EBufferLoadMethod::CPULoad);
		mIndexBuffer = IIndexBuffer::Create("sss", EBufferLoadMethod::CPULoad);


		Vertex triangleVertices[] = {
			{{0.0f, 1.0F , 0.0f},   {1.0f, 1.0f, 0.0f}},
			{{1.0F, -1.0f , 0.0f},  {0.0f, 1.0f, 1.0f}},
			{{-1.0F, -1.0F , 0.0f}, {1.0f, 0.0f, 1.0f}}
		};
		u32 triangleIndices[] = {
			0,1,2
		};

		mVertexBuffer->SetData(triangleVertices, sizeof(Vertex), 3);
		mIndexBuffer->SetData(triangleIndices, 3);

		LoadShader();
		CreateBottomLevelAS();
		CreateTopLevelAS();
	


		//TopLevel °¡¼Ó
	}

	void Develop::CreateBottomLevelAS()
	{
		ComputeCommandList* cmdList = DirectX12API::GetComputeCommandList();
		SharedPtr<DirectX12ComputeContext> context = CreateSharedPtr<DirectX12ComputeContext>();
		context->mCommandList = cmdList;
		mTriBLAS = CreateSharedPtr<DirectX12BottomLevelAccelerationsStructure>();
		mTriBLAS->Generate(context, mVertexBuffer, mIndexBuffer);

		//auto dx12VBuffer = static_cast<DirectX12VertexBuffer*>(mVertexBuffer.get());
		//auto dx12IBuffer = static_cast<DirectX12IndexBuffer*>(mIndexBuffer.get());

		//u64 scratchSize = 0;
		//u64 resultSize  = 0;

		//BottomLevelASGenerator btASGen;
		//btASGen.AddVertexBuffer(
		//	dx12VBuffer->Get(), 0, 3, sizeof(Vertex),
		//	dx12IBuffer->Get(), 0, 3, nullptr, 0);
		//btASGen.ComputeASBufferSizes(false, &scratchSize, &resultSize);

		//HRESULT hResult = DirectX12API::GetD3DDevice()->CreateCommittedResource(
		//	&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		//	D3D12_HEAP_FLAG_NONE,
		//	&CD3DX12_RESOURCE_DESC::Buffer(scratchSize, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS),
		//	D3D12_RESOURCE_STATE_COMMON,
		//	nullptr,
		//	IID_PPV_ARGS(mTriAS.pScratch.GetAddressOf()));

		//hResult = DirectX12API::GetD3DDevice()->CreateCommittedResource(
		//	&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		//	D3D12_HEAP_FLAG_NONE,
		//	&CD3DX12_RESOURCE_DESC::Buffer(resultSize, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS),
		//	D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE,
		//	nullptr,
		//	IID_PPV_ARGS(mTriAS.pResult.GetAddressOf()));


		//btASGen.Generate(cmdList, mTriAS.pScratch.Get(), mTriAS.pResult.Get());
	}

	void Develop::CreateTopLevelAS()
	{
		ComputeCommandList* cmdList = DirectX12API::GetComputeCommandList();
		SharedPtr<DirectX12ComputeContext> context = CreateSharedPtr<DirectX12ComputeContext>();
		context->mCommandList = cmdList;


		mSceneAS = CreateSharedPtr<DirectX12TopLevelAccelerationStructure>();
		mSceneAS->AddInstance(mTriBLAS, JMatrix::Identity(), 0, 0);
		mSceneAS->Generate(context);

		//u64 scratchSize = 0;
		//u64 resultSize = 0;
		//u64 instanceSize = 0;
		//TopLevelASGenerator topASGen;
		//topASGen.AddInstance(mTriAS.pResult.Get(), JMatrix::Identity(), 0, 0);
		//topASGen.ComputeASBufferSizes(false, &scratchSize, &resultSize, &instanceSize);


		//HRESULT hResult = DirectX12API::GetD3DDevice()->CreateCommittedResource(
		//	&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		//	D3D12_HEAP_FLAG_NONE,
		//	&CD3DX12_RESOURCE_DESC::Buffer(instanceSize + 1000, D3D12_RESOURCE_FLAG_NONE),
		//	D3D12_RESOURCE_STATE_GENERIC_READ,
		//	nullptr,
		//	IID_PPV_ARGS(mSceneAS.pInstanceDesc.GetAddressOf()));


		//hResult = DirectX12API::GetD3DDevice()->CreateCommittedResource(
		//	&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		//	D3D12_HEAP_FLAG_NONE,
		//	&CD3DX12_RESOURCE_DESC::Buffer(scratchSize + 1000, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS),
		//	D3D12_RESOURCE_STATE_COMMON,
		//	nullptr,
		//	IID_PPV_ARGS(mSceneAS.pScratch.GetAddressOf()));

		//hResult = DirectX12API::GetD3DDevice()->CreateCommittedResource(
		//	&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		//	D3D12_HEAP_FLAG_NONE,
		//	&CD3DX12_RESOURCE_DESC::Buffer(resultSize + 1000, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS),
		//	D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE,
		//	nullptr,
		//	IID_PPV_ARGS(mSceneAS.pResult.GetAddressOf()));
		//topASGen.Generate(cmdList, mSceneAS.pScratch.Get(), mSceneAS.pResult.Get(), mSceneAS.pInstanceDesc.Get());
	}


	void Develop::LoadShader()
	{
		// RootSig
		static bool isInit = false;

		if (isInit == true)
		{
			return;
		}
		isInit = true;

		SharedPtr<IRootSignatureCreater> creater = CreateSharedPtr<DirectX12RootSignatureCreater>();
		creater->AddDescriptorTable(0, EDescriptorTableRangeType::UAV, 1024, 0, 0);
		creater->AddSRV(1, 0, 0);
		mGlobalRootSignature = creater->Generate();




		mRaytracingPipeline->AddLibrary(SHADER_PATH("RayGeneration.hlsli"), {"RayGeneration"});
		mRaytracingPipeline->AddLibrary(SHADER_PATH("HitMiss.hlsli"), { "Hit", "Miss" });
		mRaytracingPipeline->AddHitGroup("HitGroup0", "Hit", "", "");
		mRaytracingPipeline->SetGlobalRootSignature(mGlobalRootSignature);
		mRaytracingPipeline->SetMaxPayloadSize(sizeof(float[4]));
		mRaytracingPipeline->SetMaxRecursionDepth(1);
		mRaytracingPipeline->SetMaxAttributeSize(sizeof(float[2]));
		mRaytracingPipeline->AddRayGenerationProgram("RayGeneration");
		mRaytracingPipeline->AddMissProgram("Miss");
		mRaytracingPipeline->AddHitProgram("HitGroup0");
		mRaytracingPipeline->Generate();
	}

}
