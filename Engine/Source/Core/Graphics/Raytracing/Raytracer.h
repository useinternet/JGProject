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
		/*
			VertexOut  vsOut = getVertexAttributes(PrimitiveIndex(), attribs);
	return prepareShadingData(vsOut, gMaterial, gCamera.posW);
		*/
	private:
		struct PassData
		{
			List<SharedPtr<ITexture>>	   Results;
			SharedPtr<IRayTracingPipeline> Pipeline;
			SharedPtr<IRootSignature>	   RootSignature;
			RP_Global_Tex Result;
		};
	private:
		List<SharedPtr<ITopLevelAccelerationStructure>> mSceneAS;
		Renderer* mRenderer = nullptr;
		JVector2 mResolution;



		bool mIsResetAccumCount = false;
		SharedPtr<Compute::FloatAccumulater> mShadowAccumulater;
		SharedPtr<Compute::Blur_Float> mShaowBlur;




		PassData mShadow;
		PassData mReflection;
		PassData mGlobalIllumination;
	public:
		RayTracer(Renderer* renderer);
		void AddInstance(SharedPtr<IBottomLevelAccelerationStructure> btas, const JMatrix& transform, u32 instanceID, u32 hitGroupIndex);
		void SetResolution(const JVector2& resolutoin);
		void Execute(SharedPtr<IComputeContext> context);
		void Reset();
	private:
		void Init();
		void InitTextures();





		void BuildShadow();
		void BuildReflection();
		void BuildGlobalIllumination();

		void UpdateAccelerationStructure(SharedPtr<IComputeContext> context);
		void UpdateShadow(SharedPtr<IComputeContext> context);
		void UpdateReflection(SharedPtr<IComputeContext> context);
		void UpdateGlobalIllumination(SharedPtr<IComputeContext> context);
	};
}