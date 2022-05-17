#pragma once
#include "Graphics/RenderProcess.h"






namespace JG
{
	class IRootSignature;
	class PreRenderProcess_ComputeCluster;

	class PreRenderProcess_LightCulling : public IRenderProcess
	{
	public:
		static const i32 MAX_VISIBLE_LIGHTINDEX_COUNT = _256KB / sizeof(u32);
		const static constexpr char* SHADER_NAME = "LightCulling";
		struct CB
		{
			JMatrix		 ViewMatirx;
			i32			 PointLightCount = 0;
			u32			 NumXSlice = 0;
			u32			 NumYSlice = 0;
			u32			 NumZSlice = 0;
		};
		CB CB;
	private:
		SharedPtr<IStructuredBuffer> mLightGridSB;
		SharedPtr<IStructuredBuffer> mVisibleLightIndiciesSB;
		SharedPtr<IStructuredBuffer> mClusterSB;
		SharedPtr<IComputeShader> mShader;
		SharedPtr<IRootSignature> mRootSignature;

	public:
		virtual void Awake(const AwakeData& data) override;
		virtual void Ready(const ReadyData& data) override;
		virtual void Run(const RunData& data) override;
		virtual bool IsCompelete() override;
		virtual Type GetType() const override;
		void SetClusters(SharedPtr<IStructuredBuffer> sb);
	};
}