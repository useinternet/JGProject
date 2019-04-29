#include<PCH.h>
#include"Direct2DCore.h"

using namespace std;
namespace Dx2D
{
	std::string g_LogGroupName = "DirectX2D";
	namespace Dx2DDevice
	{
		static ComPtr<ID2D1Factory3>      g_Dx2DFactory;
		static ComPtr<IDWriteFactory>     g_DxWriteFactory;
		static float g_DpiX;
		static float g_DpiY;
		

		bool CreateDevice(ID3D12Device* dx12device, ComPtr<ID3D12CommandQueue> cmdQueue, const string& logpath)
		{
			REGISTER_LOG(g_LogGroupName, logpath);


			D2D1_FACTORY_OPTIONS d2dFactoryOptions = {};
#if defined(_DEVELOP_DEBUG) || defined(_DEVELOP_RELEASE)
			d2dFactoryOptions.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#endif


			HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, g_Dx2DFactory.GetAddressOf());
			if (FAILED(hr))
			{
				DX2D_LOG_ERROR("Failed Create D2D1Factory..");
				return false;
			}
			
			hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(g_DxWriteFactory.Get()), (IUnknown**)g_DxWriteFactory.GetAddressOf());
			if (FAILED(hr))
			{
				DX2D_LOG_ERROR("Failed Create DxWriteFactory..");
				return false;
			}


			g_Dx2DFactory->GetDesktopDpi(&g_DpiX, &g_DpiY);





			DX2D_LOG_INFO("2DDevice Complete");
			return true;
		}
		void DestoryDevice()
		{
			g_DxWriteFactory.Reset();
			g_Dx2DFactory.Reset();
		}
		ID2D1Factory* GetDx2DFactory()
		{
			return g_Dx2DFactory.Get();
		}
		IDWriteFactory* GetDxWriteFactory()
		{
			return g_DxWriteFactory.Get();
		}
		float GetDpiX()
		{
			return g_DpiX;
		}
		float GetDpiY()
		{
			return g_DpiY;
		}
	}
}
