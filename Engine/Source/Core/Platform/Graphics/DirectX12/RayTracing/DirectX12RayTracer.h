#pragma once
#include "JGCore.h"
#include "Platform/Graphics/DirectX12/Utill/RayTracingHelper.h"



namespace JG
{
	class DirectX12RayTracer
	{



	public:



		// Mesh�� Material �ް� BottomLevelAcc ���
		// Mesh���� ���� �ѹ� Bottom�� �θ��� BLA ���� �ϰ� �״������ʹ� ������ BLA ȣ��
		// 
		// 
		// TopLevel�� �ڵ������� ����



		// Pipeline State & SBT
		// Shader ����


		// Ű�������ؼ� PipelineState & SBT ����
		// RootSignature Binding
		// Resource      Binding
		// SetMaxPayloadSize
		// SetMaxRecursionDepth
		// 

		void RegisterPieline();

	};



}