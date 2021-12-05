#pragma once
#include "Graphics/RenderProcess.h"






namespace JG
{
	class IComputer;
	class IReadBackBuffer;

	class PreRenderProcess_ComputeCluster : public IRenderProcess
	{
		// 
	public:
		struct CB
		{
			JMatrix InvProjMatrix;
			JVector3 EyePosition;
			f32 FarZ;
			JVector2 Resolution;
			JVector2 TileSize;
			f32 NearZ;
		};
		CB CB;
	private:
		List<SharedPtr<IComputer>>       mComputers;
		List<SharedPtr<IReadBackBuffer>> mReadBackBuffers;
	public:
		PreRenderProcess_ComputeCluster();
	public:
		virtual void Run(IGraphicsAPI* api, const RenderInfo& info) override;
		virtual bool IsCompelete() override;
	};
}