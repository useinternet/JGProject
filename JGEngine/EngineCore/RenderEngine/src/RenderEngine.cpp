#include "pch.h"
#include "RenderEngine.h"
#include "Object/DxObject/Fence.h"
#include "Object/DxObject/DescriptorAllocator.h"
#include "Object/DxObject/DescriptorHandle.h"
#include "Object/DxObject/RenderDevice.h"
#include "Object/DxObject/CommandQueue.h"
#include "Object/DxObject/CommandList.h"
#include "Object/DxObject/Resource.h"
#include "DxUtill.h"
#include "GUI.h"
#include "Object/ReObject/RenderTarget.h"
#include "Object/DxObject/Shader.h"
#include "Object/DxObject/PipelineState.h"
#include "Object/DxObject/RootSignature.h"

#include "Temp/DDSTextureLoader.h"
using namespace std;
using namespace DirectX;
#define BUFFERCOUNT 3
namespace RE
{

	RenderEngine::RenderEngine(const GlobalLinkStream& stream) :
		EngineCore(stream)
	{
	
	}
	RenderEngine::~RenderEngine()
	{

	}
	void RenderEngine::Init(HWND hWnd, int width, int height, const std::shared_ptr<GUI>& bind_gui)
	{
		RenderDeviceDesc desc;
		desc.width = width;
		desc.height = height;
		desc.bufferCount = 3;
		desc.hWnd = hWnd;
		desc.format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.gui = bind_gui;
		m_RenderDevice = make_shared<RenderDevice>(desc);
		m_EventListener.m_RenderEngine = this;
		m_EventListener.m_RenderDevice = m_RenderDevice.get();
		RE_LOG_INFO("RenderEngine Init Complete...");


		m_RenderItem.Rootsignature = make_shared<RootSignature>(2);
		m_RenderItem.Rootsignature->InitParam(0).InitAsConstantBufferView(0);
		m_RenderItem.Rootsignature->InitParam(1).PushAsDescriptorRange(
			D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 10, 0);
		m_RenderItem.Rootsignature->InitParam(1).InitAsDescriptorTable();
		m_RenderItem.Rootsignature->AddStaticSampler(
			CD3DX12_STATIC_SAMPLER_DESC(0,
				D3D12_FILTER_ANISOTROPIC,
				D3D12_TEXTURE_ADDRESS_MODE_WRAP,
				D3D12_TEXTURE_ADDRESS_MODE_WRAP,
				D3D12_TEXTURE_ADDRESS_MODE_WRAP));
		m_RenderItem.Rootsignature->Finalize();

		CreateBox(1.0f, 1.0f, 1.0f);

		std::vector<D3D12_INPUT_ELEMENT_DESC> InputLayouts =
		{
			 { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		     { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		     { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		     { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 36, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
		};



		std::string shadercode = R"(
	    cbuffer cbPerObject : register(b0)
		{
            float4x4 gWorld;
			float4x4 gViewProj;
            uint gFrame;
            float3 gpadding;
		};

		struct VertexIn
		{
			float3 PosL : POSITION;
            float3 Normal : NORMAL;
            float3 Tangent : TANGENT;
			float2 TexC : TEXCOORD;
		};

		struct VertexOut
		{
			float4 PosH : SV_POSITION;
            float3 NormalW : NORMAL;
            float3 TangentW : TANGENT;
			float2 TexC : TEXCOORD;
		};

		VertexOut VS(VertexIn vin)
		{
			VertexOut vout;

			// Transform to homogeneous clip space.
            float4 posW = mul(float4(vin.PosL, 1.0f), gWorld);
			vout.PosH = mul(posW, gViewProj);

			// Just pass vertex color into the pixel shader.
			vout.TexC = vin.TexC;
            vout.NormalW = float3(0.0f,0.0f,0.0f);
            vout.TangentW = float3(0.0f,0.0f,0.0f);
			return vout;
		} 


Texture2D  gtexture[10] : register(t0);
SamplerState  gsampler  : register(s0);
float4 PS(VertexOut pin) : SV_Target
{
    uint index = gFrame / 60;
    float4 color = gtexture[index].Sample(gsampler, pin.TexC);
    // 
    return color;
}
       )";

		//VertexShader v_shader;
		m_RenderItem.VertexShader = make_shared<VertexShader>();
		m_RenderItem.VertexShader->Set(shadercode);
		//PixelShader  p_shader;
		m_RenderItem.PixelShader = make_shared<PixelShader>();
		m_RenderItem.PixelShader->Set(shadercode);
		ShaderCompiler v_shader_Compiler(m_RenderItem.VertexShader->GetType());
		ShaderCompiler p_shader_Compiler(m_RenderItem.PixelShader->GetType());
		v_shader_Compiler.Flags1 = p_shader_Compiler.Flags1 = D3DCOMPILE_DEBUG;
		m_RenderDevice->CompileShader(*m_RenderItem.VertexShader, v_shader_Compiler);
		m_RenderDevice->CompileShader(*m_RenderItem.PixelShader, p_shader_Compiler);



		m_RenderItem.PSO = make_shared<GraphicsPipelineState>();

		m_RenderItem.PSO->SetInputLayout(InputLayouts);
		m_RenderItem.PSO->BindShader(*m_RenderItem.VertexShader);
		m_RenderItem.PSO->BindShader(*m_RenderItem.PixelShader);
		m_RenderItem.PSO->SetRenderTargetFormat({ 0 }, { DXGI_FORMAT_R8G8B8A8_UNORM });
		m_RenderItem.PSO->SetDepthStencilFormat(DXGI_FORMAT_D24_UNORM_S8_UINT);
		m_RenderItem.PSO->SetRootSignature(*m_RenderItem.Rootsignature);
		m_RenderItem.PSO->Finalize();

		 JMatrix view = JMatrix::LookAtLH({ 0.0f, 0.0f, -5.0f }, { 0.0f,0.0f,1.0f }, { 0.0f,1.0f,0.0f });
		 JMatrix proj = JMatrix::PerspectiveFovLH(0.25f * 3.141592f,
			 (float)1920 / (float)1080, 1.0f, 1000.0f);
		
		 {
			 m_RenderItem.contants1.ViewProj = JMatrix::Transpose(view * proj);
		 }
		 {
			 m_RenderItem.contants2.ViewProj = JMatrix::Transpose(view * proj);
			 m_RenderItem.contants2.World = JMatrix::Transpose(JMatrix::Translation({ 1.5f,0.0f,0.0f }));
			 m_RenderItem.contants2.Frame = 120;
		 }
		 {
			 m_RenderItem.contants3.ViewProj = JMatrix::Transpose(view * proj);
			 m_RenderItem.contants3.World = JMatrix::Transpose(JMatrix::Translation({ -1.5f,0.0f,0.0f }));
			 m_RenderItem.contants3.Frame = 240;
		 }




		m_RenderItem.width = 1920;
		m_RenderItem.height = 1080;


		D3D12_CLEAR_VALUE rtv_clear = {};
		rtv_clear.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		rtv_clear.Color[0] = 0.0f;
		rtv_clear.Color[1] = 0.0f;
		rtv_clear.Color[2] = 0.0f;
		rtv_clear.Color[3] = 0.0f;
		Texture texture("FinalScene");
		texture.CreateResource(
			CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R8G8B8A8_UNORM,
				1920 ,1080, 1, 1, 1, 0,
				D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET), 
			&rtv_clear);
		m_RenderDevice->RegisterGUITexture(texture);


		D3D12_CLEAR_VALUE dsv_clear = {};
		dsv_clear.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		dsv_clear.DepthStencil.Depth = 1.0f;
		dsv_clear.DepthStencil.Stencil = 0;

		Texture depth_texture;
		depth_texture.CreateResource(
			CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D24_UNORM_S8_UINT,
				1920, 1080, 1, 1, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL),
			&dsv_clear);



		m_RenderItem.rendertarget = make_shared<RenderTarget>();
		m_RenderItem.rendertarget->BindTexture(0, texture);
		m_RenderItem.rendertarget->BindDepthTexture(depth_texture);


		//임시 텍스쳐
		{
			CommandQueue texture_que(D3D12_COMMAND_LIST_TYPE_DIRECT);
			
			auto texture_cmdList = texture_que.GetCommandList();
			LoadTexture(L"./Textures/bricks3.dds", texture_cmdList);
			LoadTexture(L"./Textures/bricks.dds", texture_cmdList);
			LoadTexture(L"./Textures/bricks2.dds", texture_cmdList);
			LoadTexture(L"./Textures/checkboard.dds", texture_cmdList);
			LoadTexture(L"./Textures/ice.dds", texture_cmdList);
			LoadTexture(L"./Textures/tile.dds", texture_cmdList);
			LoadTexture(L"./Textures/stone.dds", texture_cmdList);
			LoadTexture(L"./Textures/WoodCrate01.dds", texture_cmdList);
			LoadTexture(L"./Textures/WoodCrate02.dds", texture_cmdList);
			LoadTexture(L"./Textures/water1.dds", texture_cmdList);

			texture_que.ExcuteCommandList({ texture_cmdList });
			texture_que.Flush();
		}
	}
	void RenderEngine::Update()
	{
		m_RenderItem.contants1.Frame++;
		if (m_RenderItem.contants1.Frame >= 600)
		{
			m_RenderItem.contants1.Frame = 0;
		}
		m_RenderItem.contants2.Frame++;
		if (m_RenderItem.contants2.Frame >= 600)
		{
			m_RenderItem.contants2.Frame = 0;
		}
		m_RenderItem.contants3.Frame++;
		if (m_RenderItem.contants3.Frame >= 600)
		{
			m_RenderItem.contants3.Frame = 0;
		}

		m_RenderDevice->SubmitToRender(0, 
			[&](CommandList* commandList) {

			Viewport viewport;
			viewport.Set((float)m_RenderItem.width, (float)m_RenderItem.height);
			ScissorRect rect;
			rect.Set(m_RenderItem.width, m_RenderItem.height);

			commandList->SetViewport(viewport);
			commandList->SetScissorRect(rect);
			commandList->SetGraphicsRootSignature(*m_RenderItem.Rootsignature);
			commandList->SetPipelineState(*m_RenderItem.PSO);
			

			commandList->ClearRenderTarget(*m_RenderItem.rendertarget);
			commandList->SetRenderTarget(*m_RenderItem.rendertarget);

			{
				commandList->BindGraphicsDynamicConstantBuffer(
					0, m_RenderItem.contants1);
				commandList->BindSRV(1, m_RenderItem.textures);
				commandList->BindDynamicVertexBuffer(
					0, m_RenderItem.vertices);
				commandList->BindDynamicIndexBuffer(m_RenderItem.indices);
				commandList->SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

				commandList->DrawIndexed((uint32_t)m_RenderItem.indices.size());
			}
			{
				commandList->BindGraphicsDynamicConstantBuffer(
					0, m_RenderItem.contants2);
				commandList->BindSRV(1, m_RenderItem.textures);
				commandList->BindDynamicVertexBuffer(
					0, m_RenderItem.vertices);
				commandList->BindDynamicIndexBuffer(m_RenderItem.indices);
				commandList->SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

				commandList->DrawIndexed((uint32_t)m_RenderItem.indices.size());


			}
			{
				commandList->BindGraphicsDynamicConstantBuffer(
					0, m_RenderItem.contants3);
				commandList->BindSRV(1, m_RenderItem.textures);
				commandList->BindDynamicVertexBuffer(
					0, m_RenderItem.vertices);
				commandList->BindDynamicIndexBuffer(m_RenderItem.indices);
				commandList->SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

				commandList->DrawIndexed((uint32_t)m_RenderItem.indices.size());
			}

		});

		m_RenderDevice->Update();
	}
	void RenderEngine::OnEvent(Event& e)
	{
		m_EventListener.OnEvent(e);
	}
	void RenderEngine::CreateBox(float  width , float height, float depth)
	{
		Vertex v[24];

		float w2 = 0.5f * width;
		float h2 = 0.5f * height;
		float d2 = 0.5f * depth;

		// Fill in the front face vertex data.
		v[0] = Vertex(-w2, -h2, -d2 , 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
		v[1] = Vertex(-w2, +h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
		v[2] = Vertex(+w2, +h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
		v[3] = Vertex(+w2, -h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

		// Fill in the back face vertex data.
		v[4] = Vertex(-w2, -h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
		v[5] = Vertex(+w2, -h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
		v[6] = Vertex(+w2, +h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
		v[7] = Vertex(-w2, +h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);

		// Fill in the top face vertex data.
		v[8] = Vertex(-w2, +h2, -d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
		v[9] = Vertex(-w2, +h2, +d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
		v[10] = Vertex(+w2, +h2, +d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
		v[11] = Vertex(+w2, +h2, -d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

		// Fill in the bottom face vertex data.
		v[12] = Vertex(-w2, -h2, -d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
		v[13] = Vertex(+w2, -h2, -d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
		v[14] = Vertex(+w2, -h2, +d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
		v[15] = Vertex(-w2, -h2, +d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);

		// Fill in the left face vertex data.
		v[16] = Vertex(-w2, -h2, +d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
		v[17] = Vertex(-w2, +h2, +d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
		v[18] = Vertex(-w2, +h2, -d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
		v[19] = Vertex(-w2, -h2, -d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

		// Fill in the right face vertex data.
		v[20] = Vertex(+w2, -h2, -d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
		v[21] = Vertex(+w2, +h2, -d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
		v[22] = Vertex(+w2, +h2, +d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f);
		v[23] = Vertex(+w2, -h2, +d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);

		for (uint32_t i = 0; i < 24; ++i)
		{
			m_RenderItem.vertices.push_back(v[i]);
		}
		//
		// Create the indices.
		//

		uint32_t i[36];

		// Fill in the front face index data
		i[0] = 0; i[1] = 1; i[2] = 2;
		i[3] = 0; i[4] = 2; i[5] = 3;

		// Fill in the back face index data
		i[6] = 4; i[7] = 5; i[8] = 6;
		i[9] = 4; i[10] = 6; i[11] = 7;

		// Fill in the top face index data
		i[12] = 8; i[13] = 9; i[14] = 10;
		i[15] = 8; i[16] = 10; i[17] = 11;

		// Fill in the bottom face index data
		i[18] = 12; i[19] = 13; i[20] = 14;
		i[21] = 12; i[22] = 14; i[23] = 15;

		// Fill in the left face index data
		i[24] = 16; i[25] = 17; i[26] = 18;
		i[27] = 16; i[28] = 18; i[29] = 19;

		// Fill in the right face index data
		i[30] = 20; i[31] = 21; i[32] = 22;
		i[33] = 20; i[34] = 22; i[35] = 23;


		for (uint32_t k = 0; k < 36; ++k)
		{
			m_RenderItem.indices.push_back(i[k]);
		}
	}
	void RenderEngine::LoadTexture(const std::wstring& name, CommandList* cmdList)
	{
		ComPtr<ID3D12Resource> d3d_resource;
		ComPtr<ID3D12Resource> upload_resource;
		DirectX::CreateDDSTextureFromFile12(m_RenderDevice->GetD3DDevice(),
			cmdList->GetD3DCommandList(), name.c_str(),
			d3d_resource, upload_resource);
		Texture tex;
		tex.SetD3DResource(d3d_resource, D3D12_RESOURCE_STATE_COMMON);
		m_RenderItem.textures.push_back(move(tex));
		

		Resource upload;
		upload.SetD3DResource(upload_resource, D3D12_RESOURCE_STATE_COMMON);
		m_UploadResource.push_back(upload);
	}
}

