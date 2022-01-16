#pragma once
#include "JGCore.h"


namespace JG
{
	class ITopLevelAccelerationStructure;
	class IBottomLevelAccelerationStructure;
	class IRayTracingPipeline;
	class IComputeContext;
	class ISubMesh;
	class RayTracer
	{
		SharedPtr<ITopLevelAccelerationStructure> mSceneAS;
		SharedPtr<IRayTracingPipeline>	          mPipeline;
		SharedPtr<IRootSignature>				  mGlobalRootSignature;


		JVector2 mResolution;
		List<SharedPtr<ITexture>> mOutputTextures;
	public:
		RayTracer();
		void SetResolution(const JVector2& resolutoin);
		void Execute(SharedPtr<IComputeContext> context);

	private:
		void Init();
		void InitTextures();
		void InitRootSignature();
	};
}