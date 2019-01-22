#include"JGRenderCore.h"
#include"DxCore.h"
#include"Data/Scene.h"
using namespace Microsoft::WRL;
using namespace DirectX;
using namespace JGRC;
using namespace std;

JGRenderCore::JGRenderCore()
{

}
JGRenderCore::~JGRenderCore()
{
}

bool JGRenderCore::Init(const DxSetting& set)
{
	hWnd = set.hWnd;
	m_DxCore = make_unique<DxCore>(set);
	if (!m_DxCore->InitDirect3D())
	{
		// 예외 처리
		return false;
	}
	ThrowIfFailed(m_DxCore->CommandList()->Reset(m_DxCore->CommandAllocator(), nullptr));

	m_Scene = make_unique<Scene>(m_DxCore.get());
	m_Camera = m_Scene->GetMainCamera();
	BuildTextures();
	BuildLandGeometry();
	BuildLight();
	m_Scene->BuildScene();
	
	// 명령 리스트&큐  초기화
	ThrowIfFailed(m_DxCore->CommandList()->Close());
	ID3D12CommandList* cmdsList[] = { m_DxCore->CommandList() };
	m_DxCore->CommandQueue()->ExecuteCommandLists(_countof(cmdsList), cmdsList);


	for (int i = 0; i < 3; ++i)
	{
		m_Scene->Update(GameTimer());
		m_Scene->Draw();
	}


	return true;
}
void JGRenderCore::Update(const GameTimer& gt)
{
	Test->OffsetRotation(0.0f, gt.DeltaTime() * 5.0f, 0.0f);
	InputCamera(gt);
	for (int i = 0; i < 10; ++i)
	{
		Obj[i]->OffsetRotation(0.0, gt.DeltaTime() * 30.0f, 0.0f);
		Objarr[i]->OffsetRotation(0.0, gt.DeltaTime() * 30.0f, 0.0f);
	}

	DirLight->SetRotation(0.0f, gt.TotalTime() * 5, 0.0f);

	for (int i = 0; i < 4; ++i)
	{
		if (i % 2 == 0)
		{
			if (PointLight[i]->GetLocation().x > 150.0f)
				PointSwitch[i] = false;
			else if (PointLight[i]->GetLocation().x < -150.0f)
				PointSwitch[i] = true;


			if (!PointSwitch[i])
				PointLight[i]->OffsetLocation(-gt.DeltaTime() * 20.0f, 0.0f, 0.0f);
			else
				PointLight[i]->OffsetLocation(gt.DeltaTime() * 20.0f, 0.0f, 0.0f);
		}
		else
		{
			if (PointLight[i]->GetLocation().z > 150.0f)
				PointSwitch[i] = false;
			else if (PointLight[i]->GetLocation().z < -150.0f)
				PointSwitch[i] = true;

			if (!PointSwitch[i])
				PointLight[i]->OffsetLocation(0.0f, 0.0f, -gt.DeltaTime() * 20.0f);
			else
				PointLight[i]->OffsetLocation(0.0f, 0.0f, gt.DeltaTime() * 20.0f);
		}
	}
	SpotLight[0]->OffsetRotation(0.0f, gt.DeltaTime() * 40.0f, 0.0f);
	SpotLight[1]->OffsetRotation(0.0f, gt.DeltaTime() * 40.0f, 0.0f);
	SpotLight[2]->OffsetRotation(0.0f, gt.DeltaTime() * 40.0f, 0.0f);
	SpotLight[3]->OffsetRotation(0.0f, gt.DeltaTime() * 40.0f, 0.0f);
	//for (int i = 0; i < 4; ++i)
	//{
	//	SpotLight[i]->OffsetRotation(0.0f, gt.DeltaTime() * 40.0f, 0.0f);
	//}
	m_Scene->Update(gt);
}
void JGRenderCore::ScreenReSize(int width, int height)
{
	if (width == 0 || height == 0)
		return;
	//m_DxCore->OnResize(width, height);
	//m_Camera->SetLens(4 / MathHelper::Pi, m_DxCore->AspectRatio(), m_Camera->GetNearZ(),
	//	m_Camera->GetFarZ());
	//m_Scene->OnReSize(width, height);
}
void JGRenderCore::Draw()
{

	m_Scene->Draw();


}
void JGRenderCore::InputCamera(const GameTimer& gt)
{
	if (GetAsyncKeyState('W') & 0x8000)
	{
		m_Camera->Walk(40.0f * gt.DeltaTime());
	}
	if (GetAsyncKeyState('S') & 0x8000)
	{
		m_Camera->Walk(-40.0f * gt.DeltaTime());
	}
	if (GetAsyncKeyState('A') & 0x8000)
	{
		m_Camera->Strafe(-40.0f * gt.DeltaTime());
	}
	if (GetAsyncKeyState('D') & 0x8000)
	{
		m_Camera->Strafe(40.0f * gt.DeltaTime());
	}
	if (GetAsyncKeyState('Q') & 0x8000)
	{
		m_Camera->Up(-40.0f * gt.DeltaTime());
	}
	if (GetAsyncKeyState('E') & 0x8000)
	{
		m_Camera->Up(40.0f * gt.DeltaTime());
	}
}
void JGRenderCore::BuildTextures()
{
	m_Scene->AddTexture(L"../Contents/Engine/Textures/tile.dds");
	m_Scene->AddTexture(L"../Contents/Engine/Textures/tile_nmap.dds");
	m_Scene->AddTexture(L"../Contents/Engine/Textures/bricks2.dds");
	m_Scene->AddTexture(L"../Contents/Engine/Textures/bricks2_nmap.dds");
	m_Scene->AddTexture(L"../Contents/Engine/Textures/ice.dds");
	m_Scene->AddTexture(L"../Contents/Engine/Textures/water1.dds");
	m_Scene->AddTexture(L"../Contents/Engine/Textures/waves0.dds");
	m_Scene->AddTexture(L"../Contents/Engine/Textures/waves1.dds");
	m_Scene->AddTexture(L"../Contents/Engine/Textures/sunsetcube1024.dds", ETextureType::Cube);
}
void JGRenderCore::BuildLight()
{
	DirLight = m_Scene->AddLight(ELightType::Direction);
	DirLight->SetLightColor(0.7f, 0.75f, 0.9f);
	DirLight->SetDirection(-1.0f, -1.0f, 1.0f);



	for (int i = 0; i < 4; ++i)
	{
		PointSwitch[i] = false;
		PointLight[i] = m_Scene->AddLight(ELightType::Point,ELightExercise::Dynamic);
		PointLight[i]->SetFalloffStart(0.0f);
		PointLight[i]->SetFalloffEnd(100.0f);
		SpotLight[i] = m_Scene->AddLight(ELightType::Spot, ELightExercise::Dynamic);
		SpotLight[i]->SetFalloffStart(50.0f);
		SpotLight[i]->SetFalloffEnd(100.0f);

		SpotLight[i]->SetDirection(1.0f, -1.0f, 0.0f);
		switch (i)
		{
		case 0:
			PointLight[i]->SetLightColor(1.0f, 0.0f, 0.0f);
			PointLight[i]->SetLocation(150.0f, 5.0f, 0.0f);
			SpotLight[i]->SetLightColor(0.0f, 1.0f, 0.0f);
			SpotLight[i]->SetLocation(100.0f, 10.0f, 0.0f);
			break;
		case 1:
			PointLight[i]->SetLightColor(0.0f, 1.0f, 0.0f);
			PointLight[i]->SetLocation(0.0f, 5.0f, 150.0f);
			SpotLight[i]->SetLightColor(0.928f, 0.34f, 1.0f);
			SpotLight[i]->SetLocation(0.0f, 10.0f, 100.0f);
			break;
		case 2:
			PointLight[i]->SetLightColor(1.0f, 1.0f, 0.0f);
			PointLight[i]->SetLocation(-150.0f, 5.0f, 0.0f);
			SpotLight[i]->SetLightColor(1.0f, 1.0f, 1.0f);
			SpotLight[i]->SetLocation(-100.0f, 10.0f, 0.0f);
			break;
		case 3:
			PointLight[i]->SetLightColor(0.0f, 1.0f, 1.0f);
			PointLight[i]->SetLocation(0.0f, 5.0f, -150.0f);
			SpotLight[i]->SetLightColor(1.0f, 0.0f, 1.0f);
			SpotLight[i]->SetLocation(0.0f, 10.0f, -100.0f);
			break;
		}
	}
}
void JGRenderCore::BuildLandGeometry()
{
	JGMesh* GroundMesh = m_Scene->AddMesh();
	GroundMesh->AddBoxArg("Box", 10, 5, 10, 3);
	GroundMesh->AddGridArg("Grid", 100, 100, 100, 100);
	GroundMesh->AddSphereArg("Sphere", 5, 3);
	GroundMesh->AddQuadArg("Debug", 0.0f, 0.0f, 1.0f, 1.0f, 0.0f);
	GroundMesh->AddFbxMeshArg("../Contents/Engine/Meshs/Samba Dancing_binary.fbx");
	//GroundMesh->AddFbxMeshArg("../Contents/Engine/Meshs/SampleSamba.txt");
	//GroundMesh->AddFbxMeshArg("../Contents/Engine/Meshs/Donut2.txt");
	JGMesh* SkyMesh = m_Scene->AddMesh();
	SkyMesh->AddBoxArg("Sky", 1.0f, 1.0f, 1.0f, 1);

	MaterialDesc Desc;
	Desc.Name = "GroundMat";
	Desc.ShaderPath = L"../Contents/Engine/Shaders/Default.hlsl";
	JGMaterial* GroundMat = m_Scene->AddMaterial(Desc);
	GroundMat->SetTexture(ETextureSlot::Diffuse, L"../Contents/Engine/Textures/tile.dds");
	GroundMat->SetTexture(ETextureSlot::Normal, L"../Contents/Engine/Textures/tile_nmap.dds");
	GroundMat->SetDiffuseAlbedo(1.0f, 1.0f, 1.0f, 1.0f);
	GroundMat->SetFresnelR0(1.0f, 1.0f, 1.0f);
	GroundMat->SetRoughness(1.0f);

	Desc.Name = "GroundMat2";
	JGMaterial* GroundMat2 = m_Scene->AddMaterial(Desc);
	GroundMat2->SetTexture(ETextureSlot::Diffuse, L"../Contents/Engine/Textures/bricks2.dds");
	GroundMat2->SetTexture(ETextureSlot::Normal, L"../Contents/Engine/Textures/bricks2_nmap.dds");
	GroundMat2->SetDiffuseAlbedo(1.0f, 1.0f, 1.0f, 1.0f);
	GroundMat2->SetFresnelR0(1.0f, 1.0f, 1.0f);
	GroundMat2->SetRoughness(1.0f);


	Desc.Name = "BoxMat";

	JGMaterial* BoxMat = m_Scene->AddMaterial(Desc);
	BoxMat->SetTexture(ETextureSlot::Diffuse, L"../Contents/Engine/Textures/tile.dds");
	BoxMat->SetTexture(ETextureSlot::Normal, L"../Contents/Engine/Textures/tile_nmap.dds");
	BoxMat->SetDiffuseAlbedo(1.0f, 1.0f, 1.0f, 1.0f);
	BoxMat->SetFresnelR0(1.0f, 1.0f, 1.0f);
	BoxMat->SetRoughness(0.5f);

	Desc.Name = "SphereMat";
	Desc.bCubeMapStatic = true;
	JGMaterial* SphereMat = m_Scene->AddMaterial(Desc);
	SphereMat->SetTexture(ETextureSlot::Diffuse, L"../Contents/Engine/Textures/bricks2.dds");
	SphereMat->SetTexture(ETextureSlot::Normal, L"../Contents/Engine/Textures/bricks2_nmap.dds");
	SphereMat->SetDiffuseAlbedo(1.0f, 0.0f, 0.0f, 1.0f);
	SphereMat->SetFresnelR0(1.0f, 1.0f, 1.0f);
	SphereMat->SetRoughness(0.05f);
	SphereMat->SetRefractive(1.0f);
	SphereMat->SetReflectivity(0.99f);

	Desc.Name = "InstanceMat";
	Desc.bCubeMapStatic = true;
	Desc.Mode = EPSOMode::INSTANCE;
	JGMaterial* InsMat = m_Scene->AddMaterial(Desc);
	InsMat->SetTexture(ETextureSlot::Diffuse, L"../Contents/Engine/Textures/bricks2.dds");
	InsMat->SetTexture(ETextureSlot::Normal, L"../Contents/Engine/Textures/bricks2_nmap.dds");
	InsMat->SetDiffuseAlbedo(1.0f, 0.0f, 0.0f, 1.0f);
	InsMat->SetFresnelR0(1.0f, 1.0f, 1.0f);
	InsMat->SetRoughness(0.05f);
	InsMat->SetRefractive(1.0f);


	JGRCObject* Obj1 = m_Scene->CreateObject(GroundMat, GroundMesh, "Grid");
	Obj1->SetLocation(0.0f, -1.0f, 0.0f);

	JGRCObject* Obj11 = m_Scene->CreateObject(GroundMat2, GroundMesh, "Grid");
	Obj11->SetLocation(0.0f, -1.0f, 100.0f);
;
JGRCObject* Obj12 = m_Scene->CreateObject(GroundMat, GroundMesh, "Grid");
	Obj12->SetLocation(0.0f, -1.0f, -100.0f);

	JGRCObject* Obj13 = m_Scene->CreateObject(GroundMat2, GroundMesh, "Grid");
	Obj13->SetLocation(100.0f, -1.0f, 0.0f);

	JGRCObject* Obj14 = m_Scene->CreateObject(GroundMat, GroundMesh, "Grid");
	Obj14->SetLocation(-100.0f, -1.0f, 0.0f);


	JGRCObject* Obj2 = m_Scene->CreateObject(BoxMat, GroundMesh, "../Contents/Engine/Meshs/Samba Dancing_binary.fbx");
	Obj2->SetLocation(-10.0f, -1.0f, 0.0f);
	Test = Obj2;

	JGRCObject* Obj3 = m_Scene->CreateObject(SphereMat, GroundMesh, "Sphere");
	Obj3->SetLocation(-10.0f, 10.0f, 0.0f);

	JGRCObject* Obj4 = m_Scene->CreateObject(BoxMat, GroundMesh, "Box");
	Obj4->SetLocation(20.0f, -1.0f, 0.0f);
	Obj4->SetScale(0.5f, 5.0f, 0.5f);


	for (int i = 0; i < 10; ++i)
	{
		Obj[i] = m_Scene->CreateObject(BoxMat, GroundMesh, "Box", EObjType::Dynamic);
		Obj[i]->SetLocation(i * 30 + -150.0f, 0.0f, 0.0f);
	}
	for (int i = 0; i < 10; ++i)
	{
		Objarr[i] = m_Scene->CreateObject(BoxMat, GroundMesh,"Box", EObjType::Dynamic);
		Objarr[i]->SetLocation(0.0f , 0.0f, i * 30 + -150.0f);
	}
	JGRCObject* Obj5 = m_Scene->CreateObject(BoxMat, GroundMesh, "Box");
	Obj5->SetLocation(-20.0f, -1.0f, -10.0f);
	Obj5->SetScale(0.5f, 5.0f, 0.5f);

	JGRCObject* Obj6 = m_Scene->CreateObject(BoxMat , GroundMesh, "Box");
	Obj6->SetLocation(-20.0f, -1.0f, 20.0f);
	Obj6->SetScale(0.5f, 5.0f, 0.5f);


	InstanceObject* InsObj = m_Scene->CreateInstanceObject(GroundMesh, "Sphere", InsMat);
	for (int i = 0; i < 10; ++i)
	{
		JGRCObject* InstanceObj1 = InsObj->AddObject();
		InstanceObj1->SetLocation(i * 10.0f, 20.0f, 0.0f);
	}


	JGRCObject* SkyBox = m_Scene->CreateSkyBox(L"../Contents/Engine/Textures/sunsetcube1024.dds");
	m_Scene->SetMainSkyBox(SkyBox);
}
void JGRenderCore::DrawRenderItems()
{
	m_Scene->Draw();
}
void JGRenderCore::UpdateLook(WPARAM btnState, int x, int y, POINT& mLastMousePos)
{
	if ((btnState & MK_LBUTTON) != 0)
	{
		// Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(0.25f*static_cast<float>(x - mLastMousePos.x));
		float dy = XMConvertToRadians(0.25f*static_cast<float>(y - mLastMousePos.y));

		m_Camera->Pitch(dy);
		m_Camera->RotateY(dx);
	}

	mLastMousePos.x = x;
	mLastMousePos.y = y;
}