#include"stdafx.h"
#include"TestWin.h"
#include"DxCommon/GameTimer.h"
#include"Data/JGMaterial.h"
#include"ResourceManagement/ResourceExtracter.h"
#include"Data/Debug/DebugScreen.h"
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
	SceneConfig setting;
	setting.Width = 1920;
	setting.Height = 1080;
	setting.hWnd = MainWnd();
	setting.FarZ = 10000.0f;
	core = new JGRenderCore;
	core->CreateCore();
	Init = true;
	
	// 씬 생성
	scene = core->CreateScene("TestScene", setting);
	
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
	GridMat->SetTexture(ETextureSlot::Diffuse, L"../Contents/Engine/Textures/bricks2.dds");
	GridMat->SetTexture(ETextureSlot::Normal, L"../Contents/Engine/Textures/bricks2_nmap.dds");
	IF_MaterialData GridMatData = GridMat->AddData("GridMatData");
	GridMatData->SetDiffuseAlbedo(1.0f, 1.0f, 1.0f, 1.0f);
	GridMatData->SetFresnelR0(0.25f, 0.25f, 0.25f);
	GridMatData->SetRoughness(0.95f);
	
	std::string AnimKey = scene->AddAnimation("../Contents/JGUser/mixamo.com.jganimation");
	Desc.Name = "AnimMat";
	IF_Material AnimMat = scene->AddMaterial(Desc);
	IF_MaterialData AnimMatData = AnimMat->AddData("AnimMatData");
	AnimMatData->SetDiffuseAlbedo(1.0f, 1.0f, 1.0f, 1.0f);
	AnimMatData->SetRoughness(0.5f);
	AnimMatData->SetMetalic(1.0f);
	IF_MaterialData AnimMatData2 = AnimMat->AddData("AnimMatData2");
	AnimMatData2->SetDiffuseAlbedo(1.0f, 1.0f, 1.0f, 1.0f);
	AnimMatData2->SetRoughness(0.5f);
	AnimMatData2->SetMetalic(0.0f);

	Desc.Name = "TestMat";
	IF_Material TestMat = scene->AddMaterial(Desc);

	IF_MaterialData TestMatData[10][10];
	for (int i = 0; i < 10; ++i)
	{
		for (int j = 0; j < 10; ++j)
		{
			TestMatData[i][j] = TestMat->AddData("TestMatData" + std::to_string(i) + std::to_string(j));
			TestMatData[i][j]->SetDiffuseAlbedo(1.0f, 0.0f, 0.0f, 1.0f);
			TestMatData[i][j]->SetFresnelR0(1.0f, 1.0f, 1.0f);
			TestMatData[i][j]->SetRoughness((float)i /10.0f);
			TestMatData[i][j]->SetMetalic((float)j / 10);
		}
	}

	// 메시 추가
	IF_StaticMesh gridMesh = scene->AddStaticMesh("GridMesh");
	gridMesh->AddGridArg("Grid", 500.0f, 500.0f, 300, 300);
	gridMesh->AddSphereArg("TestSphere", 10.0f, 3);
	IF_SkeletalMesh animMesh = scene->AddSkeletalMesh("AnimMesh");
	animMesh->AddSkeletalMeshArg("../Contents/JGUser/Beta_Surface.jgskeletalmesh");
	animMesh->AddSkeletalMeshArg("../Contents/JGUser/Beta_Joints.jgskeletalmesh");
	// 오브젝트 추가
	IF_Object GridObj = scene->CreateObject("GridObj", GridMat, GridMatData, gridMesh, "Grid");
	IF_Object AnimObj = scene->CreateObject("AnimObj", AnimMat, AnimMatData2, animMesh, "../Contents/JGUser/Beta_Surface.jgskeletalmesh", E_IF_ObjType::Dynamic);
	AnimObj->SetLocation(0.0f, 0.0f, -200.0f);
	AnimObj->SetRotation(0.0f, 0.0f, 0.0f);
	AnimObj->SetAnimation(AnimKey);

	IF_Object AnimObj2 = scene->CreateObject("AnimObj2", AnimMat, AnimMatData, animMesh, "../Contents/JGUser/Beta_Joints.jgskeletalmesh", E_IF_ObjType::Dynamic);
	AnimObj->AttachTo(AnimObj2);
	AnimObj2->SetAnimation(AnimKey);

	scene->DebugBox(AnimObj, { 1.0f,0.0f,1.0f }, 0.5f);
	scene->DebugBox(AnimObj2, { 1.0f,1.0f,1.0f }, 0.5f);

	IF_Object TestObj[10][10];
	for (int i = 0; i < 10; ++i)
	{
		for (int j = 0; j < 10; ++j)
		{
			TestObj[i][j] = scene->CreateObject("TestObj" + std::to_string(i) + std::to_string(j),
				TestMat, TestMatData[i][j], gridMesh, "TestSphere");
			TestObj[i][j]->SetLocation(i * 25.0f - 125.0f, j * 25.0f + 25.0f, 0.0f);
			scene->DebugBox(TestObj[i][j], { 0.0f,0.0f,0.0f }, 0.5f);
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