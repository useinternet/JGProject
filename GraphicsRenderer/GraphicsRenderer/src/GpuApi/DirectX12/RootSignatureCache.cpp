#include"pch.h"
#include"RootSignatureCache.h"
#include"RenderObject.h"
#include"Renderer.h"


namespace GR
{
	namespace Dx12
	{

		RootSignature RootSignatureCache::GetRootSignature(ERootSignature enumpass)
		{
			RootSignature rootSig;
			if (m_RootSignatureCache.find(enumpass) != m_RootSignatureCache.end())
			{
				return m_RootSignatureCache[enumpass];
			}

			switch (enumpass)
			{
			case ERootSignature::G_Common:
				rootSig.InitShaderResourceView(0, 0); 
				rootSig.InitConstantBufferView(0, 0);
				rootSig.InitDescriptorTable(RootSignature::SRV, 8, 0, 1,
					D3D12_SHADER_VISIBILITY_PIXEL);
				rootSig.InitDescriptorTable(RootSignature::SRV, 8, 0, 2,
					D3D12_SHADER_VISIBILITY_PIXEL);
				rootSig.AddStaticSampler(0, RootSignature::ANISOTROPIC_WRAP);
				rootSig.AddStaticSampler(1, RootSignature::ANISOTROPIC_CLAMP);
				rootSig.AddStaticSampler(2, RootSignature::LINEAR_WRAP);
				rootSig.AddStaticSampler(3, RootSignature::LINEAR_CLAMP);
				rootSig.AddStaticSampler(4, RootSignature::POINT_WRAP);
				rootSig.AddStaticSampler(5, RootSignature::POINT_CLAMP);
				break;
			case ERootSignature::C_IrrMap:
			case ERootSignature::C_InputOutput:
				rootSig.InitDescriptorTable(RootSignature::SRV, 1, 0);
				rootSig.InitDescriptorTable(RootSignature::UAV, 1, 0);
				rootSig.AddStaticSampler(0, RootSignature::LINEAR_WRAP);
				break;
			case ERootSignature::C_GenerateMipMaps:
				rootSig.InitConstantBufferView(0);
				rootSig.InitDescriptorTable(RootSignature::SRV, 1, 0);
				rootSig.InitDescriptorTable(RootSignature::UAV, 4, 0);
				rootSig.AddStaticSampler(0, RootSignature::LINEAR_CLAMP);
				break;
			case ERootSignature::C_SpecularBRDF:
				rootSig.InitDescriptorTable(RootSignature::UAV, 1, 0);
				break;
			case ERootSignature::C_SpecularMap:
				rootSig.InitConstantBufferView(0);
				rootSig.InitDescriptorTable(RootSignature::SRV, 1, 0);
				rootSig.InitDescriptorTable(RootSignature::UAV, 1, 0);
				rootSig.AddStaticSampler(0, RootSignature::LINEAR_CLAMP);
				break;
			}

			rootSig.Finalize();
			m_RootSignatureCache[enumpass] = rootSig;
			return rootSig;
		}

	}
}