#pragma once
#include "JGCore.h"
#include "Graphics/RenderParam.h"

namespace JG
{
	namespace Compute
	{
		class FloatAccumulater;
		class Blur_Float;
	}
	class ITopLevelAccelerationStructure;
	class IBottomLevelAccelerationStructure;
	class IRayTracingPipeline;
	class IComputeContext;
	class ISubMesh;
	class ITexture;
	class IRootSignature;
	class Renderer;

	class RayTracer
	{
	private:
		struct CB
		{
			JMatrix ProjMatrix;
			JMatrix ViewMatrix;
			JMatrix InverseProjMatrix;
			JMatrix InverseViewMatrix;
			JVector3 EyePosition;
			f32  NearZ;
			f32  FarZ;
			i32  PointLightCount;
			u32  FrameCount;
		};
		enum
		{
			RootParam_Output,
			RootParam_SceneAS,
			RootParam_CB,
			RootParam_PointLightSB,
		};
		CB _CB;
	private:
		List<SharedPtr<ITopLevelAccelerationStructure>> mSceneAS;
		SharedPtr<IRayTracingPipeline>	          mPipeline;
		SharedPtr<IRootSignature>				  mGlobalRootSignature;
		Renderer* mRenderer = nullptr;
		JVector2 mResolution;
		List<SharedPtr<ITexture>> mOutputTextures;

		RP_Global_Tex mShadowTex;



		bool mIsResetAccumCount = false;
		SharedPtr<Compute::FloatAccumulater> mShadowAccumulater;
		SharedPtr<Compute::Blur_Float> mShaowBlur;
	public:
		RayTracer(Renderer* renderer);
		void AddInstance(SharedPtr<IBottomLevelAccelerationStructure> btas, const JMatrix& transform, u32 instanceID, u32 hitGroupIndex);
		void SetResolution(const JVector2& resolutoin);
		void Execute(SharedPtr<IComputeContext> context);
		void Reset();
		void UpdateCB();
	private:
		void Init();
		void InitTextures();
		void InitRootSignature();
	};
}