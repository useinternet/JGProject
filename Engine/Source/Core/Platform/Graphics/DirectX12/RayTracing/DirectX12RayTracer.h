#pragma once
#include "JGCore.h"
#include "Platform/Graphics/DirectX12/Utill/RayTracingHelper.h"



namespace JG
{
	class DirectX12RayTracer
	{



	public:



		// Mesh와 Material 받고 BottomLevelAcc 얻기
		// Mesh에서 최초 한번 Bottom을 부르면 BLA 생성 하고 그다음부터는 생성한 BLA 호출
		// 
		// 
		// TopLevel은 자동적으로 생성



		// Pipeline State & SBT
		// Shader 설정


		// 키값으로해서 PipelineState & SBT 생성
		// RootSignature Binding
		// Resource      Binding
		// SetMaxPayloadSize
		// SetMaxRecursionDepth
		// 

		void RegisterPieline();

	};



}