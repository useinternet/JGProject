#pragma once
#include "JGCore.h"
#include "Graphics/RenderParam.h"

namespace JG
{
	class ITopLevelAccelerationStructure;
	class IRayTracingShaderResourceTable;
	class IBottomLevelAccelerationStructure;
	class IRayTracingPipeline;
	class IComputeContext;
	class ISubMesh;
	class IMaterial;
	class IVertexBuffer;
	class IIndexBuffer;
	class ITexture;
	class IRootSignature;
	class Renderer;

	
	

	/// <summary>
	/// Ray Offset
	/// 0 : Radiance Ray
	/// 1 : Shadow   Ray
	/// </summary>
	class RayTracer
	{
	private:
		struct InstanceData
		{
			SharedPtr<ISubMesh>		 SubMesh;
			SharedPtr<IMaterial>	 Material;
			u64		InstanceID = 0;
			u64     HitGroupIndex = 0;
			JMatrix Transform;

			InstanceData(SharedPtr<ISubMesh> subMesh, SharedPtr<IMaterial> m,
				const JMatrix& transform, u64 insID, u64 hitGroupIndex)
				: SubMesh(subMesh), Material(m), Transform(transform), InstanceID(insID), HitGroupIndex(hitGroupIndex) {}
		};
	private:
		List<SharedPtr<ITopLevelAccelerationStructure>> mSceneAS;
		List<InstanceData> mInstances;

		SharedPtr<IRayTracingShaderResourceTable> mSRT;



		Renderer* mRenderer = nullptr;
		JVector2 mResolution;

		List<SharedPtr<ITexture>>  mResults;
		RP_Global_Tex mResultTex;

		u64 mHitGroupOffset = 0;
		u64 mHitGroupStride  = 1;

	/// <summary>
	/// 1. Gen ���� ( ��� ���۵� ���Բ� )
	/// 2. GPU ���ҽ��� CPU������ �����Ҽ��ְڱ� ����
	/// 3. RaytracingContext ���� ���� 
	/// RaytracingContext
	/// BindShaderTable�� ������ �� �ִ� Context
	/// �̰ɷ� ���� ����
	/// </summary>
		List<Color> testColor;
	public:
		RayTracer(Renderer* renderer);
		void AddInstance(SharedPtr<ISubMesh> subMesh, SharedPtr<IMaterial> material, const List<JMatrix>& transform);
		void SetResolution(const JVector2& resolutoin);
		void Execute(SharedPtr<IComputeContext> context);
		void Reset();
	private:
		void Init();
		void InitTextures();
		void UpdateAccelerationStructure(SharedPtr<IComputeContext> context);
		void Update(SharedPtr<IComputeContext> context);
	public:
		static const String& GetDefaultRayTracingPipelineName();
		static u64 GetMaxConstantBufferSize();
		static SharedPtr<IRootSignature> CreateGlobalRootSignature();
		static SharedPtr<IRootSignature> CreateLocalRootSignature();

	};
}