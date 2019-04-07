#pragma once
#include"DirectXCommon/DirectXCommon.h"

namespace DirectX
{
	class ScratchImage;
}
namespace Dx12
{
	namespace DxUtil
	{
		// Create Command Objects
		
		/* ID3D12CommandQueue 생성
		@param type : 커맨드 리스트 타입 */
		ComPtr<ID3D12CommandQueue>        CreateCommandQueue(D3D12_COMMAND_LIST_TYPE type);


		/* ID3D12GraphicsCommandList 생성
		@param type : 커맨드 리스트 타입 
		@param CmdAllocator : 커맨드 할당자 */
		ComPtr<ID3D12GraphicsCommandList> CreateCommandList(
			D3D12_COMMAND_LIST_TYPE type,
			ComPtr<ID3D12CommandAllocator> CmdAllocator);

		/* ID3D12CommandAllocator 생성
        @param type : 커맨드 리스트 타입 */
		ComPtr<ID3D12CommandAllocator>    CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE type);

		/* IDXGISwapChain 생성 
		@param CmdQue : ID3D12CommandQueue
		@param HWND   : WindowHandle
		@param width  : 백버퍼 가로 크기
		@param height : 백버퍼 세로 크기
		@param bufferCount : 백버퍼 갯수 
		@param format : 백버퍼 Format  */
		ComPtr<IDXGISwapChain4> CreateSwapChain(
			ID3D12CommandQueue* CmdQue,
			HWND hWnd, 
			int width, int height, 
			int bufferCount,
			DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM);


		/* 셰이더 컴파일
		@param filename :  hlsl 파일 path
		@param define   : shader define 설정
		@param entrypoint : 시작 함수(main) 이름
		@param target   : 셰이더 모델 */
		ComPtr<ID3DBlob> CompileShader(
			const std::wstring& filename,
			const D3D_SHADER_MACRO* defines,
			const std::string& entrypoint,
			const std::string& target,
			bool* isSuccess = nullptr,
			std::string* errorStr = nullptr);


		/* 루트 서명 생성  */
		ComPtr<ID3D12RootSignature> CreateRootSignature(CD3DX12_ROOT_SIGNATURE_DESC* rootSigDesc);


		UINT CalcConstantBufferByteSize(UINT byteSize);
	}

}
