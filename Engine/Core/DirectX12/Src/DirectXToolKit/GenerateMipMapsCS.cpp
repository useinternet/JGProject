#include<PCH.h>
#include"GenerateMipMapsCS.h"
#include"DirectXCore.h"
#include"Shader.h"
#define GENERATE_MIP_MAP_CS_PATH GLOBAL_SHADER_PATH("GenerateMips_CS.hlsl")
using namespace Dx12;
using namespace std;

GenerateMipMapsCS::GenerateMipMapsCS()
{
	auto device = DxDevice::GetDevice();

	m_RootSig.InitAsConstant(sizeof(GenerateMipsCB) / 4, 0);
	m_RootSig.InitAsDescriptor(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
	m_RootSig.InitAsDescriptor(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 4, 0);

	m_RootSig.AddSampler(CD3DX12_STATIC_SAMPLER_DESC(
		0,
		D3D12_FILTER_MIN_MAG_MIP_LINEAR,
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP));

	m_RootSig.Finalize();



	ComputeShader shader;
	shader.SetShaderPathAndFunction(GENERATE_MIP_MAP_CS_PATH, "main");
	if (!shader.Compile())
	{
		DX12_LOG_ERROR("%s is failed compile", (GENERATE_MIP_MAP_CS_PATH).c_str());
	}
	else
	{
		m_PSO.SetRootSig(m_RootSig.Get());
		m_PSO.SetComputeShader(shader.GetShaderCode("main"), shader.GetShaderCodeSize("main"));
		m_PSO.Finalize();
	}

	m_UAV = DxDevice::AllocateDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 4);
	uint32_t increaseSize = DxDevice::GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);


	for (uint32_t i = 0; i < 4; ++i)
	{
		D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
		uavDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		uavDesc.Texture2D.MipSlice = i;
		uavDesc.Texture2D.PlaneSlice = 0;

		device->CreateUnorderedAccessView(nullptr, nullptr, &uavDesc, m_UAV.GetDescriptorHandle(i));
	}


}