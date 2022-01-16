#include "pch.h"
#include "Raytracer.h"
#include "Application.h"
#include "TopLevelAccelerationStructure.h"
#include "BottomLevelAccelerationStructure.h"
#include "RayTracingPipeline.h"
#include "Graphics/Resource.h"
#include "Graphics/Mesh.h"
#include "Graphics/RootSignature.h"
#include "Graphics/GraphicsHelper.h"

#define SHADER_PATH(x) PathHelper::CombinePath(PathHelper::CombinePath(Application::GetEnginePath(), "Shader/Raytracing"),x)

namespace JG
{
	RayTracer::RayTracer()
	{
		Init();


	}

	void RayTracer::SetResolution(const JVector2& resolutoin)
	{
		if (mResolution != resolutoin)
		{
			mResolution = resolutoin;
			InitTextures();
		}
	}

	void RayTracer::Execute(SharedPtr<IComputeContext> context)
	{
		context->BindRootSignature(mGlobalRootSignature);





	}

	void RayTracer::Init()
	{
		InitRootSignature();



		mSceneAS  = ITopLevelAccelerationStructure::Create();
		mPipeline = IRayTracingPipeline::Create();




		mPipeline->AddLibrary(SHADER_PATH("StandardRayTracingShader.hlsli"), 
			{"RayGeneration", "ClosestHit", "Miss"});
		mPipeline->AddHitGroup("HitGroup0", "ClosestHit", "", "");



		mPipeline->AddRayGenerationProgram("RayGeneration");
		mPipeline->AddHitProgram("HitGroup0");
		mPipeline->AddMissProgram("Miss");
		mPipeline->SetGlobalRootSignature(mGlobalRootSignature);
		mPipeline->SetMaxPayloadSize(sizeof(Color));
		mPipeline->SetMaxRecursionDepth(1);
		if (mPipeline->Generate() == false)
		{
			JG_CORE_ERROR("Failed RayTracing Pipline");
		}
	}

	void RayTracer::InitTextures()
	{

		//GraphicsHel


	}
	
	void RayTracer::InitRootSignature()
	{
		SharedPtr<IRootSignatureCreater> creater = IRootSignatureCreater::Create();
		creater->AddDescriptorTable(0, EDescriptorTableRangeType::UAV, 1024, 0, 0);
		creater->AddSRV(1, 0, 0);
		mGlobalRootSignature = creater->Generate();
	}
}

