#include "pch.h"
#include "DxObject.h"
#include "RenderDevice.h"
using namespace std;


namespace RE
{


	DxObject::DxObject() : ReObject("DxObject") {}
	DxObject::DxObject(const std::string& name) : ReObject(name) {}


	ID3D12Device* DxObject::GetD3DDevice()
	{
		return GetRenderDevice()->GetD3DDevice();
	}
	ID3D12Device* DxObject::GetD3DDevice() const
	{
		return GetRenderDevice()->GetD3DDevice();
	}
}