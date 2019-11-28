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
#include "Object/DxObject/DxScreen.h"
#include "Object/DxObject/TextureManager.h"
#include <d3dcompiler.h>
using namespace std;
using namespace DirectX;

namespace RE
{
	RenderEngine* g_RenderEngine = nullptr;
	RenderEngine::RenderEngine(const GlobalLinkStream& stream) :
		EngineCore(stream)
	{
		g_RenderEngine = this;
	}
	RenderEngine::~RenderEngine()
	{
		std::string path = GlobalLinkData::_EngineConfig->InEngine("Shader\\Lib\\ShaderTypeLib.jg");
		m_ShaderLibManager->Save(path);

		path = GlobalLinkData::_EngineConfig->InEngine("Shader\\Module");
		m_ShaderModuleManager->Save(path);
	}
	void RenderEngine::Init(HWND hWnd, int width, int height)
	{
		RenderDeviceDesc desc;
		desc.gui = nullptr;
		m_RenderDevice = make_shared<RenderDevice>(desc);
		m_ShaderLibManager    = make_shared<ShaderLibManager>();
		m_RenderItemManager   = make_shared<RenderItemManager>();
		m_ShaderModuleManager = make_shared<ShaderModuleManager>();
		m_TextureManager      = make_shared<TextureManager>();
		m_MaterialManager     = make_shared<ReMaterialManager>();
		RE_LOG_INFO("RenderEngine Init Complete...");

		m_MainScreen = m_RenderDevice->CreateDxScreen(hWnd, width, height);
	}
	void RenderEngine::Load()
	{
		std::string path = GlobalLinkData::_EngineConfig->InEngine("Shader\\Lib\\ShaderTypeLib.jg");
		m_ShaderLibManager->Load(path);
		

		path = GlobalLinkData::_EngineConfig->InEngine("Shader\\Module");
		m_ShaderModuleManager->Load(path);


		path = GlobalLinkData::_EngineConfig->InEngine("Textures");
		m_TextureManager->InitLoad(path);


		//
		m_ShaderModuleManager->CreateGraphicsModule("DefaultStaticGBuffer", EModuleFormat::G_StaticGBuffer);
		m_ShaderModuleManager->CreateGraphicsModule("JGUI", EModuleFormat::G_GUI);

		RenderItem::BindShaderModuleByUsage(ERenderItemUsage::Static3D, "DefaultStaticGBuffer");
		RenderItem::BindShaderModuleByUsage(ERenderItemUsage::GUI, "JGUI");


		GUIModule    = m_ShaderModuleManager->FindGraphicsModule("JGUI");
		StaticModule = m_ShaderModuleManager->FindGraphicsModule("DefaultStaticGBuffer");

		// 머터리얼 등록
		{
			ReMaterial mat("TestMaterial", ERenderItemUsage::GUI);
			mat.AddFloat4InMaterialCB("TestFloat4", { 1.0f,1.0f,1.0f,1.0f });
			mat.SetCode(R"(

    Output output;


    // Next Material Code;

    float3 color = diffuse.Sample(AnisotropicSampler,  input.TexC).rgb;
    float3 color2 = diffuse2.Sample(AnisotropicSampler, input.TexC).rgb;

    //color = color * d;
    output.Screen = TestFloat4;


    return output;

)");

			auto desc = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
			desc.DepthEnable = false;
			mat.SetDepthStencilState(desc);
			mat.AddTexture("diffuse2", "ice.dds");
			mat.AddTexture("diffuse", "bricks.dds");
			mat.Compile();
			ReMaterialManager::RegisterMaterial("TextMaterial", mat);

		}
		
		// 아이템 생성
		{
			item = m_RenderItemManager->CreateGUIItem("Object1");
			item->SetMesh(ReGuiMesh::CreateEmptyRect(100,100));
			item->SetMaterial("TextMaterial");
			item->GetMaterial()->SetValueAsFloat4("TestValue", JVector4(1.0f,1.0f,1.0f,1.0f));
			instance = item->AddInstance();
		}



	
		auto desc = m_MainScreen->GetDesc();
		m_Cam = make_shared<ReCamera>();
		m_Cam->SetLens(45, (float)desc.width, (float)desc.height);
		m_Cam->SetPosition(0, 0, -10);
		m_Cam->ConvertOrthographic();
		GUIModule->BindCamera(m_Cam.get());
	}

	void RenderEngine::Update()
	{
		ENGINE_PERFORMANCE_TIMER("Application", "RenderEngine");

		static int count = 0;
		count++;
		if ((count % 100) == 1)
		{
			std::string sss[5] = { "bricks.dds","bricks2.dds","bricks3.dds", "grass.dds", "ice.dds" };

			int num = rand() % 5;
			item->GetMaterial()->SetTexture("diffuse", sss[num]);
			count = 1;

			float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
			float g = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
			float b = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
			item->GetMaterial()->SetValueAsFloat4("TestFloat4", { r , g, b,1.0f });
		}
		auto shader_module = GUIModule;
		m_RenderDevice->SubmitToRender(0, 
			[&](CommandList* commandList) {
//	S
			
			shader_module->Execute(commandList);


		});
		m_MainScreen->SetFinalTexture(GUIModule->GetRenderTargetTexture(0));
		m_RenderDevice->Update();
	}
	void RenderEngine::OnEvent(Event& e)
	{
	
	}




	DxScreen* RenderEngine::CreateDxScreen(HWND hWnd, uint32_t width, uint32_t height, DXGI_FORMAT format)
	{
		return g_RenderEngine->m_RenderDevice->CreateDxScreen(hWnd, width, height, format);
	}
	void RenderEngine::DestroyDxScreen(HWND hWnd)
	{
		g_RenderEngine->m_RenderDevice->DestroyDxScreen(hWnd);
	}
	void RenderEngine::DestroyDxScreen(DxScreen* screen)
	{
		g_RenderEngine->m_RenderDevice->DestroyDxScreen(screen);
	}
	DxScreen* RenderEngine::FindDxScreen(HWND hWnd)
	{
		return g_RenderEngine->m_RenderDevice->FindDxScreen(hWnd);
	}
}

