#include<PCH.h>
#include"DxUtil.h"
#include"DirectXCore.h"
#include<DirectXTex/DirectXTex.h>
#include<Util/StringUtil.h>
#include"CommandQueue.h"
using namespace DirectX;
using namespace std;
using namespace Common;
namespace Dx12
{
	namespace DxUtil
	{
		ComPtr<ID3D12CommandQueue> CreateCommandQueue(D3D12_COMMAND_LIST_TYPE type)
		{
			ComPtr<ID3D12CommandQueue> result;
			D3D12_COMMAND_QUEUE_DESC Desc;
			Desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
			Desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
			Desc.Type = type;
			Desc.NodeMask = 0;

			ThrowIfFailed(DxDevice::GetDevice()->CreateCommandQueue(
				&Desc,
				IID_PPV_ARGS(&result)));

			return result;
		}




		ComPtr<ID3D12GraphicsCommandList> CreateCommandList(
			D3D12_COMMAND_LIST_TYPE type,
			ComPtr<ID3D12CommandAllocator> CmdAllocator)
		{
			ComPtr<ID3D12GraphicsCommandList> result;

			ThrowIfFailed(DxDevice::GetDevice()->CreateCommandList(
				0, type, CmdAllocator.Get(), nullptr, IID_PPV_ARGS(&result)));

			return result;
		}
		ComPtr<ID3D12CommandAllocator>    CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE type)
		{
			ComPtr<ID3D12CommandAllocator> result;

			ThrowIfFailed(DxDevice::GetDevice()->CreateCommandAllocator(
				type, IID_PPV_ARGS(&result)));

			return result;
		}

		ComPtr<IDXGISwapChain4> CreateSwapChain(
			ID3D12CommandQueue* CmdQue,
			HWND hWnd,
			int width, int height,
			int bufferCount,
			DXGI_FORMAT format)
		{
			auto Device = DxDevice::GetDevice();


			ComPtr<IDXGISwapChain4> dxgiSwapChain4;
			ComPtr<IDXGIFactory4>   dxgiFactory4;
			UINT createFactoryFlags = 0;
#if defined(_DEVELOP_DEBUG)
			createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif

			ThrowIfFailed(CreateDXGIFactory2(
				createFactoryFlags, 
				IID_PPV_ARGS(&dxgiFactory4)));

			DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
			swapChainDesc.Width = width;
			swapChainDesc.Height = height;
			swapChainDesc.Format = format;
			swapChainDesc.Stereo = FALSE;
			swapChainDesc.SampleDesc = { 1, 0 };
			swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			swapChainDesc.BufferCount = bufferCount;
			swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
			swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
			swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
			//
			swapChainDesc.Flags = 0;
	

			ComPtr<IDXGISwapChain1> swapChain1;
			ThrowIfFailed(dxgiFactory4->CreateSwapChainForHwnd(
				CmdQue,
				hWnd,
				&swapChainDesc,
				nullptr,
				nullptr,
				&swapChain1));

	

			ThrowIfFailed(dxgiFactory4->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER));

			ThrowIfFailed(swapChain1.As(&dxgiSwapChain4));
			return dxgiSwapChain4;
		}

		ComPtr<ID3DBlob> CompileShader(
			const wstring& filename,
			const D3D_SHADER_MACRO* defines,
			const string& entrypoint,
			const string& target,
			bool* isSuccess,
			std::string* errorStr)
		{
			UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEVELOP_DEBUG)  
			compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
			HRESULT hr = S_OK;

			ComPtr<ID3DBlob> byteCode = nullptr;
			ComPtr<ID3DBlob> errors;
			hr = D3DCompileFromFile(filename.c_str(), defines, D3D_COMPILE_STANDARD_FILE_INCLUDE,
				entrypoint.c_str(), target.c_str(), compileFlags, 0, &byteCode, &errors);

			if (errors != nullptr)
			{
				OutputDebugStringA((char*)errors->GetBufferPointer());
				if (errorStr)
				{
					*errorStr = string((char*)errors->GetBufferPointer());
				}
			}
				


			if (isSuccess != nullptr)
			{
				(SUCCEEDED(hr)) ?
					*isSuccess = true :
					*isSuccess = false;
			}



			DX12_LOG_INFO("%s : shader compile success..", Common::Util::AnsiToString(filename).c_str());
			return byteCode;
		}

		ComPtr<ID3D12RootSignature> CreateRootSignature(CD3DX12_ROOT_SIGNATURE_DESC* rootSigDesc)
		{
			ComPtr<ID3D12RootSignature> rootSig;

			ComPtr<ID3DBlob> serializedRootSig;
			ComPtr<ID3DBlob> errorBlob;

			ThrowIfFailed(D3D12SerializeRootSignature(
				rootSigDesc, 
				D3D_ROOT_SIGNATURE_VERSION_1,
				serializedRootSig.GetAddressOf(), 
				errorBlob.GetAddressOf()));


			ThrowIfFailed(DxDevice::GetDevice()->CreateRootSignature(
				0, serializedRootSig->GetBufferPointer(), serializedRootSig->GetBufferSize(),
				IID_PPV_ARGS(rootSig.GetAddressOf())));

			return rootSig;
		}
		UINT CalcConstantBufferByteSize(UINT byteSize)
		{
			return (byteSize + 255) & ~255;
		}
	}
}




