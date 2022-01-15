#pragma once
#include "JGCore.h"
#include "Application.h"
#include "Class/Asset/Asset.h"
#include "Graphics/GraphicsDefine.h"
#include "Graphics/FrameBuffer.h"
#include "Platform/Graphics/DirectX12/DirectX12API.h"
#include "Platform/Graphics/DirectX12/Utill/CommandList.h"
#include "Platform/Graphics/DirectX12/Utill/RootSignature.h"
#include "Platform/Graphics/DirectX12/Utill/RayTracingHelper.h"

namespace JG
{
	/*
	RAY_FLAG_NONE                            = 0x00,
    RAY_FLAG_FORCE_OPAQUE                    = 0x01, 광선 추적에서 만나는 모든 광선 기본 교차점은 불투명하게 처리된다.
    RAY_FLAG_FORCE_NON_OPAQUE                = 0x02, 광선 추적에서 만나는 모든 광선 기본 교차점은 불투명하지 않은 것으로 처리된다.
    RAY_FLAG_ACCEPT_FIRST_HIT_AND_END_SEARCH = 0x04, 레이트레이스에서 처음 마주치는 레이프라이머 교집합은 히트 셰이더가 없는 경우를 포함하여 히트 셰이더 직후에 AcceptHitAndEndSearch가 자동으로 호출되도록 한다.
    RAY_FLAG_SKIP_CLOSEST_HIT_SHADER         = 0x08, 적어도 하나의 히트가 커밋되었고 가장 근접한 히트의 히트 그룹이 가장 근접한 히트 셰이더를 포함하더라도 해당 셰이더의 실행을 건너뜁니다.
    RAY_FLAG_CULL_BACK_FACING_TRIANGLES      = 0x10,후향 삼각망을 컬링할 수 있습니다. 인스턴스별로 뒤로 향할 삼각형을 선택하려면 D3D12_RAYTRACHINCE_INSTANCE_FLAGS를 참조하십시오.
    RAY_FLAG_CULL_FRONT_FACING_TRIANGLES     = 0x20,정향 삼각망을 컬링할 수 있습니다. 인스턴스별로 뒤로 향할 삼각형을 선택하려면 D3D12_RAYTRACHINCE_INSTANCE_FLAGS를 참조하십시오.
    RAY_FLAG_CULL_OPAQUE                     = 0x40,지오메트리 및 인스턴스 플래그를 기반으로 불투명하다고 간주되는 모든 원시 요소를 Cull합니다.
    RAY_FLAG_CULL_NON_OPAQUE                 = 0x80,지오메트리 및 인스턴스 플래그를 기반으로 불투명하지 않은 것으로 간주되는 모든 원시 요소를 Cull합니다.
	*/
	/*
	
	1. Build Pipeline State
	// 엔비디아 헬퍼 참고해서 편의용 함수 제작

	2. Bottom Level AS
	// Mesh 모델을 그대로 제작
	// 헬퍼 함수 제작

	3. Top Level AS
	// Scene을 그대로 제작
	// 헬퍼 함수 제작
	4. DispatchRays
	// 함수 제작



	5. 렌더링
		지금 현재 Forwarding 한 거는 그대로 냅두고
		AO, 그림자만 Ray-tracing으로 제작
	
		** 그림자 구현 방법 **
		* RayGen 셰이더
		*	- DepthTexture에서 object depth 추출
		*	- DepthTexture에서 object worldPos 추출
		*	- 현재 물체의 위치에서 라이트 방향으로 Ray 생성
		*	- miss 값이 true이면 검은색, false이면 원래 색
		*	- 책에서는 점점점해서 샘플링해서 축적함
	
	
	
	
	*/
	//class IFrameBuffer;
	class Develop : public Application
	{
		List<SharedPtr<IFrameBuffer>> mFrameBuffers;
		List<SharedPtr<ITexture>>     mRenderTextures;
		List<SharedPtr<ITexture>>     mDepthTextures;


		SharedPtr<IAsset> mXBotMesh = nullptr;
		ID3D12Device5* mD3DDevice   = nullptr;


		struct __declspec(align(256)) ShaderParameter
		{
			char ShaderIdentifier[D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES];
			u64 RootParameters[1];
		};
		// Top Level Acc
		// instance 집합
		// Bottom Level Acc
		// 하나하나 Model Buffer
		struct AccelerationStructureBuffers
		{
			ComPtr<ID3D12Resource> pScratch; // Scratch memory for AS builder
			ComPtr<ID3D12Resource> pResult; // Where the AS is
			ComPtr<ID3D12Resource> pInstanceDesc; // Hold the matrices of the instances
		};
		struct Vertex
		{
			DirectX::XMFLOAT3 position;
			DirectX::XMFLOAT3 color;
		};
		// DescriptorHeap Copy 해서
		// UploadDirect 해서 STB 만들기
		// 


		SharedPtr<RootSignature>			mRaytracingRootSig;
		ComPtr<ID3D12StateObject>			mRaytracingPipelineState;
		ComPtr<ID3D12StateObjectProperties> mRaytracingPipelineStateProperties;
		ComPtr<ID3D12Resource>              mRayGenerationSBT;
		ComPtr<ID3D12Resource>              mHitSBT;
		ComPtr<ID3D12Resource>              mMissSBT;
		ComPtr<ID3D12Resource> mSBTBuffer;
		AccelerationStructureBuffers mTriAS;
		AccelerationStructureBuffers mSceneAS;
		RayTracingShaderBindingTableGenerator mSBTGen;
		SharedPtr<IVertexBuffer> mVertexBuffer;
		SharedPtr<IIndexBuffer>  mIndexBuffer;

	protected:
		virtual void OpenImpl() override;
		virtual void CloseImpl() override;
		virtual void LoadImpl() override;

	private:
		void Update();
		void OnError();
		void Init();
		void Load();
		void CreateBottomLevelAS();
		void CreateTopLevelAS();
		void LoadShader();
	};

	
}