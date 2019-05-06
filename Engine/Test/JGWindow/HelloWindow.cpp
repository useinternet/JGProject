#include"HelloWindow.h"
#include<wincodec.h>
#include<random>
#include"DirectXToolKit/Shader.h"
#include<DirectXCollision.h>
using namespace std;
using namespace DirectX;
using namespace Common;

bool HelloWindow::Initialize()
{
	if (!JGWindow::Initialize())
		return false;
	DxDevice::CreateDevice();
	Dx2D::Dx2DDevice::CreateDevice(DxDevice::GetDevice(), DxDevice::GetCommandQueue()->Get());
	m_CommandQueue = DxDevice::GetCommandQueue();
	auto commandList = m_CommandQueue->GetCommandList();

	// 스크린 생성
	m_Screen = make_unique<Screen>();
	m_Screen->InitScreen(mhMainWnd, mClientWidth, mClientHeight);
	// GBuffer 및 씬 생성
	m_GBuffer = make_unique<GBuffer>(mClientWidth, mClientHeight);
	m_Scene = make_unique<Scene>(mClientWidth, mClientHeight);

	// 카메라 설정
	{
		m_Camera.SetLens(0.25f * JG_PI, (float)mClientWidth, (float)mClientHeight, 1.0f, 100000.0f);
		m_Camera.SetPosition({ 0.0f, 20, -100.0f });
	}
	// 텍스쳐 불러오기
	{
		commandList->LoadTextureFromFile(GLOBAL_TEXTURE_PATH("UV_Test_Pattern.png"), m_Texture, TextureUsage::Albedo);
		commandList->LoadTextureFromFile(GLOBAL_TEXTURE_PATH("tile.dds"), m_GridTexture, TextureUsage::Albedo);
		commandList->LoadTextureFromFile(GLOBAL_TEXTURE_PATH("tile_nmap.dds"), m_GridNormalTexture, TextureUsage::Normal);
		commandList->LoadTextureFromFile(GLOBAL_TEXTURE_PATH("grasscube1024.dds"), m_SkyTexture, TextureUsage::Albedo);
	}
	// 오브젝트 설정
	{
		m_PassCB.padding.x = 0.0f;
		m_PassCB.padding.y = 0.0f;

		m_Object.SetMesh(GeometryGenerator::CreateSphere(
			commandList, 10, 20, 20));
		//m_Object.AddTexture(m_Texture);

		m_Object.Build(
			GBUFFER_SHADER_PATH,
			{ ShaderStage::Vertex, ShaderStage::Pixel },
			PreparedPSO::Main_Static);
		m_Object.GetObjectFragments(0)->GetMaterial()->Get().Roughness = 0.0f;
		m_Object.GetObjectFragments(0)->GetMaterial()->Get().SurfaceColor = { 1.0f,0.0f,0.0f };


		m_GridObject.SetMesh(GeometryGenerator::CreateGrid(
			commandList, 100, 100, 10, 10));
		m_GridObject.AddTexture(m_GridTexture);
		m_GridObject.AddTexture(m_GridNormalTexture);
		m_GridObject.Build(GBUFFER_SHADER_PATH,
			{ ShaderStage::Vertex, ShaderStage::Pixel },
			PreparedPSO::Main_Static);
		m_GridObject.GetObjectFragments(0)->GetLocation().Set({ 0.0f, -20.0f, 0.0f });


		m_SkyObject.SetMesh(GeometryGenerator::CreateBox(commandList,
			10000.0f, 10000.0f, 10000.0f, 0));
		m_SkyObject.AddCubeTexture(m_SkyTexture);
		m_SkyObject.Build(
			SKY_BOX_SHADER_PATH,
			{ ShaderStage::Vertex, ShaderStage::Pixel },
			PreparedPSO::SkyBox);
	}
	{
		// 큐브 맵 밉맵
		m_CubeMap = make_unique<ReflectionMap>(512, 512, m_Camera.GetFarZ(), m_Camera.GetNearZ(), false , DXGI_FORMAT_R16G16B16A16_FLOAT);
		m_CubeMap->SetPosition(m_Object.GetObjectFragments(0)->GetLocation().Get());

		m_IrradianceMap = make_unique<IrradianceMap>(commandList, 32, 32,m_Camera.GetFarZ(), m_Camera.GetNearZ());
		m_PrefilterMap = make_unique<PrefilterMap>(commandList, 512, 512, m_Camera.GetFarZ(), m_Camera.GetNearZ());
	}
	Update();
	m_CubeMap->Draw(commandList, { &m_SkyObject ,&m_GridObject });
    m_IrradianceMap->Draw(commandList, m_CubeMap->GetTexture());
    m_PrefilterMap->Draw(commandList, m_CubeMap->GetTexture());
	commandList->Close();
	m_CommandQueue->ExcuteCommandList(commandList);
	m_CommandQueue->Flush();


	// GenerateMipMap
	auto cmdList = DxDevice::GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COMPUTE)->GetCommandList();
	cmdList->GenerateMipMaps(m_Texture);
	cmdList->Close();
	DxDevice::GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COMPUTE)->ExcuteCommandList(cmdList);
	DxDevice::GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COMPUTE)->Flush();



	// IrradianceMap
	//commandList = m_CommandQueue->GetCommandList();

	//commandList->Close();
	//m_CommandQueue->ExcuteCommandList(commandList);
	//m_CommandQueue->Flush();





	// 텍스쳐 설정
	m_Object.AddCubeTexture(m_CubeMap->GetTexture());
	m_Object.AddCubeTexture(m_IrradianceMap->GetTexture());
	m_Object.AddCubeTexture(m_PrefilterMap->GetTexture());
	return true;
}
void HelloWindow::OnResize(int width, int height)
{
	if (width == 0 || height == 0)
		return;
	JGWindow::OnResize(width, height);

	m_Screen->Resize(width, height);
	m_Scene->ReSize(width, height);
	m_GBuffer->Resize(width, height);
	m_Camera.SetLens(0.25f * JG_PI, (float)width, (float)height, 1.0f, 1000.0f);
	DxDevice::Flush();
}
void HelloWindow::Update()
{
	
	m_Object.Update();
	m_GridObject.Update();
	m_SkyObject.Update();
	Inverse(m_Camera.GetHlslMatrix());
	m_PassCB.Set(m_Camera);
}
void HelloWindow::Draw()
{
	// 3d 그래픽 Draw
    m_CommandQueue->Flush();
	auto commandList = m_CommandQueue->GetCommandList();

	m_GBuffer->Draw(commandList, { &m_SkyObject, &m_Object ,&m_GridObject }, m_PassCB);
	m_Scene->Draw(commandList, m_GBuffer.get());
	commandList->Close();
    m_CommandQueue->ExcuteCommandList(commandList);
	m_Screen->Present(m_Scene->GetTexture());
}

void HelloWindow::OnMouseDown(WPARAM btnState, int x, int y)
{
	m_PrevMouseX = x;
	m_PrevMouseY = y;

	SetCapture(mhMainWnd);
}
void HelloWindow::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}
void HelloWindow::OnMouseMove(WPARAM btnState, int x, int y)
{
	if ((btnState & MK_LBUTTON) != 0)
	{
		// Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(0.25f*static_cast<float>(x - m_PrevMouseX));
		float dy = XMConvertToRadians(0.25f*static_cast<float>(y - m_PrevMouseY));
		
		m_Camera.RotationPitch(dy * 30.0f);
		m_Camera.RotationYaw(dx * 30.0f);
	}

	m_PrevMouseX = x;
	m_PrevMouseY = y;
}
void HelloWindow::KeyDown(WPARAM wparam) 
{
	switch (wparam)
	{
	case 'W':
		m_Camera.FowardMove(3.0f);
	
		break;

	case 'S':
		m_Camera.FowardMove(-3.0f);
	
		break;

	case 'A':
		m_Camera.RightMove(-3.0f);
		
		break;

	case 'D':
		m_Camera.RightMove(3.0f);

		break;

	case 'E':
		m_Camera.UpMove(3.0f);
		break;

	case 'Q':
		m_Camera.UpMove(-3.0f);
		break;

	case 'H':
		m_PassCB.padding.x -= 0.1f;
		break;
	case 'J':
		m_PassCB.padding.x += 0.1f;
		break;
	case 'K':
		m_PassCB.padding.y -= 0.1f;
		break;
	case 'L':
		m_PassCB.padding.y += 0.1f;
		break;
	case '1':
		m_Scene->DebugModeOn(GBufferTexture::Albedo);
		break;
	case '2':
		m_Scene->DebugModeOn(GBufferTexture::Normal);
		break;
	case '3':
		m_Scene->DebugModeOn(GBufferTexture::Specular);
		break;
	case '4':
		m_Scene->DebugModeOn(GBufferTexture::Depth);
		break;
	case '0':
		m_Scene->DebugModeOff();
	}
	
}

LRESULT HelloWindow::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

	return JGWindow::MsgProc(hwnd, msg, wParam, lParam);
}