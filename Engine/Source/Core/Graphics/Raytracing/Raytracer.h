#pragma once
#include "JGCore.h"
#include "Graphics/RenderParam.h"

namespace JG
{
	namespace Graphics
	{
		class RenderPassData;
	}
	class ITopLevelAccelerationStructure;
	class IRayTracingShaderResourceTable;
	class IBottomLevelAccelerationStructure;
	class CalculatePartialDerivatives;
	class RT_Composite;
	class IRayTracingPipeline;
	class IComputeContext;
	class ISubMesh;
	class IMaterial;
	class IVertexBuffer;
	class IIndexBuffer;
	class ITexture;
	class IRootSignature;
	class Renderer;
	class RTAO;
	class Denoiser;
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
		struct CB
		{
			JMatrix ProjMatrix;
			JMatrix ViewMatrix;
			JMatrix ViewProjMatrix;
			JMatrix InvViewMatrix;
			JMatrix InvProjMatrix;
			JMatrix InvViewProjMatrix;
			JMatrix PrevFrameViewProjMatrix;
			JMatrix PrevFrameInvViewProjMatrix;


			JVector2 Resolution;
			f32 NearZ;
			f32 FarZ;

			JVector3 EyePosition;
			u32 PointLightCount;

			JVector2 ClusterSize;
			f32 ClusterScale;
			f32 ClusterBias;

			JVector3Uint NumClusterSlice;
			u32 FrameCount;
			u32 MaxRayDepth;

			JVector3 PrevFrameEyePosition;

			void Begin(const Graphics::RenderPassData& passData);
			void End();
		};

		enum EResource
		{
			Direct,
			IndirectR,
			IndirectG,
			IndirectB,
			Emissive,
			Shadow,
			HitPosition,
			RayDistance,
			MotionVector,
			ReprojectedNormalDepth,
			NormalDepth,
			Depth,
			PartialDepthDerivatives,
			Count,
		};

		enum EDenoiser
		{
			Denoise_IndirectR,
			Denoise_IndirectG,
			Denoise_IndirectB,
			Denoise_Ao,
			Denoise_Shadow,
			Denoise_Count
		};
	private:
		List<SharedPtr<ITopLevelAccelerationStructure>> mSceneAS;
		List<InstanceData> mInstances;

		SharedPtr<IRayTracingShaderResourceTable> mSRT;



		Renderer* mRenderer = nullptr;
		JVector2 mResolution;


		RP_Global_Tex mTex[EResource::Count];
		RP_Global_Tex mDenoisedTex[EDenoiser::Denoise_Count];
		RP_Global_Int mRayBounds;

		u64 mHitGroupOffset  = 0;
		u64 mHitGroupStride  = 3;


		SharedPtr<CalculatePartialDerivatives> mCalculatePartialDerivatives;
		SharedPtr<RT_Composite> mComposite;
		SharedPtr<RTAO>     mRTAO;

		SharedPtr<Denoiser> mDenoiser[EDenoiser::Denoise_Count];
		List<SharedPtr<ITexture>> mResources[EResource::Count];
		CB mCB;
	public:
		RayTracer(Renderer* renderer);
		void AddInstance(SharedPtr<ISubMesh> subMesh, SharedPtr<IMaterial> material, const List<JMatrix>& transform);
		void SetResolution(const JVector2& resolutoin);
		void Execute(SharedPtr<IComputeContext> context, SharedPtr<ITexture> targetTexture);
		void Reset();
	private:
		void Init();
		void InitTextures();
		void UpdateAccelerationStructure(SharedPtr<IComputeContext> context);
		void Update(SharedPtr<IComputeContext> context, SharedPtr<ITexture> targetTexture);
	
		SharedPtr<ITexture> GetResource(EResource type);
	public:
		static const String& GetDefaultRayTracingPipelineName();
		static u64 GetMaxConstantBufferSize();
		static SharedPtr<IRootSignature> CreateGlobalRootSignature();
		static SharedPtr<IRootSignature> CreateLocalRootSignature();

	};
}