#include"pch.h"
#include"PipelineState.h"
#include"GraphicsResource.h"
#include"GraphicsDevice.h"
#include"Dx12Helper.h"
#include"RootSignature.h"
using namespace std;

namespace GR
{
	namespace Dx12
	{
		static map< size_t, ComPtr<ID3D12PipelineState>> s_GraphicsPSOHashMap;
		static map< size_t, ComPtr<ID3D12PipelineState>> s_ComputePSOHashMap;


		GraphicsPSO::GraphicsPSO()
		{
			ZeroMemory(&m_Desc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));

			m_Desc.NodeMask = 0;
			m_Desc.SampleMask = 0xFFFFFFFFu;
			m_Desc.SampleDesc.Count = 1;
			m_Desc.SampleDesc.Quality = 0;
			m_Desc.InputLayout.NumElements = 0;
			m_Desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			m_Desc.BlendState        = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
			m_Desc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
			m_Desc.RasterizerState   = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		}
		void GraphicsPSO::SetBlendState(const D3D12_BLEND_DESC& desc)
		{
			m_Desc.BlendState = desc;
		}
		void GraphicsPSO::SetDepthStencilState(const D3D12_DEPTH_STENCIL_DESC& desc)
		{
			m_Desc.DepthStencilState = desc;
		}
		void GraphicsPSO::SetRasterizerState(const D3D12_RASTERIZER_DESC& desc)
		{
			m_Desc.RasterizerState = desc;
		}
		void GraphicsPSO::SetSampleMask(uint32_t sampleMask)
		{
			m_Desc.SampleMask = sampleMask;
		}
		void GraphicsPSO::SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE type)
		{
			m_Desc.PrimitiveTopologyType = type;

		}
		void GraphicsPSO::SetRenderTargetFormats(
			const std::vector<uint32_t>& slots,
			const std::vector<DXGI_FORMAT>& formats,
			DXGI_FORMAT DSVFormat,
			uint32_t msaaCount ,
			uint32_t msaaQuality )
		{
			uint32_t numRTV = (uint32_t)slots.size();
			for (uint32_t i = 0; i< numRTV; ++i)
			{
				m_Desc.RTVFormats[i] = formats[i];
			}
			for (uint32_t i = numRTV; i < m_Desc.NumRenderTargets; ++i)
			{
				m_Desc.RTVFormats[i] = DXGI_FORMAT_UNKNOWN;
			}

			m_Desc.NumRenderTargets = (UINT)slots.size();
			m_Desc.DSVFormat = DSVFormat;
			m_Desc.SampleDesc.Count = msaaCount;
			m_Desc.SampleDesc.Quality = msaaQuality;

			
		}
		void GraphicsPSO::SetInputLayout(uint32_t numInputLayout, D3D12_INPUT_ELEMENT_DESC* inputDescs)
		{
			m_Desc.InputLayout.NumElements = numInputLayout;
			if (numInputLayout != 0)
			{
				D3D12_INPUT_ELEMENT_DESC* NewElements = (D3D12_INPUT_ELEMENT_DESC*)malloc(sizeof(D3D12_INPUT_ELEMENT_DESC) * m_Desc.InputLayout.NumElements);
				memcpy(NewElements, inputDescs, m_Desc.InputLayout.NumElements * sizeof(D3D12_INPUT_ELEMENT_DESC));
				m_InputLayout.reset((const D3D12_INPUT_ELEMENT_DESC*)NewElements);
			}
			else
			{
				m_InputLayout = nullptr;
			}
		}
		void GraphicsPSO::SetPrimitiveRestart(D3D12_INDEX_BUFFER_STRIP_CUT_VALUE ibProps)
		{
			m_Desc.IBStripCutValue = ibProps;
		}
		void GraphicsPSO::BindVertexShader(const VertexShader& shader)
		{
			m_Desc.VS.pShaderBytecode = shader.GetBufferPointer();
			m_Desc.VS.BytecodeLength = shader.GetSize();
		}
		void GraphicsPSO::BindHullShader(const HullShader& shader)
		{
			m_Desc.HS.pShaderBytecode = shader.GetBufferPointer();
			m_Desc.HS.BytecodeLength = shader.GetSize();

		}
		void GraphicsPSO::BindDomainShader(const DomainShader& shader)
		{
			m_Desc.DS.pShaderBytecode = shader.GetBufferPointer();
			m_Desc.DS.BytecodeLength = shader.GetSize();

		}
		void GraphicsPSO::BindGeometryShader(const GeometryShader& shader)
		{

			m_Desc.GS.pShaderBytecode = shader.GetBufferPointer();
			m_Desc.GS.BytecodeLength = shader.GetSize();
		}
		void GraphicsPSO::BindPixelShader(const PixelShader& shader)
		{

			m_Desc.PS.pShaderBytecode = shader.GetBufferPointer();
			m_Desc.PS.BytecodeLength = shader.GetSize();

		}
		void GraphicsPSO::BindRootSignature(ComPtr<ID3D12RootSignature> rootSig)
		{
			m_RootSig = rootSig;
			m_Desc.pRootSignature = m_RootSig.Get();
		}
		void GraphicsPSO::BindRootSignature(RootSignature& rootSig)
		{
			m_RootSig = rootSig.GetD3DRootSignature();
			m_Desc.pRootSignature = rootSig.GetD3DRootSignature();
		}
		void GraphicsPSO::Finalize()
		{
			assert(m_Desc.pRootSignature != nullptr);

			m_Desc.InputLayout.pInputElementDescs = nullptr;
			uint32_t hash_code = (uint32_t)Hash::HashState(&m_Desc);
			hash_code = (uint32_t)Hash::HashState(m_InputLayout.get(), m_Desc.InputLayout.NumElements, hash_code);
			m_Desc.InputLayout.pInputElementDescs = m_InputLayout.get();



			ID3D12PipelineState** PSORef = nullptr;
			bool firstCompile = false;
			{
				static mutex s_HashMapMutex;
				lock_guard<mutex> CS(s_HashMapMutex);

				auto iter = s_GraphicsPSOHashMap.find(hash_code);
				if (iter == s_GraphicsPSOHashMap.end())
				{
					firstCompile = true;
					PSORef = s_GraphicsPSOHashMap[hash_code].GetAddressOf();
				}
				else
					PSORef = iter->second.GetAddressOf();
			}
			if (firstCompile)
			{
				auto device = GraphicsDevice::GetPointer()->GetD3DDevice();
				HRESULT hr = device->CreateGraphicsPipelineState(&m_Desc, IID_PPV_ARGS(m_PSO.GetAddressOf()));
				assert(SUCCEEDED(hr) && "failed Create Graphics_PSO");

				s_GraphicsPSOHashMap[hash_code].Attach(m_PSO.Get());
			}
			else
			{
				while (*PSORef == nullptr)
					this_thread::yield();
				m_PSO = *PSORef;
			}
		}



		ComputePSO::ComputePSO()
		{
			ZeroMemory(&m_Desc, sizeof(m_Desc));
			m_Desc.NodeMask = 0;
		}
		void ComputePSO::BindRootSignature(ComPtr<ID3D12RootSignature> rootSig)
		{
			m_RootSig = rootSig;
			m_Desc.pRootSignature = m_RootSig.Get();
		}
		void ComputePSO::BindComputeShader(const ComputeShader& shader)
		{
			m_Desc.CS.pShaderBytecode = shader.GetBufferPointer();
			m_Desc.CS.BytecodeLength = shader.GetSize();
		}
		void ComputePSO::Finalize()
		{
			assert(m_Desc.pRootSignature != nullptr);

			size_t HashCode = Hash::HashState(&m_Desc);

			ID3D12PipelineState** PSORef = nullptr;
			bool firstCompile = false;
			{
				static mutex s_HashMapMutex;
				lock_guard<mutex> CS(s_HashMapMutex);
				auto iter = s_ComputePSOHashMap.find(HashCode);

				// Reserve space so the next inquiry will find that someone got here first.
				if (iter == s_ComputePSOHashMap.end())
				{
					firstCompile = true;
					PSORef = s_ComputePSOHashMap[HashCode].GetAddressOf();
				}
				else
					PSORef = iter->second.GetAddressOf();
			}

			if (firstCompile)
			{
				auto device = GraphicsDevice::GetPointer()->GetD3DDevice();
				HRESULT hr = device->CreateComputePipelineState(&m_Desc, IID_PPV_ARGS(m_PSO.GetAddressOf()));
				assert(SUCCEEDED(hr) && "failed Create Compute_PSO");
				s_ComputePSOHashMap[HashCode].Attach(m_PSO.Get());
			}
			else
			{
				while (*PSORef == nullptr)
					this_thread::yield();
				m_PSO = *PSORef;
			}
		}
	}
}