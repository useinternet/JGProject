#pragma once
#include "JGCore.h"
#include "Graphics/RenderParam.h"

namespace JG
{
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
			JMatrix _ProjMatrix;
			JMatrix _ViewMatrix;
			JMatrix _InverseProjMatrix;
			JMatrix _InverseViewMatrix;
			JVector3 _EyePosition;
			f32  _NearZ;
			f32  _FarZ;
			i32  _PointLightCount;
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