#include"stdafx.h"
#include"TestWin.h"
#include"DxCommon/GameTimer.h"
#include"Data/JGMaterial.h"
#include"ResourceManagement/ResourceExtracter.h"
TestWindow::TestWindow(HINSTANCE hInst) : D3DApp(hInst)
{

}
TestWindow::~TestWindow()
{
	delete core;
}
bool TestWindow::Initialize()
{
	D3DApp::Initialize();
	DxSetting setting;
	setting.Width = 1920;
	setting.Height = 1080;
	setting.hWnd = MainWnd();
	setting.FarZ = 10000.0f;
	core = new JGRenderCore;
	core->Init(setting);
	Init = true;
	
	// 씬 생성
	scene = core->CreateScene("TestScene");
	
	cam = scene->AddCamera();
	cam->SetPosition(0.0f, 30.0f, -100.0f);  	
	cam->SetLens(0.25f* 3.141592f, (float)setting.Width / (float)setting.Height, setting.NearZ, setting.FarZ);
	scene->SetMainCamera(cam);

	// 텍스쳐 추가
	scene->AddTexture(L"../Contents/Engine/Textures/bricks2.dds");
	scene->AddTexture(L"../Contents/Engine/Textures/bricks2_nmap.dds");
	scene->AddTexture(L"../Contents/Engine/Textures/maria_diffuse.dds");
	scene->AddTexture(L"../Contents/Engine/Textures/maria_normal.dds");
	scene->AddTexture(L"../Contents/Engine/Textures/OutputCube.dds", ETextureType::Cube);
	IF_SkyBox SkyBox = scene->CreateSkyBox(L"../Contents/Engine/Textures/OutputCube.dds");
	scene->SetMainSkyBox(SkyBox);
	// 머터리어러 추가
	MaterialDesc Desc;
	Desc.Name = "GridMat";
	Desc.ShaderPath = L"../Contents/Engine/Shaders/Default.hlsl";
	IF_Material GridMat = scene->AddMaterial(Desc);
	GridMat->SetDiffuseAlbedo(1.0f, 1.0f, 1.0f, 1.0f);
	GridMat->SetFresnelR0(0.25f, 0.25f, 0.25f);
	GridMat->SetRoughness(0.95f);
	GridMat->SetTexture(ETextureSlot::Diffuse, L"../Contents/Engine/Textures/bricks2.dds");
	GridMat->SetTexture(ETextureSlot::Normal, L"../Contents/Engine/Textures/bricks2_nmap.dds");
	std::string AnimKey = scene->AddAnimation("../Contents/JGUser/mixamo.com.jganimation");
	Desc.Name = "AnimMat";
	IF_Material AnimMat = scene->AddMaterial(Desc);
	AnimMat->SetDiffuseAlbedo(1.0f, 1.0f, 1.0f, 1.0f);
	AnimMat->SetRoughness(0.5f);
	AnimMat->SetMetalic(1.0f);

	Desc.Name = "AnimMat2";
	IF_Material AnimMat2 = scene->AddMaterial(Desc);
	AnimMat2->SetDiffuseAlbedo(1.0f, 1.0f, 1.0f, 1.0f);
	AnimMat2->SetRoughness(0.0f);
	AnimMat2->SetMetalic(0.0f);


	//AnimMat->SetTexture(ETextureSlot::Diffuse, L"../Contents/Engine/Textures/maria_diffuse.dds");
	//AnimMat->SetTexture(ETextureSlot::Normal, L"../Contents/Engine/Textures/maria_normal.dds");



	IF_Material TestMat[10][10];
	for (int i = 0; i < 10; ++i)
	{
		for (int j = 0; j < 10; ++j)
		{
			Desc.Name = "TestMat" + std::to_string(i) + std::to_string(j);
			TestMat[i][j] = scene->AddMaterial(Desc);
			TestMat[i][j]->SetDiffuseAlbedo(1.0f, 0.0f, 0.0f, 1.0f);
			TestMat[i][j]->SetFresnelR0(1.0f, 1.0f, 1.0f);
			TestMat[i][j]->SetRoughness((float)i /10.0f);
			TestMat[i][j]->SetMetalic((float)j / 10);
		}
	}

	// 메시 추가
	IF_StaticMesh gridMesh = scene->AddStaticMesh();
	gridMesh->AddGridArg("Grid", 500.0f, 500.0f, 300, 300);
	gridMesh->AddSphereArg("TestSphere", 10.0f, 3);
	IF_SkeletalMesh animMesh = scene->AddSkeletalMesh();
	animMesh->AddSkeletalMeshArg("../Contents/JGUser/Beta_Surface.jgskeletalmesh");
	animMesh->AddSkeletalMeshArg("../Contents/JGUser/Beta_Joints.jgskeletalmesh");
	// 오브젝트 추가
	IF_Object GridObj = scene->CreateObject(GridMat, gridMesh, "Grid");
	IF_Object AnimObj = scene->CreateObject(AnimMat2, animMesh, "../Contents/JGUser/Beta_Surface.jgskeletalmesh");
	AnimObj->SetLocation(0.0f, 0.0f, -200.0f);
	AnimObj->SetAnimation(AnimKey);

	IF_Object AnimObj2 = scene->CreateObject(AnimMat, animMesh, "../Contents/JGUser/Beta_Joints.jgskeletalmesh");
	AnimObj2->SetLocation(0.0f, 0.0f, -200.0f);
	AnimObj2->SetAnimation(AnimKey);
	IF_Object TestObj[10][10];
	for (int i = 0; i < 10; ++i)
	{
		for (int j = 0; j < 10; ++j)
		{
			TestObj[i][j] = scene->CreateObject(TestMat[i][j], gridMesh, "TestSphere");
			TestObj[i][j]->SetLocation(i * 25.0f - 125.0f, j * 25.0f + 25.0f, 0.0f);
		}

	}
	IF_DirectionLight dirLight = scene->AddDirLight();
	dirLight->SetLightColor(0.88f, 0.88f, 0.95f);
	dirLight->SetDirection(1.0f, -1.0f, 1.0f);

	core->Build(mTimer);
	return true;
}
void TestWindow::OnResize(int width, int height)
{
	if (Init)
	{
		D3DApp::OnResize(width, height);
		core->ReSize(width, height);
	}

}
void TestWindow::Update(const GameTimer& gt)
{
	InputCamera(gt);
	core->Update(gt);
}
void TestWindow::Draw(const GameTimer& gt)
{
	core->Draw();
}

void TestWindow::OnMouseDown(WPARAM btnState, int x, int y)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;

	SetCapture(mhMainWnd);
}
void TestWindow::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}
void TestWindow::InputCamera(const GameTimer& gt)
{
	if (GetAsyncKeyState('W') & 0x8000)
	{
		cam->Walk(200.0f * gt.DeltaTime());
	}
	if (GetAsyncKeyState('S') & 0x8000)
	{
		cam->Walk(-200.0f * gt.DeltaTime());
	}
	if (GetAsyncKeyState('A') & 0x8000)
	{
		cam->Strafe(-200.0f * gt.DeltaTime());
	}
	if (GetAsyncKeyState('D') & 0x8000)
	{
		cam->Strafe(200.0f * gt.DeltaTime());
	}
	if (GetAsyncKeyState('Q') & 0x8000)
	{
		cam->Up(-200.0f * gt.DeltaTime());
	}
	if (GetAsyncKeyState('E') & 0x8000)
	{
		cam->Up(200.0f * gt.DeltaTime());
	}
}
void TestWindow::OnMouseMove(WPARAM btnState, int x, int y)
{
	if ((btnState & MK_LBUTTON) != 0)
	{
		// Make each pixel correspond to a quarter of a degree.
		float dx = DirectX::XMConvertToRadians(0.25f*static_cast<float>(x - mLastMousePos.x));
		float dy = DirectX::XMConvertToRadians(0.25f*static_cast<float>(y - mLastMousePos.y));

		cam->Pitch(dy);
		cam->RotateY(dx);
	}
	mLastMousePos.x = x;
	mLastMousePos.y = y;
}