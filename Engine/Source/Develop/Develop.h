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
    RAY_FLAG_FORCE_OPAQUE                    = 0x01, ���� �������� ������ ��� ���� �⺻ �������� �������ϰ� ó���ȴ�.
    RAY_FLAG_FORCE_NON_OPAQUE                = 0x02, ���� �������� ������ ��� ���� �⺻ �������� ���������� ���� ������ ó���ȴ�.
    RAY_FLAG_ACCEPT_FIRST_HIT_AND_END_SEARCH = 0x04, ����Ʈ���̽����� ó�� ����ġ�� ���������̸� �������� ��Ʈ ���̴��� ���� ��츦 �����Ͽ� ��Ʈ ���̴� ���Ŀ� AcceptHitAndEndSearch�� �ڵ����� ȣ��ǵ��� �Ѵ�.
    RAY_FLAG_SKIP_CLOSEST_HIT_SHADER         = 0x08, ��� �ϳ��� ��Ʈ�� Ŀ�ԵǾ��� ���� ������ ��Ʈ�� ��Ʈ �׷��� ���� ������ ��Ʈ ���̴��� �����ϴ��� �ش� ���̴��� ������ �ǳʶݴϴ�.
    RAY_FLAG_CULL_BACK_FACING_TRIANGLES      = 0x10,���� �ﰢ���� �ø��� �� �ֽ��ϴ�. �ν��Ͻ����� �ڷ� ���� �ﰢ���� �����Ϸ��� D3D12_RAYTRACHINCE_INSTANCE_FLAGS�� �����Ͻʽÿ�.
    RAY_FLAG_CULL_FRONT_FACING_TRIANGLES     = 0x20,���� �ﰢ���� �ø��� �� �ֽ��ϴ�. �ν��Ͻ����� �ڷ� ���� �ﰢ���� �����Ϸ��� D3D12_RAYTRACHINCE_INSTANCE_FLAGS�� �����Ͻʽÿ�.
    RAY_FLAG_CULL_OPAQUE                     = 0x40,������Ʈ�� �� �ν��Ͻ� �÷��׸� ������� �������ϴٰ� ���ֵǴ� ��� ���� ��Ҹ� Cull�մϴ�.
    RAY_FLAG_CULL_NON_OPAQUE                 = 0x80,������Ʈ�� �� �ν��Ͻ� �÷��׸� ������� ���������� ���� ������ ���ֵǴ� ��� ���� ��Ҹ� Cull�մϴ�.
	*/
	/*
	
	1. Build Pipeline State
	// ������ ���� �����ؼ� ���ǿ� �Լ� ����

	2. Bottom Level AS
	// Mesh ���� �״�� ����
	// ���� �Լ� ����

	3. Top Level AS
	// Scene�� �״�� ����
	// ���� �Լ� ����
	4. DispatchRays
	// �Լ� ����



	5. ������
		���� ���� Forwarding �� �Ŵ� �״�� ���ΰ�
		AO, �׸��ڸ� Ray-tracing���� ����
	
		** �׸��� ���� ��� **
		* RayGen ���̴�
		*	- DepthTexture���� object depth ����
		*	- DepthTexture���� object worldPos ����
		*	- ���� ��ü�� ��ġ���� ����Ʈ �������� Ray ����
		*	- miss ���� true�̸� ������, false�̸� ���� ��
		*	- å������ �������ؼ� ���ø��ؼ� ������
	
	
	
	
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
		// instance ����
		// Bottom Level Acc
		// �ϳ��ϳ� Model Buffer
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
		// DescriptorHeap Copy �ؼ�
		// UploadDirect �ؼ� STB �����
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