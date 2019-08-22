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
		m_Grid.AddInstance(ObjectCB());
	}
	// 텍스쳐 바인딩
	{
		m_XbotMaterial.BindTexture(ETextureSlot::Albedo, m_AlbedoTexture);
		m_XbotMaterial.BindTexture(ETextureSlot::Normal, m_NormalTexture);



		m_GunMaterial.BindTexture(ETextureSlot::Albedo, m_GunTexture_A);
		m_GunMaterial.BindTexture(ETextureSlot::Normal, m_GunTexture_N);
		m_GunMaterial.BindTexture(ETextureSlot::Metallic, m_GunTexture_M);
		m_GunMaterial.BindTexture(ETextureSlot::Roughness, m_GunTexture_R);


	}
	// 메시 바인딩
	{
		Mesh mesh;
		Mesh gun;
		Mesh grid;
		m_Device->CreateMeshDataFromFile("./xbot.mesh", &mesh);
		m_Device->CreateMeshDataFromFile("./Data/cerberus.mesh", &gun);
			
		m_XBot1.BindMesh(&mesh);
		m_XBot2.BindMesh(&mesh);
		m_Gun.BindMesh(&gun);

		auto griddata = GeometryGenerator::CreateGrid(1000.0f, 1000.0f, 100, 100);
		m_Device->CreateMeshDataFromGenerator(griddata, &grid);
		m_Grid.BindMesh(&grid);

	}
	m_Device->ExecuteCommander(GraphicsDevice::GRAPHICS, true);




	for (auto& pack : m_SkyPack)
	{
		auto computeCommander = m_Device->GetComputeCommander();
		m_Renderer->BakeIBLTexture(computeCommander, pack.sky,
			pack.spMap, pack.spBrdf, pack.irrMap);
		m_Device->PushCommander(computeCommander);
	}
	m_Device->ExecuteCommander(GraphicsDevice::COMPUTE, true);


	m_GunMaterial.BindCubeTexture(ETextureSlot::SpecularMap, m_SkyPack[0].spMap);
	m_GunMaterial.BindCubeTexture(ETextureSlot::IrradianceMap, m_SkyPack[0].irrMap);
	m_GunMaterial.BindTexture(ETextureSlot::SpecularBRDF, m_SkyPack[0].spBrdf);
	m_XbotMaterial.BindCubeTexture(ETextureSlot::SpecularMap, m_SkyPack[0].spMap);
	m_XbotMaterial.BindCubeTexture(ETextureSlot::IrradianceMap, m_SkyPack[0].irrMap);
	m_XbotMaterial.BindTexture(ETextureSlot::SpecularBRDF, m_SkyPack[0].spBrdf);
	m_Gun.BindMaterial(&m_GunMaterial);
	m_XBot1.BindMaterial(&m_XbotMaterial);
	m_XBot2.BindMaterial(&m_XbotMaterial);
	m_Grid.BindMaterial(&m_XbotMaterial);

	m_Light = make_unique<PointLight>();
	m_Light->SetPosition(0.0f, 100.0f, -100.0f);

	m_AlbedoGPU = m_Device->UIGPUAllocateAndRegister(m_Renderer->GetGBuffer()->GetTexture(EGBufferSlot::Albedo));
	m_NormalGPU = m_Device->UIGPUAllocateAndRegister(m_Renderer->GetGBuffer()->GetTexture(EGBufferSlot::Normal));
	m_AmbientGPU = m_Device->UIGPUAllocateAndRegister(m_Renderer->GetGBuffer()->GetTexture(EGBufferSlot::Ambient));
	m_MatPropertyGPU = m_Device->UIGPUAllocateAndRegister(m_Renderer->GetGBuffer()->GetTexture(EGBufferSlot::MaterialProperty));
	m_LinearDepthGPU = m_Device->UIGPUAllocateAndRegister(m_Renderer->GetGBuffer()->GetTexture(EGBufferSlot::Depth));
	m_LightingPassGPU = m_Device->UIGPUAllocateAndRegister(m_Renderer->GetLightingPass()->GetTexture());
	//m_ToneMappingGPU = m_Device->UIGPUAllocateAndRegister(m_Renderer->GetToneMapping()->GetTexture());
	//m_ToneMappingGPU = m_Device->UIGPUAllocateAndRegister(m_Light->GetTexture());
	m_ToneMappingGPU = m_Device->UIGPUAllocateAndRegister(m_Renderer->GetShadowMapping()->GetTexture());
	m_TextureID = m_AlbedoGPU.GPU().ptr;




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
	//"Albedo", "Normal", "Ambient", "MatProperty", "LinearDepth"
	if (m_CurrentItem == "Albedo")
	{
		m_TextureID = m_AlbedoGPU.GPU().ptr;
	}
	else if (m_CurrentItem == "Normal")
	{
		m_TextureID = m_NormalGPU.GPU().ptr;
	}
	else if (m_CurrentItem == "Ambient")
	{
		m_TextureID = m_AmbientGPU.GPU().ptr;
	}
	else if (m_CurrentItem == "MatProperty")
	{
		m_TextureID = m_MatPropertyGPU.GPU().ptr;
	}
	else if (m_CurrentItem == "Depth")
	{
		m_TextureID = m_LinearDepthGPU.GPU().ptr;
	}
	else if (m_CurrentItem == "Lighitng")
	{
		m_TextureID =  m_LightingPassGPU.GPU().ptr;
	}
	else if (m_CurrentItem == "ToneMapping")
	{
		m_TextureID = m_ToneMappingGPU.GPU().ptr;
	}
}
void HelloWindow::Draw()
{
	m_Gun.SetInstance(0, UpdateObjectCB(0, 100, 0));
	m_XBot1.SetInstance(0, UpdateObjectCB(100,0,0));
	m_XBot2.SetInstance(0, UpdateObjectCB(-100,0,0));
	m_Grid.SetInstance(0, UpdateObjectCB(0, 0, 0, false));
	BindIBLTexture(m_SkyPack[m_SkyIndex]);
	m_Renderer->PushObjects({ &m_Gun, &m_XBot1, &m_XBot2, &m_Grid });
	m_Renderer->PushLight(m_Light.get());
	m_Renderer->BindSkyTexture(m_SkyPack[m_SkyIndex].sky);
	//m_Renderer->BindSkyTexture(*m_Light->GetTexture());


	m_Renderer->Execute();
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
	std::vector<std::string> itemList = {
		"Albedo", "Normal", "Ambient" , "MatProperty", "Depth", "Lighitng", "ToneMapping"
	};

	std::vector<std::string> skyList = {
		"environment" ,"HDR_029_Sky_Cloudy_Ref", "hdri_sky_01_sample", "grace-new", "doge2","uffizi-large"
	};
	std::vector<std::string> LightList = {
	"DirectionalLight" 
	};

	if (ImGui::Begin("Setting", &demo))
	{
		ImGui::Text("Draw PipeLine");
		if (ImGui::BeginCombo("##draw pipeline", m_CurrentItem.c_str()))
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

		ImGui::Text("SkyTexture");
		if (ImGui::BeginCombo("## skytexture", m_CurrentSky.c_str()))
		{
			for (int n = 0; n < skyList.size(); n++)
			{
				bool is_selected = (m_CurrentSky == skyList[n]); // You can store your selection however you want, outside or inside your objects
				if (ImGui::Selectable(skyList[n].c_str(), is_selected))
				{
					m_SkyIndex = n;
					m_CurrentSky = skyList[n];
				}
				

				if (is_selected)
				{
					ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
				}
					
			}
			ImGui::EndCombo();
		}

		ImGui::NewLine();
		ImGui::Text("*********  Light *********");
		if (ImGui::BeginCombo("## LightCombo", m_CurrentLight.c_str()))
		{
			for (int n = 0; n < LightList.size(); n++)
			{
				bool is_selected = (m_CurrentLight ==  LightList[n]); // You can store your selection however you want, outside or inside your objects
				if (ImGui::Selectable(LightList[n].c_str(), is_selected))
				{
					m_CurrentLight = LightList[n];
				}


				if (is_selected)
				{
					ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
				}

			}
			ImGui::EndCombo();
		}




		if (m_CurrentLight == "DirectionalLight")
		{
			ImGui::Text("Directional Light Setting");
			static float v[3] = { 0.0f,100.0f,-100.0f };
			//static float v[3] = { 0.0f,0.0f,0.0f };
			if (ImGui::DragFloat3("## direction", v, 1.0f, -360.0f, 360.0f))
			{
				m_Light->SetPosition(v[0], v[1], v[2]);
				//m_Light->SetRotation(v[0], v[1], v[2]);
				//m_Light->SetAround(v[0], v[1], v[2]);
			}
			ImGui::Text("LightColor");
			static float color[3] = { 0.7f,0.78f,1.0f };

			if (ImGui::ColorPicker3("##LightColor", color))
			{
				m_Light->SetColor(color[0], color[1], color[2]);
			}
			ImGui::Text("Shadow Bias");
			static float bias = 0.000011f;
			if (ImGui::InputFloat("##Shadow Bios", &bias, 0.000001f, 0.000001f, 7))
			{
				m_Light->SetBias(bias);
			}
		}
		ImGui::End();
	}

	if (ImGui::Begin("Rendering", &demo))
	{
		ImVec2 size = ImGui::GetWindowSize();
		size.y *= 0.95f;
		ImGui::Image((ImTextureID)m_TextureID, size);

		ImGui::End();
	}


}
ObjectCB HelloWindow::UpdateObjectCB(float x, float y, float z, bool rotate)
{
	ObjectCB objectCB;

	sim_matrix sim_world = DirectX::XMMatrixIdentity();
	sim_matrix sim_trans = DirectX::XMMatrixTranslation(x, y, z);
	
	static float yaw = 0.0f;
	yaw += 0.001f;//
	if (rotate)
	{
		sim_matrix sim_rotation = DirectX::XMMatrixRotationRollPitchYaw(0.0f, yaw, 0.0f);
		sim_world = DirectX::XMMatrixMultiply(sim_rotation, sim_trans);
	}


	sim_world = DirectX::XMMatrixTranspose(sim_world);

	DirectX::XMStoreFloat4x4(&objectCB.World, sim_world);


	return objectCB;

}

void HelloWindow::BindIBLTexture(SkyTexturePack& t)
{
	m_Gun.GetMaterial()->BindCubeTexture(ETextureSlot::SpecularMap, t.spMap);
	m_Gun.GetMaterial()->BindCubeTexture(ETextureSlot::IrradianceMap, t.irrMap);
	m_Gun.GetMaterial()->BindTexture(ETextureSlot::SpecularBRDF, t.spBrdf);
	m_XBot1.GetMaterial()->BindCubeTexture(ETextureSlot::SpecularMap, t.spMap);
	m_XBot1.GetMaterial()->BindCubeTexture(ETextureSlot::IrradianceMap, t.irrMap);
	m_XBot1.GetMaterial()->BindTexture(ETextureSlot::SpecularBRDF, t.spBrdf);
	m_XBot2.GetMaterial()->BindCubeTexture(ETextureSlot::SpecularMap, t.spMap);
	m_XBot2.GetMaterial()->BindCubeTexture(ETextureSlot::IrradianceMap, t.irrMap);
	m_XBot2.GetMaterial()->BindTexture(ETextureSlot::SpecularBRDF, t.spBrdf);



}