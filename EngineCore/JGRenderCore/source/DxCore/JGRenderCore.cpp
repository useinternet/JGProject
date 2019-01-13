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
	return true;
}
void JGRenderCore::Update(const GameTimer& gt)
{
	Test->OffsetRotation(0.0f, gt.DeltaTime() * 5.0f, 0.0f);
	InputCamera(gt);
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
	for (int i = 0; i < 4; ++i)
	{
		SpotLight[i]->OffsetRotation(0.0f, gt.DeltaTime() * 40.0f, 0.0f);
	}
	for (int i = 0; i < 10; ++i)
	{
		Obj[i]->OffsetRotation(0.0, gt.DeltaTime() * 30.0f, 0.0f);
		Objarr[i]->OffsetRotation(0.0, gt.DeltaTime() * 30.0f, 0.0f);
	}
	m_Scene->Update(gt);
}
void JGRenderCore::ScreenReSize(int width, int height)
{
	if (width == 0 || height == 0)
		return;
	m_DxCore->OnResize(width, height);
	m_Camera->SetLens(4 / MathHelper::Pi, m_DxCore->AspectRatio(), m_Camera->GetNearZ(),
		m_Camera->GetFarZ());
	m_Scene->OnReSize(width, height);
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
	DirLight->SetLightColor(0.55f, 0.34f, 0.3f);
	DirLight->SetDirection(-1.0f, -1.0f, 1.0f);



	for (int i = 0; i < 4; ++i)
	{
		PointSwitch[i] = false;
		PointLight[i] = m_Scene->AddLight(ELightType::Point);
		PointLight[i]->SetFalloffStart(0.0f);
		PointLight[i]->SetFalloffEnd(50.0f);
		SpotLight[i] = m_Scene->AddLight(ELightType::Spot);
		SpotLight[i]->SetFalloffStart(0.0f);
		SpotLight[i]->SetFalloffEnd(75.0f);
	
		SpotLight[i]->SetDirection(1.0f, 0.0f, 0.0f);
		switch (i)
		{
		case 0:
			PointLight[i]->SetLightColor(2.0f, 0.0f, 0.0f);
			PointLight[i]->SetLocation(150.0f, 5.0f, 0.0f);
			SpotLight[i]->SetLightColor(0.0f, 2.0f, 0.0f);
			SpotLight[i]->SetLocation(100.0f, 5.0f, 0.0f);
			break;
		case 1:
			PointLight[i]->SetLightColor(0.0f, 2.0f, 0.0f);
			PointLight[i]->SetLocation(0.0f, 5.0f, 150.0f);
			SpotLight[i]->SetLightColor(0.0f, 0.0f, 2.0f);
			SpotLight[i]->SetLocation(0.0f, 5.0f, 100.0f);
			break;
		case 2:
			PointLight[i]->SetLightColor(2.0f, 2.0f, 0.0f);
			PointLight[i]->SetLocation(-150.0f, 5.0f, 0.0f);
			SpotLight[i]->SetLightColor(2.0f, 0.0f, 0.0f);
			SpotLight[i]->SetLocation(-100.0f, 5.0f, 0.0f);
			break;
		case 3:
			PointLight[i]->SetLightColor(0.0f, 2.0f, 2.0f);
			PointLight[i]->SetLocation(0.0f, 5.0f, -150.0f);
			SpotLight[i]->SetLightColor(2.0f, 0.0f, 2.0f);
			SpotLight[i]->SetLocation( 0.0f, 5.0f, -100.0f);
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
	BoxMat->SetFresnelR0(0.02f, 0.02f, 0.02f);
	BoxMat->SetRoughness(1.0f);

	Desc.Name = "SphereMat";
	Desc.bCubMapDynamic = false;
	Desc.bRefraction = false;
	Desc.bReflection = true;
	JGMaterial* SphereMat = m_Scene->AddMaterial(Desc);
	SphereMat->SetTexture(ETextureSlot::Diffuse, L"../Contents/Engine/Textures/bricks2.dds");
	SphereMat->SetTexture(ETextureSlot::Normal, L"../Contents/Engine/Textures/bricks2_nmap.dds");
	SphereMat->SetDiffuseAlbedo(0.0f, 0.0f, 0.0f, 1.0f);
	SphereMat->SetFresnelR0(1.0f, 1.0f, 1.0f);
	SphereMat->SetRoughness(0.0f);
	SphereMat->SetRefractive(1.0f);



	Desc.Name = "WaterMat";
	Desc.bCubMapDynamic = false;
	Desc.bRefraction = true;
	Desc.bReflection = true;
	Desc.ShaderPath = L"../Contents/Engine/Shaders/Water.hlsl";
	JGMaterial* WaterMat = m_Scene->AddMaterial(Desc);
	WaterMat->SetTexture(ETextureSlot::Diffuse, L"../Contents/Engine/Textures/water1.dds");
	WaterMat->SetTexture(ETextureSlot::Normal, L"../Contents/Engine/Textures/waves0.dds");
	WaterMat->SetTexture(ETextureSlot::Custom0, L"../Contents/Engine/Textures/waves1.dds");
	WaterMat->SetDiffuseAlbedo(0.2f, 0.13f, 0.5f, 1.0f);
	WaterMat->SetFresnelR0(1.0f, 1.0f, 1.0f);
	WaterMat->SetRoughness(0.0f);


	// 하늘 박스 머터리얼
	Desc.Name = "SkyMat";
	Desc.bReflection = false;
	Desc.ShaderPath = L"../Contents/Engine/Shaders/Sky.hlsl";
	Desc.Mode = EPSOMode::SKYBOX;

	JGMaterial* SkyMat = m_Scene->AddMaterial(Desc);
	SkyMat->SetDiffuseAlbedo(1.0f, 1.0f, 1.0f, 1.0f);
	SkyMat->SetFresnelR0(0.1f, 0.1f, 0.1f);
	SkyMat->SetRoughness(0.0f);




	JGRCObject* Obj1 = m_Scene->CreateObject();
	Obj1->SetLocation(0.0f, -1.0f, 0.0f);
	Obj1->SetMesh(GroundMesh, "Grid");
	Obj1->SetMaterial(WaterMat);

	JGRCObject* Obj11 = m_Scene->CreateObject();
	Obj11->SetLocation(0.0f, -1.0f, 100.0f);
	Obj11->SetMesh(GroundMesh, "Grid");
	Obj11->SetMaterial(GroundMat2);
	JGRCObject* Obj12 = m_Scene->CreateObject();
	Obj12->SetLocation(0.0f, -1.0f, -100.0f);
	Obj12->SetMesh(GroundMesh, "Grid");
	Obj12->SetMaterial(GroundMat);
	JGRCObject* Obj13 = m_Scene->CreateObject();
	Obj13->SetLocation(100.0f, -1.0f, 0.0f);
	Obj13->SetMesh(GroundMesh, "Grid");
	Obj13->SetMaterial(GroundMat2);
	JGRCObject* Obj14 = m_Scene->CreateObject();
	Obj14->SetLocation(-100.0f, -1.0f, 0.0f);
	Obj14->SetMesh(GroundMesh, "Grid");
	Obj14->SetMaterial(GroundMat);

	JGRCObject* Obj2 = m_Scene->CreateObject();
	Obj2->SetLocation(-10.0f, -1.0f, 0.0f);
	Obj2->SetMesh(GroundMesh, "Box");
	Obj2->SetMaterial(BoxMat);
	Test = Obj2;

	JGRCObject* Obj3 = m_Scene->CreateObject();
	Obj3->SetLocation(-10.0f, 10.0f, 0.0f);
	Obj3->SetMesh(GroundMesh, "Sphere");
	Obj3->SetMaterial(SphereMat);

	JGRCObject* Obj4 = m_Scene->CreateObject();
	Obj4->SetLocation(20.0f, -1.0f, 0.0f);
	Obj4->SetScale(0.5f, 5.0f, 0.5f);
	Obj4->SetMesh(GroundMesh, "Box");
	Obj4->SetMaterial(BoxMat);


	for (int i = 0; i < 10; ++i)
	{
		Obj[i] = m_Scene->CreateObject();
		Obj[i]->SetLocation(i * 30 + -150.0f, 0.0f, 0.0f);
		Obj[i]->SetMesh(GroundMesh, "Box");
		Obj[i]->SetMaterial(BoxMat);
	}
	for (int i = 0; i < 10; ++i)
	{
		Objarr[i] = m_Scene->CreateObject();
		Objarr[i]->SetLocation(0.0f , 0.0f, i * 30 + -150.0f);
		Objarr[i]->SetMesh(GroundMesh, "Box");
		Objarr[i]->SetMaterial(BoxMat);
	}
	JGRCObject* Obj5 = m_Scene->CreateObject();
	Obj5->SetLocation(-20.0f, -1.0f, -10.0f);
	Obj5->SetScale(0.5f, 5.0f, 0.5f);
	Obj5->SetMesh(GroundMesh, "Box");
	Obj5->SetMaterial(BoxMat);

	JGRCObject* Obj6 = m_Scene->CreateObject();
	Obj6->SetLocation(-20.0f, -1.0f, 20.0f);
	Obj6->SetScale(0.5f, 5.0f, 0.5f);
	Obj6->SetMesh(GroundMesh, "Box");
	Obj6->SetMaterial(BoxMat);

	JGRCObject* Obj7 = m_Scene->CreateObject();
	Obj7->SetScale(5000.0f);
	Obj7->SetMesh(SkyMesh, "Sky");
	Obj7->SetMaterial(SkyMat);

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