#include"stdafx.h"
#include"HelloWindow.h"
#include<ppltasks.h>
using namespace std;
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);


bool HelloWindow::Initialize()
{



	if (!JGWindow::Initialize())
		return false;
	GraphicsDevice::UIEvents e;
	// UI 이벤트 지정
	{
		e.initEvent = [this](HWND hWnd, GraphicsDevice* device) {
			IMGUI_CHECKVERSION();
			ImGui::CreateContext();
			ImGuiIO& io = ImGui::GetIO(); (void)io;
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
			io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

			ImGui::StyleColorsDark();
			ImGuiStyle& style = ImGui::GetStyle();
			if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
			{
				style.WindowRounding = 0.0f;
				style.Colors[ImGuiCol_WindowBg].w = 1.0f;
			}

			this->m_FontGPU = device->UIGPUAllcoate();
			ImGui_ImplWin32_Init(hWnd);
			ImGui_ImplDX12_Init(device->GetD3DDevice(), 3, device->GetBackBufferFormat(),
				this->m_FontGPU.CPU(), this->m_FontGPU.GPU());
		};


		e.destroyEvent = []() {
			ImGui_ImplDX12_Shutdown();
			ImGui_ImplWin32_Shutdown();
			ImGui::DestroyContext();

		};
		e.newFrameEvent = []() {
			ImGui_ImplDX12_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();
		};
		e.renderEvent = [this](GraphicsCommander* commander) {
			//m_Device->
			this->UI();
			ImGui::Render();
			ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commander->GetD3DCommandList());
		};

	}
	//  그래픽 장치 생성
	{
		m_Device = make_unique<GraphicsDevice>();
		m_Device->Initialize(mhMainWnd, mClientWidth, mClientHeight, false, &e);
		m_Device->SetShaderDirPath(L"../GraphicsRenderer/src/Shader/HLSL/");
	}
	// 렌더러 생성및 설정
	{
		m_Renderer = m_Device->GetRenderer();
		m_Renderer->Initialize(1920, 1080);
		CameraPos = { 0.0f, 200.0f, -200.0f };
		m_Renderer->GetCamera()->SetPosition(CameraPos);
		pitch = 30.0f;
		m_Renderer->GetCamera()->RotationPitch(pitch);
	}
	// 텍스쳐
	{
		m_SkyPack.resize(6);
		m_Device->CreateTextureFromFile("./Textures/bricks.dds", &m_AlbedoTexture);
		m_Device->CreateTextureFromFile("./Textures/bricks_nmap.dds", &m_NormalTexture);
		m_Device->CreateTextureFromFile("./Data/cerberus_A.png", &m_GunTexture_A);
		m_Device->CreateTextureFromFile("./Data/cerberus_N.png", &m_GunTexture_N);
		m_Device->CreateTextureFromFile("./Data/cerberus_R.png", &m_GunTexture_R);
		m_Device->CreateTextureFromFile("./Data/cerberus_M.png", &m_GunTexture_M);
		m_Device->CreateTextureFromFile("./Data/environment.hdr", &m_SkyPack[0].sky, true, true);
		m_Device->CreateTextureFromFile("./Data/HDR_029_Sky_Cloudy_Ref.hdr", &m_SkyPack[1].sky, true, true);
		m_Device->CreateTextureFromFile("./Data/hdri_sky_01_sample.hdr", &m_SkyPack[2].sky, true, true);
		m_Device->CreateTextureFromFile("./Data/grace-new.hdr", &m_SkyPack[3].sky, true, true);
		m_Device->CreateTextureFromFile("./Data/doge2.hdr", &m_SkyPack[4].sky, true, true);
		m_Device->CreateTextureFromFile("./Data/uffizi-large.hdr", &m_SkyPack[5].sky, true, true);
	}
	// 오브젝트 인스턴스 정의
	{
		m_Gun.AddInstance(ObjectCB());
		m_XBot1.AddInstance(ObjectCB());
		m_XBot2.AddInstance(ObjectCB());
	}
	// 텍스쳐 바인딩
	{
		m_XBot1.BindTexture(RenderObject::ALBEDO, m_AlbedoTexture);
		m_XBot1.BindTexture(RenderObject::NORMAL, m_NormalTexture);
		m_XBot2.BindTexture(RenderObject::ALBEDO, m_AlbedoTexture);
		m_XBot2.BindTexture(RenderObject::NORMAL, m_NormalTexture);
		m_Gun.BindTexture(RenderObject::ALBEDO, m_GunTexture_A);
		m_Gun.BindTexture(RenderObject::NORMAL, m_GunTexture_N);
		m_Gun.BindTexture(RenderObject::METALLIC, m_GunTexture_M);
		m_Gun.BindTexture(RenderObject::ROUGHNESS, m_GunTexture_R);
	}
	// 메시 바인딩
	{
		Mesh mesh;
		Mesh gun;
		m_Device->CreateMeshDataFromFile("./xbot.mesh", &mesh);
		m_Device->CreateMeshDataFromFile("./Data/cerberus.mesh", &gun);
			
		m_XBot1.BindMesh(&mesh);
		m_XBot2.BindMesh(&mesh);
		m_Gun.BindMesh(&gun);
	}
	m_Device->ExecuteCommander(GraphicsDevice::GRAPHICS, true);


	
	concurrency::parallel_for(0, (int)m_SkyPack.size(), [&](int i) {
		auto computeCommander = m_Device->GetComputeCommander();
		m_Renderer->BakeIBLTexture(computeCommander, m_SkyPack[i].sky,
			m_SkyPack[i].spMap, m_SkyPack[i].spBrdf, m_SkyPack[i].irrMap);
		m_Device->PushCommander(computeCommander);
	});


	m_Device->ExecuteCommander(GraphicsDevice::COMPUTE, true);


	BindIBLTexture(m_SkyPack[0]);

	auto gbuffertexture = m_Renderer->GetRenderTarget(Renderer::GBUFFER)->GetTexture(RenderTarget::Slot0);
	auto tonemapping = m_Renderer->GetRenderTarget(Renderer::TONEMAPPING)->GetTexture(RenderTarget::Slot0);


	m_GBufferGPU = m_Device->UIGPUAllocateAndRegister(gbuffertexture);
	m_ToneMapGpu = m_Device->UIGPUAllocateAndRegister(tonemapping);


	return true;
}

void HelloWindow::OnResize(int width, int height)
{
	JGWindow::OnResize(width, height);


	if (width != 0 &&  height != 0)
	{
		m_Device->Resize(width, height);
	}
}
void HelloWindow::Update()
{

	Input();
	m_Device->NewFrame();

}
void HelloWindow::Draw()
{
	m_Gun.SetInstance(0, UpdateObjectCB(0, 100, 0));
	m_XBot1.SetInstance(0, UpdateObjectCB(100,0,0));
	m_XBot2.SetInstance(0, UpdateObjectCB(-100,0,0));

	m_Renderer->RenderBegin();


	// GBuffer 그리기
	int skyindex = atoi(m_CurrentItem.c_str());
	m_Renderer->Renderer::GBufferOn();
	BindIBLTexture(m_SkyPack[skyindex]);
	m_Renderer->SkyBoxRender(m_SkyPack[skyindex].spMap);
 
	m_Renderer->GBufferRender(m_Gun);
	m_Renderer->GBufferRender(m_XBot1);
	m_Renderer->GBufferRender(m_XBot2);

	m_Renderer->GBufferOff();
	if (m_isToneMapping)
	{
		m_TextureID = m_ToneMapGpu.GPU().ptr;
		m_Renderer->ToneMapping();
	}
	else
	{
		m_TextureID = m_GBufferGPU.GPU().ptr;
	}

	m_Renderer->RenderEnd();

	m_Device->Present();
}
void HelloWindow::Destroy()
{

}
// Convenience overrides for handling mouse input.
void HelloWindow::OnMouseMove(WPARAM btnState, int x, int y)
{

	if (GetKeyState(VK_RBUTTON) & 0x80000)
	{
		if ((btnState & MK_LBUTTON) != 0)
		{
			float dx = DirectX::XMConvertToRadians(0.25f*static_cast<float>(x - m_LastMouseX));
			float dy = DirectX::XMConvertToRadians(0.25f*static_cast<float>(y - m_LastMouseY));

			pitch += (30.0f * dy);
			yaw += (30.0f * dx);
			m_Renderer->GetCamera()->RotationPitch(pitch);
			m_Renderer->GetCamera()->RotationYaw(yaw);
		}
		m_LastMouseX = x;
		m_LastMouseY = y;
	}

}
LRESULT HelloWindow::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (!(GetKeyState(VK_RBUTTON) & 0x80000))
	{
		if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam))
			return true;
	}


	return JGWindow::MsgProc(hwnd, msg, wParam, lParam);
}
void HelloWindow::Input()
{
	
	if (GetKeyState(VK_RBUTTON) & 0x80000)
	{
		static float speed = 3.0f;
		float3 v = { 0.0f,0.0f,0.0f };


		if (GetKeyState(0x57) & 0x80000) // w
		{
			v = m_Renderer->GetCamera()->GetLookVector();
		}
		if (GetKeyState(0x41) & 0x80000) // a
		{
			v = m_Renderer->GetCamera()->GetRightVector();
			v.x = -v.x;
			v.y = -v.y;
			v.z = -v.z;
		}
		if (GetKeyState(0x53) & 0x80000) // s
		{
			v = m_Renderer->GetCamera()->GetLookVector();
			v.x = -v.x;
			v.y = -v.y;
			v.z = -v.z;
		}
		if (GetKeyState(0x44) & 0x80000) //d
		{
			v = m_Renderer->GetCamera()->GetRightVector();
		}
		if (GetKeyState(0x51) & 0x80000) // q
		{
			CameraPos.y -= speed;
		}
		if (GetKeyState(0x45) & 0x80000) //e
		{
			CameraPos.y += speed;
		}
		if (GetKeyState(VK_UP) & 0x80000) //e
		{

		}
		if (GetKeyState(VK_DOWN) & 0x80000) //e
		{

		}
		CameraPos.x += (v.x * speed);
		CameraPos.y += (v.y * speed);
		CameraPos.z += (v.z * speed);

		m_Renderer->GetCamera()->SetPosition(CameraPos);
	}
	
}
void HelloWindow::UI()
{
	static bool demo = true;
	//ImGui::ShowDemoWindow(&demo);
	std::vector<std::string> itemList;

	for (int i = 0; i < m_SkyPack.size(); ++i)
	{
		itemList.push_back(to_string(i));
	}
	

	if (ImGui::Begin("Setting", &demo))
	{
		ImGui::Checkbox("ToneMapping", &m_isToneMapping);

		if (ImGui::BeginCombo("SkyTexture", m_CurrentItem.c_str()))
		{
			for (int n = 0; n < itemList.size(); n++)
			{
				bool is_selected = (m_CurrentItem == itemList[n]); // You can store your selection however you want, outside or inside your objects
				if (ImGui::Selectable(itemList[n].c_str(), is_selected))
					m_CurrentItem = itemList[n];
					
					if (is_selected)
						ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
			}
			ImGui::EndCombo();
		}
		ImGui::End();
	}

	float aspectWidth = 1920 / 1080;
	float aspectHeight = 1080 / 1920;

	auto resource = m_Renderer->GetTexture()->GetResource();

	
	if (ImGui::Begin("Rendering", &demo, ImGuiWindowFlags_NoMove))
	{
		ImVec2 size = ImGui::GetWindowSize();
		size.y *= 0.95f;
		ImGui::Image((ImTextureID)m_TextureID, size);

		ImGui::End();
	}


}
ObjectCB HelloWindow::UpdateObjectCB(float x, float y, float z)
{
	ObjectCB objectCB;

	sim_matrix sim_world = DirectX::XMMatrixIdentity();
	sim_matrix sim_trans = DirectX::XMMatrixTranslation(x, y, z);
	static float yaw = 0.0f;
	yaw += 0.001f;//

	sim_matrix sim_rotation = DirectX::XMMatrixRotationRollPitchYaw(0.0f, yaw, 0.0f);
	sim_world = DirectX::XMMatrixMultiply(sim_rotation, sim_trans);

	sim_world = DirectX::XMMatrixTranspose(sim_world);

	DirectX::XMStoreFloat4x4(&objectCB.World, sim_world);


	return objectCB;

}

void HelloWindow::BindIBLTexture(SkyTexturePack& t)
{
	m_Gun.BindCubeTexture(RenderObject::SPECULARMAP, t.spMap);
	m_Gun.BindCubeTexture(RenderObject::IRRADIANCEMAP, t.irrMap);
	m_Gun.BindTexture(RenderObject::SPECULARBRDF, t.spBrdf);
	m_XBot1.BindCubeTexture(RenderObject::SPECULARMAP, t.spMap);
	m_XBot1.BindCubeTexture(RenderObject::IRRADIANCEMAP, t.irrMap);
	m_XBot1.BindTexture(RenderObject::SPECULARBRDF, t.spBrdf);

	m_XBot2.BindCubeTexture(RenderObject::SPECULARMAP, t.spMap);
	m_XBot2.BindCubeTexture(RenderObject::IRRADIANCEMAP, t.irrMap);
	m_XBot2.BindTexture(RenderObject::SPECULARBRDF, t.spBrdf);

}