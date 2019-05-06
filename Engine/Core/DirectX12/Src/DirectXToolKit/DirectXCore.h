#pragma once

#include"DirectXCommon/DirectXCommon.h"

namespace Dx12
{
	class Dx12CommonShaderDefines;
	class DescriptorAllocation;
	class CommandQueue;
	namespace DxDevice
	{
		// DirectX12 장치 생성
		bool CreateDevice();


		// DirectX12 장치 제거
		void DestroyDevice();


		/* Descriptor 힙 할당 하기
		@param type : DescriptorHeap 타입 
		@param numDescriptors : 할당할 Descriptor 힙 갯수  */
		DescriptorAllocation AllocateDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptors = 1);



		// CommandQueue 얻기
		// @param type : CommandQeueu 타입
		CommandQueue* GetCommandQueue(D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT);



		// 내장 Shader 정의 클래스 얻기
		Dx12CommonShaderDefines* GetShaderCommonDefines();


		// 사용하고 버려진 DescriptorHeap 삭제 
		// @param finishedFrame : 장치내 카운트되어지고있는 Frame
		void ReleaseStaleDescriptors(uint64_t finishedFrame);

		// 장치 내 카운트 증가
		void NewFrame();

		// 모든 타입의 CommandQueue 명령 리스트 비우기
		void Flush();


		// ID3D12Device* 가져오기
		ID3D12Device*  GetDevice();

		// IDXGIFactory4* 가져오기
		IDXGIFactory4* GetFactory();

		// IDXGIAdapter 리스트 가져오기 
		std::vector<IDXGIAdapter*>    GetAdapter();

		// DescriptorHeap 하나당 크기 얻어오기
		// @param type : DescriptorHeap 타입
		UINT GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE type);

		// 장치내 카운트 가져오기
		uint32_t  GetFrameCount();

		// Tearing 기능이 지원되는지 여부
		bool      IsTearingSupport();
	}

}

