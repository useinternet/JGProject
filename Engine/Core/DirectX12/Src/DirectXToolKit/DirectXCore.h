#pragma once

#include"DirectXCommon/DirectXCommon.h"

namespace Dx12
{
	class Dx12CommonShaderDefines;
	class DescriptorAllocation;
	class CommandQueue;
	namespace DxDevice
	{
		// 장치 생성 함수들
		bool CreateDevice(const std::string& logpath);
		void DestroyDevice();



		DescriptorAllocation AllocateDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptors = 1);
		CommandQueue* GetCommandQueue(D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT);
		Dx12CommonShaderDefines* GetShaderCommonDefines();
		void ReleaseStaleDescriptors(uint64_t finishedFrame);
		void NewFrame();
		void Flush();
		// 장치 및 설정 변수 가져오기 함수들
		ID3D12Device*  GetDevice();
		IDXGIFactory4* GetFactory();
		
		std::vector<IDXGIAdapter*>    GetAdapter();
		UINT GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE type);


		uint32_t  GetFrameCount();
		bool      IsTearingSupport();
	}

}

