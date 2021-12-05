#include "pch.h"
#include "PreRenderProcess_ComputeCluster.h"
#include "Graphics/JGGraphics.h"


#define COMPUTE_CLUSTER_SHADER_NAME "ComputeCluster"

namespace JG
{
	PreRenderProcess_ComputeCluster::PreRenderProcess_ComputeCluster()
	{
		u64 bufferCnt = JGGraphics::GetInstance().GetBufferCount();


		auto shader = ShaderLibrary::GetInstance().GetShader(COMPUTE_CLUSTER_SHADER_NAME);
		if (shader != nullptr)
		{
			for (u64 i = 0; i < bufferCnt; ++i)
			{
				auto computer = IComputer::Create("ClusterComputer", shader);
				auto readBackBuffer = IReadBackBuffer::Create("ClusterComputer_ReadBackBuffer");

				mComputers.push_back(computer);
				mReadBackBuffers.push_back(readBackBuffer);
			}
		}
	}
	void PreRenderProcess_ComputeCluster::Run(IGraphicsAPI* api, const RenderInfo& info)
	{
		if (mComputers.empty() || mReadBackBuffers.empty())
		{
			return;
		}
		u64 bufferCnt = JGGraphics::GetInstance().GetBufferCount();
		i32 bufferIndex = info.CurrentBufferIndex;


		// Dispatch
		{
			auto& computer  = mComputers[bufferIndex];
			computer->SetFloat4x4("__InvProjMatrix__", CB.InvProjMatrix);
			computer->SetFloat3("__EyePosition__", CB.EyePosition);
			// 변수 설정
		}





		i32 computerIndex  = info.CurrentBufferIndex;
		i32 readBackBufferIndex = computerIndex - 1;
		if (readBackBufferIndex < 0) readBackBufferIndex = bufferCnt - 1;

		auto computer       = mComputers[computerIndex];
		auto readBackBuffer = mReadBackBuffers[readBackBufferIndex];












		// 0, 1, 2, 0, 1, 2  0
		//    0, 1, 2, 0, 1 ,2
		//             0, 1, 2 


		// computer shader 돌리기    
		// readback 으로 값알아오기
		// readback 으로 cpu값 복사










	}

	bool PreRenderProcess_ComputeCluster::IsCompelete()
	{
		return true;
	}
}

