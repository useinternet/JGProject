#include"HelloWindow.h"
#include<wincodec.h>
#include<random>
#include"DirectXToolKit/Shader.h"

using namespace std;
using namespace DirectX;
using namespace Common;

bool HelloWindow::Initialize()
{
	if (!JGWindow::Initialize())
		return false;
	DxDevice::CreateDevice(DX12_LOG_PATH);
	Dx2D::Dx2DDevice::CreateDevice();
	m_CommandQueue = DxDevice::GetCommandQueue();
	auto commandList = m_CommandQueue->GetCommandList();

	// 스크린 생성
	m_Screen = make_unique<Screen>();
	m_Screen->InitScreen(mhMainWnd, mClientWidth, mClientHeight);
	// GBuffer 및 씬 생성
	m_GBuffer = make_unique<GBuffer>(mClientWidth, mClientHeight);
	m_Scene = make_unique<Scene>(mClientWidth, mClientHeight);

	// 애니메이션 불러오기
	JgAsset::AssetManager::CreateAnimationTransformsByTimePos(
		m_AnimTransform, GLOBAL_PATH / "Contents/Maria_J_J_Ong.skmesh", GLOBAL_PATH / "Contents/mixamo.com.anim",
		GLOBAL_PATH / "Contents/Bellydancing.skeletal");


	// 카메라 설정
	{
		m_Camera.SetLens(0.25f * JG_PI, (float)mClientWidth, (float)mClientHeight, 1.0f, 100000.0f);
		m_Camera.SetPosition({ 0.0f, 50.0f, -200.0f });
	}
	// 텍스쳐 불러오기
	{
		commandList->LoadTextureFromFile(GLOBAL_TEXTURE_PATH("UV_Test_Pattern.png"), m_Texture, TextureUsage::Albedo);
		commandList->LoadTextureFromFile(GLOBAL_TEXTURE_PATH("grasscube1024.dds"), m_SkyTexture, TextureUsage::Albedo);
	}
	// 오브젝트 설정
	{
		m_Object.SetMesh(
			GeometryGenerator::CreateSkeletalMesh(
				commandList, 
				GLOBAL_PATH / "Contents/Maria_J_J_Ong.skmesh", 
				nullptr));

		m_Object.AddTexture(m_Texture);

		m_Object.Build(
			MAIN_SHADER_PATH,
			{ ShaderStage::Vertex, ShaderStage::Pixel },
			PreparedPSO::Main_Skeletal);


		m_InstanceObj1 = m_Object.AddObjectFragments();



		m_SkyObject.SetMesh(GeometryGenerator::CreateBox(commandList,
			10000.0f, 10000.0f, 10000.0f, 1));
		m_SkyObject.AddCubeTexture(m_SkyTexture);

		m_SkyObject.Build(
			SKY_BOX_SHADER_PATH,
			{ ShaderStage::Vertex, ShaderStage::Pixel },
			PreparedPSO::SkyBox);
		m_SkyObject.AddObjectFragments();

	}


	commandList->Close();

	m_CommandQueue->ExcuteCommandList(commandList);
	m_CommandQueue->Flush();



	auto cmdList = DxDevice::GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COMPUTE)->GetCommandList();

	cmdList->GenerateMipMaps(m_Texture);
	cmdList->Close();
	DxDevice::GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COMPUTE)->ExcuteCommandList(cmdList);
	DxDevice::GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COMPUTE)->Flush();

	return true;
}
void HelloWindow::OnResize(int width, int height)
{
	
	JGWindow::OnResize(width, height);

	m_Screen->Resize(width, height);
	m_Scene->ReSize(width, height);
	m_GBuffer->Resize(width, height);
	m_Camera.SetLens(0.25f * JG_PI, (float)width, (float)height, 1.0f, 100000.0f);
	DxDevice::Flush();
}
void HelloWindow::Update()
{
	static float time = 0.0f;
	static float timepos = 0.0f;
	time += ((1.0f / 60.0f) * 30.0f);
	if (m_AnimTransform.size() - 1 <= timepos)
	{
		timepos = 0.0f;
	}
	if (time >= 1.0f)
	{
		timepos += 1.0f;
		time = 0.0f;

		SkinnedCB skinnedCB;

		for (int i = 0; i < m_AnimTransform[timepos].GetDataSize(); ++i)
		{
			skinnedCB.Offset[i] = m_AnimTransform[timepos].GetData()[i];
		}
		m_Object.SetSkinnedCB(skinnedCB);
	}


	m_Object.Update();
	

	

	m_SkyObject.Update();
	m_PassCB.ViewProj = m_Camera.GetHlslMatrix().Get();
	m_PassCB.ToEye = m_Camera.GetPosition().Get();
	m_CommandQueue->Flush();
}
void HelloWindow::Draw()
{



    m_CommandQueue->Flush();
	auto commandList = m_CommandQueue->GetCommandList();

	m_GBuffer->Draw(commandList, { &m_SkyObject, &m_Object }, m_PassCB);

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
	ObjectFragments* obj;
	random_device rd;
	std::mt19937 rEngine(rd());
	uniform_real_distribution<> dist(0.0f, 1.0f);
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
		obj = m_Object.AddObjectFragments();
		obj->GetMaterial()->Get().SurfaceColor = { (float)dist(rEngine),(float)dist(rEngine),(float)dist(rEngine) };
		obj->GetLocation().SetX(m_OffsetLocation + 100);
		m_InstanceArray.push_back(obj);
		m_OffsetLocation += 100;
		break;
	case 'J':
		if (m_InstanceArray.empty())
			break;
		obj = m_InstanceArray[m_InstanceArray.size() - 1];
		m_Object.RemoveObjectFragments(obj);
		m_OffsetLocation -= 100;
		m_InstanceArray.pop_back();
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