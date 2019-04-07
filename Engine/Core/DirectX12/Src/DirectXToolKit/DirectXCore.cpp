#include<PCH.h>
#include"DirectXCore.h"
#include"DescriptorAllocator.h"
#include"CommandQueue.h"
#include"DirectXObjects/ShaderCommonDefines.h"
using namespace std;
using namespace Common;
namespace Dx12
{
	namespace DxDevice
	{
		// namespace DxDevice 전역 변수들
		static bool g_IsInitDevice   = false;
		static bool g_IsAllowTearing = false;
		string g_LogGroupName = "DirectX12";

		// DirectX 장치 변수
		static ID3D12Device*               g_Device;
		static IDXGIFactory4*              g_Factory;
		static std::unique_ptr<Dx12CommonShaderDefines> g_ShaderCommonDefines;
		static vector<IDXGIAdapter*>       g_Adapter;
		static vector<unique_ptr<DescriptorAllocator>> g_DescriptorAllocator;
		static vector<unique_ptr<CommandQueue>>        g_CommandQueue;
		// DirectX 장치 설정 변수
		static DXGI_FORMAT SCBackBufferFormat  = DXGI_FORMAT_R8G8B8A8_UNORM;
		static DXGI_FORMAT SCDepthBufferFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
		static uint32_t g_RtvDescriptorSize = 0;
		static uint32_t g_DsvDescriptorSize = 0;
		static uint32_t g_CbvSrvUavDescriptorSize = 0;
		static uint32_t g_DeviceFrameCount = 0;

		// 커맨드 큐 및 어댑터 장치 로그 찍기 함수 (인터페이스 X)
		void CreateDescriptorAllocator();
		void CreateCommandQueues();
		bool CheckTearingSupport();
		void LogAdapters();
		void LogAdapterOutputs(IDXGIAdapter* adapter);
		void LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format);


		// 인터페이스 함수들 
		bool CreateDevice(const string& logpath)
		{
			UINT FactoryFlags = 0;
			REGISTER_LOG(g_LogGroupName, logpath);
			if (g_IsInitDevice) {
				// log error
				return false;
			}
#if defined(_DEVELOP_DEBUG) || defined(_DEVELOP_RELEASE)
			{
				FactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
				ComPtr<ID3D12Debug> debugController;
				ThrowIfFailed(
					D3D12GetDebugInterface(IID_PPV_ARGS(debugController.GetAddressOf())));
				debugController->EnableDebugLayer();
			}
#endif

			// 팩토리 생성
			ThrowIfFailed(CreateDXGIFactory2(FactoryFlags, IID_PPV_ARGS(&g_Factory)));


			// 장치 생성
			HRESULT hResult = D3D12CreateDevice(
				nullptr, // default
				D3D_FEATURE_LEVEL_11_0,
				IID_PPV_ARGS(&g_Device));
			if (FAILED(hResult))
			{
				ComPtr<IDXGIAdapter> pWarpAdapter;
				ThrowIfFailed(g_Factory->EnumWarpAdapter(
					IID_PPV_ARGS(pWarpAdapter.GetAddressOf())));

				ThrowIfFailed(D3D12CreateDevice(
					pWarpAdapter.Get(),
					D3D_FEATURE_LEVEL_11_0,
					IID_PPV_ARGS(&g_Device)));
			}
			g_RtvDescriptorSize = g_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
			g_DsvDescriptorSize = g_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
			g_CbvSrvUavDescriptorSize = g_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
			g_IsAllowTearing = CheckTearingSupport();

			CreateDescriptorAllocator();
			CreateCommandQueues();
			LogAdapters();


			g_ShaderCommonDefines = make_unique<Dx12CommonShaderDefines>();
			DX12_LOG_INFO("DX12 Device Init Success..");
			return true;
		}

		void DestroyDevice()
		{
			g_ShaderCommonDefines.reset();
			g_DescriptorAllocator.clear();
		
			for (auto& adap : g_Adapter) {
				adap->Release();
				adap = nullptr;
			}
			if (g_Device) {
				g_Device->Release();
				g_Device = nullptr;
			}
			if (g_Factory) {
				g_Factory->Release();
				g_Factory = nullptr;
			}
		}
		DescriptorAllocation AllocateDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptors)
		{
			return g_DescriptorAllocator[type]->Allocate(numDescriptors);
		}
		CommandQueue* GetCommandQueue(D3D12_COMMAND_LIST_TYPE type)
		{
			switch (type)
			{
			case D3D12_COMMAND_LIST_TYPE_DIRECT:
				return g_CommandQueue[0].get();
			case D3D12_COMMAND_LIST_TYPE_COPY:
				return g_CommandQueue[1].get();
			case D3D12_COMMAND_LIST_TYPE_COMPUTE:
				return g_CommandQueue[2].get();
			}
			return nullptr;
		}
		Dx12CommonShaderDefines* GetShaderCommonDefines()
		{
			return g_ShaderCommonDefines.get();
		}
		void ReleaseStaleDescriptors(uint64_t finishedFrame)
		{
			for (int i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++i)
			{
				g_DescriptorAllocator[i]->ReleaseStaleDescriptors(finishedFrame);
			}
		}
		void   NewFrame()
		{
			g_DeviceFrameCount++;
		}
		void Flush()
		{
			for (auto& que : g_CommandQueue)
			{
				que->Flush();
			}
		}
		ID3D12Device* GetDevice()
		{
			if (g_IsInitDevice) {
				DX12_LOG(Log::Critical, "Device is not init");
				return nullptr;
			}
			return g_Device;
		}
		IDXGIFactory4* GetFactory()
		{
			if (g_IsInitDevice) {
				DX12_LOG(Log::Critical, "Device is not init");
				return nullptr;
			}
			return g_Factory;
		}

		vector<IDXGIAdapter*> GetAdapter()
		{
			if (g_IsInitDevice) {
				DX12_LOG(Log::Critical, "Device is not init");
				return vector<IDXGIAdapter*>();
			}
			vector<IDXGIAdapter*> result(g_Adapter.size());
			for (int i = 0; i < g_Adapter.size(); ++i)
				result[i] = g_Adapter[i];

			return result;
		}
		UINT GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE type)
		{
			return g_Device->GetDescriptorHandleIncrementSize(type);
		}
		uint32_t  GetRtvDescriptorSize() {
			return g_RtvDescriptorSize;
		}
		uint32_t  GetDsvDescriptorSize() {
			return g_DsvDescriptorSize;
		}
		uint32_t  GetCbvSrvUavDescriptorSize() {
			return g_CbvSrvUavDescriptorSize;
		}
		std::uint32_t  GetFrameCount() {
			return g_DeviceFrameCount;
		}
		bool           IsTearingSupport() {
			return g_IsAllowTearing;
		}
		void CreateDescriptorAllocator()
		{
			for (int i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++i)
			{
				g_DescriptorAllocator.push_back(make_unique<DescriptorAllocator>(static_cast<D3D12_DESCRIPTOR_HEAP_TYPE>(i)));
			}
		}
		void CreateCommandQueues()
		{
			g_CommandQueue.resize(3);
			g_CommandQueue[0] = make_unique<CommandQueue>(D3D12_COMMAND_LIST_TYPE_DIRECT);
			g_CommandQueue[1] = make_unique<CommandQueue>(D3D12_COMMAND_LIST_TYPE_COPY);
			g_CommandQueue[2] = make_unique<CommandQueue>(D3D12_COMMAND_LIST_TYPE_COMPUTE);
		}
		bool CheckTearingSupport()
		{
			BOOL allowTearing = FALSE;
			ComPtr<IDXGIFactory4> factory4;
			if (SUCCEEDED(CreateDXGIFactory1(IID_PPV_ARGS(&factory4))))
			{
				ComPtr<IDXGIFactory5> factory5;
				if (SUCCEEDED(factory4.As(&factory5)))
				{
					if (FAILED(factory5->CheckFeatureSupport(
						DXGI_FEATURE_PRESENT_ALLOW_TEARING,
						&allowTearing, sizeof(allowTearing))))
					{
						allowTearing = FALSE;
					}
				}
			}

			return allowTearing == TRUE;
		}
		// 로그
		void LogAdapters()
		{
			UINT i = 0;
			IDXGIAdapter* adapter = nullptr;
			while (g_Factory->EnumAdapters(i, &adapter) != DXGI_ERROR_NOT_FOUND)
			{
				g_Adapter.push_back(adapter);
				++i;
			}

			for (size_t i = 0; i < g_Adapter.size(); ++i)
			{
				LogAdapterOutputs(g_Adapter[i]);
			}
		}
		void LogAdapterOutputs(IDXGIAdapter* adapter)
		{
			UINT i = 0;
			IDXGIOutput* output = nullptr;
			while (adapter->EnumOutputs(i, &output) != DXGI_ERROR_NOT_FOUND)
			{
				LogOutputDisplayModes(output, DXGI_FORMAT_R8G8B8A8_UNORM);

				output->Release();
				output = nullptr;
				++i;
			}
		}

		void LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format)
		{
			UINT count = 0;
			UINT flags = 0;

			// Call with nullptr to get list count.
			output->GetDisplayModeList(format, flags, &count, nullptr);

			std::vector<DXGI_MODE_DESC> modeList(count);
			output->GetDisplayModeList(format, flags, &count, &modeList[0]);

			for (auto& x : modeList)
			{
				//UINT n = x.RefreshRate.Numerator;
				//UINT d = x.RefreshRate.Denominator;
			}
		}

	}

}




