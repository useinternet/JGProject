#include "pch.h"
#include "DX12GraphicsDevice.h"
#include "DirectXApi.h"
using namespace std;
using namespace Microsoft::WRL;
using namespace DX12;

GE_REGISTER_GLOBALDATA
GE_REGISTER_DEVICECLASS(DX12::DX12GraphicsDevice);


DX12::DX12GraphicsDevice* g_Instance = nullptr;
void DX12::DX12GraphicsDevice::Init()
{
	if (g_Instance) assert("X12GraphicsDevice can make only one");
	g_Instance = this;


	DX12::DXDevice::Create();
}

bool DX12::DX12GraphicsDevice::Load()
{
	return true;

}

void DX12::DX12GraphicsDevice::Update()
{

	//DX12::DXDevice::Update();


}

void DX12::DX12GraphicsDevice::Destroy()
{

	DX12::DXDevice::Destroy();

}






