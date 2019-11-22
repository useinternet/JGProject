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
#include "Object/Shader/ShaderData.h"
#include "Object/Shader/ShaderLib.h"
#include "Object/Shader/ShaderDataType.h"
#include "Object/Shader/ShaderModule.h"
#include "Object/Shader/ShaderModuleManager.h"
#include "Object/ReObject/RenderItem.h"
#include "Object/ReObject/ReMesh.h"
#include "Object/ReObject/ReMaterial.h"
#include "Object/ReObject/ReCamera.h"
#include <d3dcompiler.h>
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
		std::string path = GlobalLinkData::_EngineConfig->InEngine("Shader\\Lib\\ShaderTypeLib.jg");
		m_ShaderLibManager->Save(path);

		path = GlobalLinkData::_EngineConfig->InEngine("Shader\\Module");
		m_ShaderModuleManager->Save(path);
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
		m_ShaderLibManager    = make_shared<ShaderLibManager>();
		m_RenderItemManager   = make_shared<RenderItemManager>();
		m_ShaderModuleManager = make_shared<ShaderModuleManager>();
		RE_LOG_INFO("RenderEngine Init Complete...");
	}
	void RenderEngine::Load()
	{
		std::string path = GlobalLinkData::_EngineConfig->InEngine("Shader\\Lib\\ShaderTypeLib.jg");
		m_ShaderLibManager->Load(path);


		path = GlobalLinkData::_EngineConfig->InEngine("Shader\\Module");
		m_ShaderModuleManager->Load(path);
		//
		m_ShaderModuleManager->CreateGraphicsModule("DefaultStaticGBuffer", EModuleFormat::G_StaticGBuffer);
		auto item1 = m_RenderItemManager->CreateItem("Object1");
		item1->Mesh = ReMesh::Create(EReMeshShape::Box);
		m_ShaderModuleManager->FindGraphicsModule("DefaultStaticGBuffer")->MakePSO(item1->Material->GetPSO());
		item1->Material->GetPSO()->Finalize();


		m_RenderDevice->RegisterGUITexture(m_ShaderModuleManager->FindGraphicsModule("DefaultStaticGBuffer")->GetRenderTargetTexture(0));

	
		m_Cam = make_shared<ReCamera>();
		m_Cam->SetLens(45, 1920, 1080);
		m_Cam->SetPosition(0, 0, -5);
		m_ShaderModuleManager->FindGraphicsModule("DefaultStaticGBuffer")->BindCamera(m_Cam.get());

		//임시 텍스쳐
		//{
		//	CommandQueue texture_que(D3D12_COMMAND_LIST_TYPE_DIRECT);

		//	auto texture_cmdList = texture_que.GetCommandList();
		//	auto config = GlobalLinkData::_EngineConfig;
		//	LoadTexture(config->InEngineW("Textures/bricks3.dds"), texture_cmdList);
		//	LoadTexture(config->InEngineW("Textures/bricks.dds"), texture_cmdList);
		//	LoadTexture(config->InEngineW("Textures/bricks2.dds"), texture_cmdList);
		//	LoadTexture(config->InEngineW("Textures/checkboard.dds"), texture_cmdList);
		//	LoadTexture(config->InEngineW("Textures/ice.dds"), texture_cmdList);
		//	LoadTexture(config->InEngineW("Textures/tile.dds"), texture_cmdList);
		//	LoadTexture(config->InEngineW("Textures/stone.dds"), texture_cmdList);
		//	LoadTexture(config->InEngineW("Textures/WoodCrate01.dds"), texture_cmdList);
		//	LoadTexture(config->InEngineW("Textures/WoodCrate02.dds"), texture_cmdList);
		//	LoadTexture(config->InEngineW("Textures/water1.dds"), texture_cmdList);
		//	texture_que.ExcuteCommandList({ texture_cmdList });
		//	texture_que.Flush();
		//}
	}
	struct GameObject
	{
		JMatrix World;
	};
	struct Camera
	{
		JMatrix View;
		JMatrix Proj;
		JMatrix ViewProj;
		JMatrix InvView;
		JMatrix InvProj;
		JMatrix InvViewProj;
		JVector3 Position;
		float FarZ;
		float NearZ;
		JVector2 ScreenSize;
		float padding;
	};
	void RenderEngine::Update()
	{
		ENGINE_PERFORMANCE_TIMER("Application", "RenderEngine");
		auto gbuffer = m_ShaderModuleManager->FindGraphicsModule("DefaultStaticGBuffer");

		




		m_RenderDevice->SubmitToRender(0, 
			[&](CommandList* commandList) {
//		
			gbuffer->Execute(commandList);


		});
		auto desc = m_ShaderModuleManager->FindGraphicsModule("DefaultStaticGBuffer")->GetRenderTargetTexture(0).GetDesc();
		m_RenderDevice->Update();
	}
	void RenderEngine::OnEvent(Event& e)
	{
		m_EventListener.OnEvent(e);
	}

	void RenderEngine::LoadTexture(const std::wstring& name, CommandList* cmdList)
	{
		ComPtr<ID3D12Resource> d3d_resource;
		ComPtr<ID3D12Resource> upload_resource;
		HRESULT hr = DirectX::CreateDDSTextureFromFile12(m_RenderDevice->GetD3DDevice(),
			cmdList->GetD3DCommandList(), name.c_str(),
			d3d_resource, upload_resource);

		std::string str;
		str.assign(name.begin(), name.end());
		Texture tex(str);
		tex.SetD3DResource(d3d_resource, D3D12_RESOURCE_STATE_COMMON);
		

		Resource upload(str + "_upload");
		upload.SetD3DResource(upload_resource, D3D12_RESOURCE_STATE_COMMON);
		m_UploadResource.push_back(upload);
	}
}

