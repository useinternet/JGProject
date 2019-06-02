#include"stdafx.h"
#include"HelloWindow.h"
#include<ppltasks.h>
using namespace std;
bool HelloWindow::Initialize()
{



	if (!JGWindow::Initialize())
		return false;

	//  그래픽 장치 생성
	{
		m_Device = make_unique<GraphicsDevice>();
		m_Device->Initialize(mhMainWnd, 1920, 1080, false);
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
		m_Device->CreateTextureFromFile("./Textures/bricks.dds", &m_AlbedoTexture);
		m_Device->CreateTextureFromFile("./Textures/bricks_nmap.dds", &m_NormalTexture);
		m_Device->CreateTextureFromFile("./Data/cerberus_A.png", &m_GunTexture_A);
		m_Device->CreateTextureFromFile("./Data/cerberus_N.png", &m_GunTexture_N);
		m_Device->CreateTextureFromFile("./Data/cerberus_R.png", &m_GunTexture_R);
		m_Device->CreateTextureFromFile("./Data/cerberus_M.png", &m_GunTexture_M);
		m_Device->CreateTextureFromFile("./Data/environment.hdr", &m_HDRSkyTexture, true, true);
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
	return true;
}

void HelloWindow::OnResize(int width, int height)
{
	JGWindow::OnResize(width, height);
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
	m_Renderer->Renderer::GBufferOn();
	m_Renderer->SkyBoxRender(m_HDRSkyTexture);
	m_Renderer->GBufferRender(m_Gun);
	m_Renderer->GBufferRender(m_XBot1);
	m_Renderer->GBufferRender(m_XBot2);

	m_Renderer->GBufferOff();



	m_Renderer->RenderEnd();
	m_Device->Present(m_Renderer->GetGBufferRenderTarget()->GetTexture(RenderTarget::Slot0));
}
// Convenience overrides for handling mouse input.
void HelloWindow::OnMouseDown(WPARAM btnState, int x, int y)
{

}
void HelloWindow::OnMouseUp(WPARAM btnState, int x, int y)
{

}
void HelloWindow::OnMouseMove(WPARAM btnState, int x, int y)
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
void HelloWindow::Input()
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
	if (GetKeyState(VK_DOWN) & 0x80000)
	{
		m_Renderer->m_GBufferPass.passCB.padding -= 0.1f;
		m_Renderer->m_GBufferPass.passCB.padding = std::max<float>(0.0f, m_Renderer->m_GBufferPass.passCB.padding);
	}
	if (GetKeyState(VK_UP) & 0x80000)
	{
		m_Renderer->m_GBufferPass.passCB.padding += 0.1f;
		m_Renderer->m_GBufferPass.passCB.padding = std::min<float>(10.0f, m_Renderer->m_GBufferPass.passCB.padding);
	}
	CameraPos.x += (v.x * speed);
	CameraPos.y += (v.y * speed);
	CameraPos.z += (v.z * speed);

	m_Renderer->GetCamera()->SetPosition(CameraPos);
}
ObjectCB HelloWindow::UpdateObjectCB(float x, float y, float z)
{
	ObjectCB objectCB;

	sim_matrix sim_world = DirectX::XMMatrixIdentity();
	sim_matrix sim_trans = DirectX::XMMatrixTranslation(x, y, z);
	static float yaw = 0.0f;
	yaw += 0.01f;

	sim_matrix sim_rotation = DirectX::XMMatrixRotationRollPitchYaw(0.0f, yaw, 0.0f);
	sim_world = DirectX::XMMatrixMultiply(sim_rotation, sim_trans);

	sim_world = DirectX::XMMatrixTranspose(sim_world);

	DirectX::XMStoreFloat4x4(&objectCB.World, sim_world);


	return objectCB;

}