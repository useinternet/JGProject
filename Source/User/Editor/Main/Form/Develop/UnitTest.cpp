#include "pch.h"
#include "UnitTest.h"
#include "Elements/Element.h"
#include "Components/DrawComponent.h"
#include "Components/Script/Slider.h"
#include "Components/Script/Border.h"
#include "Components/Script/Dropdown.h"
#include "Components/Script/Button.h"
#include "Components/Script/TreeView.h"


using namespace std;
using namespace JGUI;



void UnitTest::Awake()
{
	GraphicsIF::RegisterInVaildTexture(TT("../Source/Resource/Texture/Engine/InvaildTexture.png"));
	SetBackGroundColor(JColor::Black());
	{
		TestScene = CreateElement<Element>(TT("Image"))->CreateComponent<ImageComponent>();
		TestScene->GetTransform()->SetSize(GetTransform()->GetSize());
		TestScene->GetTransform()->SetAnchor(JGUI::AnchorPreset::Full_Stretch);
		TestScene->SetInteractionActive(false);
		for (int i = 0; i < 10; ++i)
		{
			DebugTextLine[i] = CreateElement<Element>(TT("TextLine"))->CreateComponent<TextMeshComponent>();
			DebugTextLine[i]->SetText(TT("TextLine") + to_wstring(i));
			DebugTextLine[i]->GetTransform()->SetAnchor(AnchorPreset::Left_Top);
			DebugTextLine[i]->GetTransform()->SetSize(1920, 20);
			DebugTextLine[i]->GetTransform()->SetLocalLocation(0.0f, i * 20);
		}

	}

	RESceneRef = GraphicsIF::RequestScene(GE::RendererMode::_3D);
	RESceneCamera.SetLens(0.45f * JG_PI, 1920, 1080, 1000, 0.01f);
	RESceneCamera.SetPosition(JVector3(0.0f, 0.0, -0.0f));
	RESceneRef->BindCamera(&RESceneCamera);


	static bool is_initMaterial = false;
	if (!is_initMaterial)
	{
		is_initMaterial = true;
		InitMaterial();
	}

	DebugTextLine[1]->SetText(TT("선택된 오브젝트 : "));

	MapLoader = CreateElement<Element>(TT("MapLoader"))->CreateComponent<MapSaveLoad>();
	MapLoader->unitTest = this;
	MapLoader->GetTransform()->SetAnchor(AnchorPreset::Right_Top);
	MapLoader->GetTransform()->SetPivot(1.0f, 0.0f);
	MapLoader->GetTransform()->SetLocalLocation(0.0f, 10.0f);
	

}
void UnitTest::Start()
{
	TestScene->SetImage(RESceneRef->GetSceneTexture());
	DebugTextLine[4]->SetText(TT("단축키   1 : Inspector    2 : Spawner   3 : WorldOutliner "));


}

void UnitTest::Tick()
{
	if (CameraMode)
	{
		auto tick = GUIIF::GetTick();
		auto currCursorPos = GUIIF::GetCursorPos();

		auto delta = currCursorPos - CursorPos;

		GUILOG_INFO("X : %f  Y : %f", delta.x, delta.y)
		delta = JVector2::Normalize(delta);
		delta *= CameraSensitivity;
	
		static JVector3 angle;

		angle.y += DirectX::XMConvertToRadians(delta.x * tick);
		angle.x += DirectX::XMConvertToRadians(delta.y * tick);
	
		RESceneCamera.SetRotation(JQuaternion::ToQuaternion(angle));

		GUIIF::SetCursorPos(CursorPos);
	}

	auto CamPos = RESceneCamera.GetPosition();
	DebugTextLine[1]->SetText(TT("CameraPos : [ X : ") +
		to_wstring(CamPos.x) + TT("  Y : ") +
		to_wstring(CamPos.y) + TT("  Z : ") + to_wstring(CamPos.z));

	std::wstring fName = TT("컬링 으로 제외된 오브젝트 : ");
	for (auto& unitpair : UnitPool)
	{
		if (!unitpair.second->Dirty) continue;
		auto unit = unitpair.second.get();


		JMatrix T = JMatrix::Translation(unit->Location);
		JMatrix R = JMatrix::Rotation(
			JQuaternion::ToQuaternion(JVector3(
				DirectX::XMConvertToRadians(unit->Rotation.x),
				DirectX::XMConvertToRadians(unit->Rotation.y),
				DirectX::XMConvertToRadians(unit->Rotation.z)
			)));
		JMatrix S = JMatrix::Scaling(unit->Scale);
		JMatrix M = R * T * S;
		unit->Object.world = JMatrix::Transpose(M);
		auto min = M.TransformPoint(unit->BoundingBox.min);
		auto max = M.TransformPoint(unit->BoundingBox.max);
		JBBox test;
		test.min = min;
		test.max = max;
		
		bool result = (RESceneCamera.GetFrustum().IntersectBBox(test));
		if (!result)
		{
			fName += unit->Name + TT(", ");
		}
		
		RESceneRef->PushObjects(unit->submitObjects);
	}
	DebugTextLine[5]->SetText(fName);
	for (auto& lightPair : LightPool)
	{
		auto type = lightPair.second->ptr->GetType();
		auto light = lightPair.second.get();
		switch (type)
		{
		case GE::LightType::Directional:
		{			
			auto l = (GE::DirectionalLight*)lightPair.second->ptr;
		l->GetSDDataRef().diffuse = light->Color;

		JMatrix R = JMatrix::Rotation(
			JQuaternion::ToQuaternion(JVector3(
				DirectX::XMConvertToRadians(light->Rotation.x),
				DirectX::XMConvertToRadians(light->Rotation.y),
				DirectX::XMConvertToRadians(light->Rotation.z)
			)));
		l->GetSDDataRef().dir = JVector3::Normalize(R.TransformVector(JVector3(0, 0, 1)));

		}
			break;
		case GE::LightType::Point:
		{
			auto l = (GE::PointLight*)lightPair.second->ptr;
			l->GetSDDataRef().diffuse = light->Color;
			l->GetSDDataRef().position = light->Location;
			l->GetSDDataRef().att0 = light->att0;
			l->GetSDDataRef().att1 = light->att1;
			l->GetSDDataRef().att2 = light->att2;
			l->GetSDDataRef().inRange = light->inRange;
			l->GetSDDataRef().outRange = light->outRange;
			l->GetSDDataRef().depthBias = 0.000006f;
			JMatrix proj = JMatrix::PerspectiveFovLH(0.5f * JG_PI, 1, 0.1f, light->outRange);
			JMatrix view = JMatrix::Identity();
			l->GetSDDataRef().proj = JMatrix::Transpose(proj);
			// +X
			view = JMatrix::LookAtLH(light->Location, light->Location + JVector3(1, 0, 0), JVector3(0,1,0));
			l->GetSDDataRef().viewProj[0] = JMatrix::Transpose(view * proj);
			// -X
			view = JMatrix::LookAtLH(light->Location, light->Location + JVector3(-1, 0, 0), JVector3(0, 1, 0));
			l->GetSDDataRef().viewProj[1] = JMatrix::Transpose(view * proj);
			//
			// +Y
			view = JMatrix::LookAtLH(light->Location, light->Location + JVector3(0, 1, 0), JVector3(0, 0, -1));
			l->GetSDDataRef().viewProj[2] = JMatrix::Transpose(view * proj);
			// -Y
			view = JMatrix::LookAtLH(light->Location, light->Location + JVector3(0, -1, 0), JVector3(0, 0, 1));
			l->GetSDDataRef().viewProj[3] = JMatrix::Transpose(view * proj);
			// +Z
			view = JMatrix::LookAtLH(light->Location, light->Location + JVector3(0, 0, 1), JVector3(0, 1, 0));
			l->GetSDDataRef().viewProj[4] = JMatrix::Transpose(view * proj);
			// -Z
			view = JMatrix::LookAtLH(light->Location, light->Location + JVector3(0, 0, -1), JVector3(0, 1, 0));
			l->GetSDDataRef().viewProj[5] = JMatrix::Transpose(view * proj);
		}
			break;
		case GE::LightType::Spot:
		{
			auto l = (GE::SpotLight*)lightPair.second->ptr;
			l->GetSDDataRef().diffuse = light->Color;
			l->GetSDDataRef().position = light->Location;
			l->GetSDDataRef().att0 = light->att0;
			l->GetSDDataRef().att1 = light->att1;
			l->GetSDDataRef().att2 = light->att2;
			l->GetSDDataRef().inRange = light->inRange;
			l->GetSDDataRef().outRange = light->outRange;
			l->GetSDDataRef().inCone = cos(Math::ConvertToRadians(light->inCone));
			l->GetSDDataRef().outCone = cos(Math::ConvertToRadians(light->outCone));

			JMatrix R = JMatrix::Rotation(
				JQuaternion::ToQuaternion(JVector3(
					DirectX::XMConvertToRadians(light->Rotation.x),
					DirectX::XMConvertToRadians(light->Rotation.y),
					DirectX::XMConvertToRadians(light->Rotation.z)
				)));
			l->GetSDDataRef().dir = JVector3::Normalize(R.TransformVector(JVector3(0, 0, 1)));



			JVector3 up    = R.TransformVector({ 0.0f,1.0f,0.0f });
			JVector3 look  = R.TransformVector({ 0.0f,0.0f,1.0f });
			JVector3 right = R.TransformVector({ 1.0f,0.0f,0.0f });

			JMatrix view = JMatrix::LookToLH(light->Location, up, right, look);
			//10000, 0.01f
			JMatrix proj = JMatrix::PerspectiveFovLH(l->GetSDDataRef().outCone, 1, 0.1f, light->outRange);
			JMatrix viewProj = view * proj;
			l->GetSDDataRef().viewProj = JMatrix::Transpose(viewProj);
		}


			break;
		}
		RESceneRef->PushLight(lightPair.second->ptr);
	}
	GraphicsIF::DrawCall(RESceneRef);
	DebugTextLine[0]->SetText(TT("FPS : ") + to_wstring(GlobalSharedData::GetEngineTimer()->GetFPS()));

}

void UnitTest::Destroy()
{



}
void UnitTest::OnKeyUp(KeyCode code)
{

	if (code == KeyCode::F1)
	{
		int filter = (int)GraphicsSetting::ShadowFilter;
		filter++;
		filter = filter % (int)EShadowFilter::Count;

		GraphicsSetting::ShadowFilter = (EShadowFilter)filter;
	}

	if (IsDebugMode) return;

	if (code == KeyCode::Esc)
	{
		if (CameraMode) CameraMode = false;
		else CameraMode = true;


		if (CameraMode)
		{
			CursorPos = GUIIF::GetCursorPos();
			GUIIF::CursorVisible(false);
		}
		else
		{
			GUIIF::CursorVisible(true);
		}
		GUIIF::SetCursorPos(CursorPos);

	}


	if (code == KeyCode::Num1)
	{
		if (InspectorForm == nullptr)
		{
			InspectorForm = CreateWindow<Inspector>(TT("Inspector"), (JGUI::WindowFlags)(JGUI::WindowFlag_ChildWindow & (~JGUI::WindowFlag_CloseBt)));
			InspectorForm->unitTest = this;
		}
		else
		{
			DestroyWindow(InspectorForm);
			InspectorForm = nullptr;
		}
	}

	if (code == KeyCode::Num2)
	{
		if (SpawnerForm == nullptr)
		{
			SpawnerForm = CreateWindow<Spawner>(TT("Spawner"), (JGUI::WindowFlags)(JGUI::WindowFlag_ChildWindow & (~JGUI::WindowFlag_CloseBt)));
			SpawnerForm->unitTest = this;
		}
		else
		{
			DestroyWindow(SpawnerForm);
			SpawnerForm = nullptr;
		}
	}
	if (code == KeyCode::Num3)
	{
		if (WorldOutLiner == nullptr)
		{
			WorldOutLiner = CreateWindow<WorldOutliner>(TT("월드"), (JGUI::WindowFlags)(JGUI::WindowFlag_ChildWindow & (~JGUI::WindowFlag_CloseBt)));
			WorldOutLiner->unitTest = this;
		}
		else
		{
			DestroyWindow(WorldOutLiner);
			WorldOutLiner = nullptr;
		}
	}
}
void UnitTest::OnKey(KeyCode code)
{
	auto camPos = RESceneCamera.GetPosition();
	auto tick = GUIIF::GetTick();
	if (code == KeyCode::W)
	{
		camPos += (RESceneCamera.GetLook() * CameraSpeed * tick);
	}
	if (code == KeyCode::S)
	{
		camPos -= (RESceneCamera.GetLook() * CameraSpeed * tick);
	}
	if (code == KeyCode::A)
	{
		camPos -= (RESceneCamera.GetRight() * CameraSpeed * tick);
	}
	if (code == KeyCode::D)
	{
		camPos += (RESceneCamera.GetRight() * CameraSpeed * tick);
	}
	if (code == KeyCode::Q)
	{
		camPos.y -= CameraSpeed * tick;
	}
	if (code == KeyCode::E)
	{
		camPos.y += CameraSpeed * tick;
	}
	RESceneCamera.SetPosition(camPos);
}
void UnitTest::OnMouseClick(KeyCode code)
{
	auto mousePos   = GetMousePos();
	auto windowSize = GetTransform()->GetSize();
	auto camCB      = RESceneCamera.GetCameraCB();
	auto screenSize = JVector2 (camCB.lensWidth, camCB.lensHeight );

	float ratioX = mousePos.x / windowSize.x;
	float ratioY = mousePos.y / windowSize.y;


	JVector2 mouseInScreen = JVector2(ratioX * screenSize.x, ratioY * screenSize.y);
	JVector3 dir = RESceneCamera.ScreenToWorldToRay(JVector3(mouseInScreen.x, mouseInScreen.y, 400));
	DebugTextLine[2]->SetText(TT("ScreenToWorld : [ X : ") +
		to_wstring(dir.x) + TT("  Y : ") +
		to_wstring(dir.y) + TT("  Z : ") + to_wstring(dir.z));

	bool IsIntersection = false;
	auto PrevSelectedUnit = SelectedUnit;
	SelectedUnit = nullptr;
	for (auto& unit : UnitPool)
	{
		JVector3 targetDir = JVector3::Normalize(dir);
		JRay ray;
		ray.dir = targetDir;
		ray.origin = RESceneCamera.GetPosition();



		JRay localRay;
		JMatrix worldMatrix = JMatrix::Transpose(unit.second->Object.world);
		JMatrix invWorld    = JMatrix::Inverse(worldMatrix);

		localRay.dir    = JVector3::Normalize(invWorld.TransformVector(ray.dir));
		localRay.origin = invWorld.TransformPoint(ray.origin);

		IsIntersection = unit.second->BoundingBox.Intersection(localRay);
		if(IsIntersection)
		{
			SelectedLight = nullptr;
			SelectedUnit = unit.second.get();
			DebugTextLine[3]->SetText(TT("선택된 오브젝트 : ") + unit.first);
			break;
		}
	}
	if (!IsIntersection)
	{
		DebugTextLine[3]->SetText(TT("선택된 오브젝트 : "));

	}



	if (PrevSelectedUnit != SelectedUnit)
	{
		if (InspectorForm) InspectorForm->SetUnitInspector(SelectedUnit);

	}

	

}
void UnitTest::OnChar(wchar_t c)
{
	

}
void UnitTest::OnDropItem(const GUIDropItem& item)
{
	int cnt = item.GetDropSourceCount();
	if (cnt > 1) return;
	auto source = item.GetDropSource<wstring>(0);
	if (fs::path(source->Data).extension() != TT(".map")) return;
	if(InspectorForm) InspectorForm->SetUnitInspector(nullptr);
	
	UnitPool.clear();
	LightPool.clear();
	IsDirLight = false;
	MapLoader->Load(source->Data);
	MapLoader->Input->Text->SetText(fs::path(source->Data).filename().wstring());
	if(WorldOutLiner) WorldOutLiner->Reset();
}
void UnitTest::CreateUnit(const std::wstring& name, std::wstring meshName, const std::wstring& initm)
{
	auto unit = make_unique<Unit>();
	unit->Name = name;
	// Mesh
	fs::path meshP(meshName);
	if (meshP.extension() == ".fbx" || meshP.extension() == ".FBX")
	{
		GlobalSharedData::GetAssetManager()->Import(meshP.wstring(), TT("../Source/Resource/Test/Model/"),
			FBX_ImportFlag_ImportMesh, nullptr);
		meshName = meshName.substr(0, meshName.length() - 4) + TT(".mesh");
	}
	if (meshP.extension() == ".mesh")
	{
		meshP = meshName;
		MeshGenerator::LoadFromFile(meshP, unit->staticMeshDatas);
	}
	else return;

	// Material

	auto m = GraphicsIF::GetMaterial(initm);
	if (!m.IsValid()) return;


	// Object
	for (int i = 0; i < unit->staticMeshDatas.size(); ++i)
	{
		unit->materialRef.push_back(GraphicsIF::GetMaterial(initm));

		unit->BoundingBox.min.x = std::min<float>(unit->staticMeshDatas[i].bbox.min.x, unit->BoundingBox.min.x);
		unit->BoundingBox.min.y = std::min<float>(unit->staticMeshDatas[i].bbox.min.y, unit->BoundingBox.min.y);
		unit->BoundingBox.min.z = std::min<float>(unit->staticMeshDatas[i].bbox.min.z, unit->BoundingBox.min.z);

		unit->BoundingBox.max.x = std::max<float>(unit->staticMeshDatas[i].bbox.max.x, unit->BoundingBox.max.x);
		unit->BoundingBox.max.y = std::max<float>(unit->staticMeshDatas[i].bbox.max.y, unit->BoundingBox.max.y);
		unit->BoundingBox.max.z = std::min<float>(unit->staticMeshDatas[i].bbox.max.z, unit->BoundingBox.max.z);

		auto obj = GraphicsIF::RequestSceneObject();
		obj->BindMesh(
			unit->staticMeshDatas[i].v.data(), unit->staticMeshDatas[i].v.size(),
			unit->staticMeshDatas[i].i.data(), unit->staticMeshDatas[i].i.size()
		);
		obj->BindMaterial(
			m.Get()
		);
		obj->BindInstanceData(&unit->Object, 1);
		unit->submitObjects.push_back(obj.Get());
		unit->sceneObjectRef.push_back(move(obj));
	}
	UnitPool[name] = move(unit);
	if (WorldOutLiner)
	{
		TreeItemDesc desc;
		desc.name = name;
		WorldOutLiner->ObjectTreeItem->AddItem(desc);
	}
}

void UnitTest::CreateUnit(const std::wstring& name, std::wstring meshName, const std::vector<std::wstring>& m)
{

	auto unit = make_unique<Unit>();
	unit->Name = name;

	auto meshIter = MeshPool.find(meshName);
	if (meshIter == MeshPool.end())
	{
		fs::path meshP(meshName);
		if (meshP.extension() == ".fbx" || meshP.extension() == ".FBX")
		{
			GlobalSharedData::GetAssetManager()->Import(meshP.wstring(), TT("../Source/Resource/Test/Model/"),
				FBX_ImportFlag_ImportMesh, nullptr);
			meshName = meshName.substr(0, meshName.length() - 4) + TT(".mesh");
		}
		if (meshP.extension() == ".mesh")
		{
			if (MeshGenerator::LoadFromFile(meshP, unit->staticMeshDatas))
			{
				MeshPool[meshName] = unit->staticMeshDatas;
			}
		}
		
	}
	else
	{
		unit->staticMeshDatas = meshIter->second;
	}

	unit->MeshName = meshName;

	
	size_t cnt = unit->staticMeshDatas.size();
	for (int i = 0; i < cnt; ++i)
	{
		if(m.size() <= i) unit->materialRef.push_back(GraphicsIF::GetMaterial(m[m.size() - 1]));
		else 	unit->materialRef.push_back(GraphicsIF::GetMaterial(m[i]));
	
	}
	// Object
	for (int i = 0; i < cnt; ++i)
	{
		unit->BoundingBox.min.x = std::min<float>(unit->staticMeshDatas[i].bbox.min.x, unit->BoundingBox.min.x);
		unit->BoundingBox.min.y = std::min<float>(unit->staticMeshDatas[i].bbox.min.y, unit->BoundingBox.min.y);
		unit->BoundingBox.min.z = std::min<float>(unit->staticMeshDatas[i].bbox.min.z, unit->BoundingBox.min.z);

		unit->BoundingBox.max.x = std::max<float>(unit->staticMeshDatas[i].bbox.max.x, unit->BoundingBox.max.x);
		unit->BoundingBox.max.y = std::max<float>(unit->staticMeshDatas[i].bbox.max.y, unit->BoundingBox.max.y);
		unit->BoundingBox.max.z = std::min<float>(unit->staticMeshDatas[i].bbox.max.z, unit->BoundingBox.max.z);




		auto obj = GraphicsIF::RequestSceneObject();
		obj->BindMesh(
			unit->staticMeshDatas[i].v.data(), unit->staticMeshDatas[i].v.size(),
			unit->staticMeshDatas[i].i.data(), unit->staticMeshDatas[i].i.size()
		);
		obj->BindMaterial(
			unit->materialRef[i].Get()
		);
		obj->BindInstanceData(&unit->Object, 1);
		unit->submitObjects.push_back(obj.Get());
		unit->sceneObjectRef.push_back(move(obj));
	}
	UnitPool[name] = move(unit);
	if (WorldOutLiner)
	{
		TreeItemDesc desc;
		desc.name = name;
		WorldOutLiner->ObjectTreeItem->AddItem(desc);
	}
}

void UnitTest::CreateDirLight(const std::wstring& name)
{
	if (IsDirLight) return;
	IsDirLight = true;


	auto light = make_unique<Light>();
	light->name = name;
	light->dref = GraphicsIF::RequestDirectionalLight();
	light->ptr = light->dref.Get();
	LightPool[name] = move(light);

}

void UnitTest::CreatePointLight(const std::wstring& name)
{
	auto light = make_unique<Light>();
	light->name = name;
	light->pref = GraphicsIF::RequestPointLight();
	light->ptr = light->pref.Get();
	LightPool[name] = move(light);
}

void UnitTest::CreateSpotLight(const std::wstring& name)
{
	auto light = make_unique<Light>();
	light->name = name;
	light->sref = GraphicsIF::RequestSpotLight();
	light->ptr = light->sref.Get();
	LightPool[name] = move(light);
}

UnitTest::Unit* UnitTest::GetUnit(const std::wstring& name)
{
	return UnitPool[name].get();
}

UnitTest::Light* UnitTest::GetLight(const std::wstring& name)
{
	return LightPool[name].get();
}

void UnitTest::InitMaterial()
{
	// Material By Data
	// Default
	{
		GE::MaterialProperty ppt;
		ppt.rendererMode = GE::RendererMode::_3D;
		ppt.blendMode    = GE::BlendMode::Opaque;
		auto m = GraphicsIF::RequestMaterial(TT("Default"), ppt);

		m->DefineVarAsFloat4("Color", JColor::White());
		m->SetCode(R"(
pout.Albedo =Color_Data;
)");
		GraphicsIF::PreCompileMaterial(m.Get());
	}

	// Texture
	{
		GE::MaterialProperty ppt;
		ppt.rendererMode = GE::RendererMode::_3D;
		ppt.blendMode = GE::BlendMode::Opaque;
		auto m = GraphicsIF::RequestMaterial(TT("Texture"), ppt);
		m->DefineVarAsFloat4("Color", JColor::White());
		m->DefineTexture("Albedo", nullptr);
		m->SetCode(R"(
pout.Albedo = Albedo_Texture.Sample(g_LinearSampler, pin.tex).rgba * Color_Data;
)");
		GraphicsIF::PreCompileMaterial(m.Get());
	}
	// NormalMapping
	{
		GE::MaterialProperty ppt;
		ppt.rendererMode = GE::RendererMode::_3D;
		ppt.blendMode = GE::BlendMode::Opaque;
		auto m = GraphicsIF::RequestMaterial(TT("NormalMapping"), ppt);
		m->DefineVarAsFloat4("Color", JColor::White());
		m->DefineTexture("Albedo", nullptr);
		m->DefineTexture("Normal", nullptr);
		m->SetCode(R"(
pout.Albedo     = Albedo_Texture.Sample(g_LinearSampler, pin.tex).rgba * Color_Data;
pout.Normal.xyz = BumpMapping(Normal_Texture.Sample(g_LinearSampler, pin.tex).xyz, pin.normalW, pin.tangentW);
)");
		GraphicsIF::PreCompileMaterial(m.Get());
	}

}

void Inspector::Awake()
{
	GetTransform()->SetSize(400, 800);


}

void Inspector::Start()
{
	auto clientRect = GetClientRect();

	float gap = 10.0f;
	float offsetX = clientRect.left + gap;
	float offsetY = clientRect.top + gap;
	float blockWidth  = clientRect.Width() - (2 * gap);
	float blockHeight = 20.0f;



	DeleteButton = CreateElement<Element>(TT("DeleteBt"))->CreateComponent<Button>();
	DeleteButton->GetOwner()->CreateComponent<ImageComponent>();
	DeleteButton->GetTransform()->SetPivot(1.0f, 0.0f);
	DeleteButton->GetTransform()->SetAnchor(AnchorPreset::Right_Top);
	DeleteButton->GetTransform()->SetLocalLocation(-gap, offsetY);
	auto btTxt = DeleteButton->GetOwner()->CreateElement<Element>(TT("텍스트"))->CreateComponent<TextMeshComponent>();
	btTxt->SetText(TT("삭제"));
	btTxt->GetTransform()->SetSize(DeleteButton->GetTransform()->GetSize());
	btTxt->GetTransform()->SetAnchor(AnchorPreset::Full_Stretch);
	btTxt->SetTextVAlign(TextVAlign_Center);
	btTxt->SetTextHAlign(TextHAlign_Center);
	DeleteButton->BindOnClick([&]()
	{
		if (SelectedUnit)
		{
			std::wstring unitName = SelectedUnit->Name;
			SetUnitInspector(nullptr);
			unitTest->UnitPool.erase(unitName);

		}
		if (SelectedLight)
		{
			if (SelectedLight->ptr->GetType() == GE::LightType::Directional) unitTest->IsDirLight = false;
			std::wstring lightName = SelectedLight->name;
			SetLightInspector(nullptr);
			unitTest->LightPool.erase(lightName);
	



			// 여기 수정
		}
		if (unitTest->WorldOutLiner)
		{
			unitTest->WorldOutLiner->Reset();
		}
	});






	ObjectType = CreateElement<Element>(TT("ObjectType"))->CreateComponent<TextInformation>();
	ObjectType->GetTransform()->SetSize(blockWidth, blockHeight);
	ObjectType->GetTransform()->SetLocalLocation(offsetX, offsetY);
	ObjectType->Name->SetText(TT("오브젝트 유형 : "));
	ObjectType->Contents->SetText(TT("(None)"));
	offsetY += (blockHeight + gap);


	ObjectName = CreateElement<Element>(TT("ObjectType"))->CreateComponent<TextInformation>();
	ObjectName->GetTransform()->SetSize(blockWidth, blockHeight);
	ObjectName->GetTransform()->SetLocalLocation(offsetX, offsetY);
	ObjectName->Name->SetText(TT("오브젝트 이름 : "));
	ObjectName->Contents->SetText(TT("(None)"));
	offsetY += (blockHeight + gap);

	InspectorByType[TT("오브젝트")]         = CreateElement<Element>(TT("오브젝트"));
	InspectorByType[TT("포인트 라이트")]    = CreateElement<Element>(TT("포인트 라이트"));
	InspectorByType[TT("스팟 라이트")]       = CreateElement<Element>(TT("스팟 라이트"));
	InspectorByType[TT("다이렉션얼 라이트")] = CreateElement<Element>(TT("다이렉션얼 라이트"));


	{
	
		Element* ObjectInspector = InspectorByType[TT("오브젝트")];
		ObjectInspector->CreateComponent<ShapeComponent>();
		ObjectInspector->CreateComponent<Border>();
		ObjectInspector->GetTransform()->SetSize(clientRect.Width(), clientRect.bottom - offsetY);
		ObjectInspector->GetTransform()->SetLocalLocation(0.0f, offsetY);



		float OI_OffsetY = 5.0f;
		_ObjectDetail.Location = ObjectInspector->CreateElement< Element>(TT("Location"))->CreateComponent<JVector3Information>();
		_ObjectDetail.Location->GetTransform()->SetSize(blockWidth, blockHeight * 2);
		_ObjectDetail.Location->GetTransform()->SetLocalLocation(offsetX, OI_OffsetY);
		_ObjectDetail.Location->Name->SetText(TT("Location"));
		_ObjectDetail.Location->XName->SetText(TT(" X "));
		_ObjectDetail.Location->YName->SetText(TT(" Y "));
		_ObjectDetail.Location->ZName->SetText(TT(" Z "));
		OI_OffsetY += (blockHeight * 2 + gap);


		_ObjectDetail.Rotation = ObjectInspector->CreateElement< Element>(TT("Rotation"))->CreateComponent<JVector3Information>();
		_ObjectDetail.Rotation->GetTransform()->SetSize(blockWidth, blockHeight * 2);
		_ObjectDetail.Rotation->GetTransform()->SetLocalLocation(offsetX, OI_OffsetY);
		_ObjectDetail.Rotation->Name->SetText(TT("Rotation"));
		_ObjectDetail.Rotation->XName->SetText(TT(" X "));
		_ObjectDetail.Rotation->YName->SetText(TT(" Y "));
		_ObjectDetail.Rotation->ZName->SetText(TT(" Z "));
		OI_OffsetY += (blockHeight * 2 + gap);

		_ObjectDetail.Scale = ObjectInspector->CreateElement<Element>(TT("Scale"))->CreateComponent<JVector3Information>();
		_ObjectDetail.Scale->GetTransform()->SetSize(blockWidth, blockHeight * 2);
		_ObjectDetail.Scale->GetTransform()->SetLocalLocation(offsetX, OI_OffsetY);
		_ObjectDetail.Scale->Name->SetText(TT("Scale"));
		_ObjectDetail.Scale->XName->SetText(TT(" X "));
		_ObjectDetail.Scale->YName->SetText(TT(" Y "));
		_ObjectDetail.Scale->ZName->SetText(TT(" Z "));
		OI_OffsetY += (blockHeight * 2 + gap);



		_ObjectDetail.MaterialSlot = ObjectInspector->CreateElement<Element>(TT("MaterialSlot"))->CreateComponent<DropdownInformation>();
		_ObjectDetail.MaterialSlot->GetTransform()->SetSize(blockWidth, blockHeight);
		_ObjectDetail.MaterialSlot->GetTransform()->SetLocalLocation(offsetX, OI_OffsetY);
		_ObjectDetail.MaterialSlot->Name->SetText(TT("머터리얼 슬롯"));
		OI_OffsetY += (blockHeight + gap);



		//
		// 노멀 매핑을 가정해서 
		_ObjectDetail.MaterialSlotByModel[TT("Default")] = ObjectInspector->CreateElement<Element>(TT("Default"));
		_ObjectDetail.MaterialSlotByModel[TT("Texture")] = ObjectInspector->CreateElement<Element>(TT("Texture"));
		_ObjectDetail.MaterialSlotByModel[TT("NormalMapping")] = ObjectInspector->CreateElement<Element>(TT("NormalMapping"));
		// Material By Data
		auto ObjectInspectorRect = ObjectInspector->GetTransform()->GetLocalRect();
		{
			float D_OffsetY = 15.0f;
			Element* Default = _ObjectDetail.MaterialSlotByModel[TT("Default")];
			Default->CreateComponent<ShapeComponent>();
			Default->CreateComponent<Border>();
			Default->GetTransform()->SetSize(ObjectInspectorRect.Width(), ObjectInspectorRect.Height() - OI_OffsetY);
			Default->GetTransform()->SetLocalLocation(0.0f, OI_OffsetY);

auto MaterialModelName = Default->CreateElement<Element>(TT("MaterialModelName"))->CreateComponent<TextInformation>();
MaterialModelName->GetTransform()->SetSize(blockWidth, blockHeight);
MaterialModelName->GetTransform()->SetLocalLocation(offsetX, D_OffsetY);
MaterialModelName->Name->SetText(TT("머터리얼 모델 : "));
MaterialModelName->Contents->SetText(TT("Default"));
D_OffsetY += (blockHeight + gap);


auto Color = Default->CreateElement<Element>(TT("Color"))->CreateComponent<JVector4Information>();
Color->GetTransform()->SetSize(blockWidth, blockHeight * 2);
Color->GetTransform()->SetLocalLocation(offsetX, D_OffsetY);
Color->Name->SetText(TT("Color"));
Color->XName->SetText(TT(" R ")); Color->YName->SetText(TT(" G ")); Color->ZName->SetText(TT(" B ")); Color->WName->SetText(TT(" A "));
D_OffsetY += (blockHeight * 2 + gap);



Default->SetActive(false);

		}
		{
		float T_OffsetY = 15.0f;
		Element* Texture = _ObjectDetail.MaterialSlotByModel[TT("Texture")];
		Texture->CreateComponent<ShapeComponent>();
		Texture->CreateComponent<Border>();
		Texture->GetTransform()->SetSize(ObjectInspectorRect.Width(), ObjectInspectorRect.Height() - OI_OffsetY);
		Texture->GetTransform()->SetLocalLocation(0.0f, OI_OffsetY);







		auto MaterialModelName = Texture->CreateElement<Element>(TT("MaterialModelName"))->CreateComponent<TextInformation>();
		MaterialModelName->GetTransform()->SetSize(blockWidth, blockHeight);
		MaterialModelName->GetTransform()->SetLocalLocation(offsetX, T_OffsetY);
		MaterialModelName->Name->SetText(TT("머터리얼 모델 : "));
		MaterialModelName->Contents->SetText(TT("Texture"));
		T_OffsetY += (blockHeight + gap);



		auto Color = Texture->CreateElement<Element>(TT("Color"))->CreateComponent<JVector4Information>();
		Color->GetTransform()->SetSize(blockWidth, blockHeight * 2);
		Color->GetTransform()->SetLocalLocation(offsetX, T_OffsetY);
		Color->Name->SetText(TT("Color"));
		Color->XName->SetText(TT(" R ")); Color->YName->SetText(TT(" G ")); Color->ZName->SetText(TT(" B ")); Color->WName->SetText(TT(" A "));
		T_OffsetY += (blockHeight * 2 + gap);



		auto Albedo = Texture->CreateElement<Element>(TT("Albedo"))->CreateComponent<TextureInformation>();
		Albedo->GetTransform()->SetSize(blockWidth, blockHeight * 4);
		Albedo->GetTransform()->SetLocalLocation(offsetX, T_OffsetY);
		Albedo->Name->SetText(TT("Albedo"));
		T_OffsetY += (blockHeight * 4 + gap);
		Texture->SetActive(false);
		}
		{
			float NM_OffsetY = 15.0f;
			Element* NormalMapping = _ObjectDetail.MaterialSlotByModel[TT("NormalMapping")];
			NormalMapping->CreateComponent<ShapeComponent>();
			NormalMapping->CreateComponent<Border>();
			NormalMapping->GetTransform()->SetSize(ObjectInspectorRect.Width(), ObjectInspectorRect.Height() - OI_OffsetY);
			NormalMapping->GetTransform()->SetLocalLocation(0.0f, OI_OffsetY);


			auto MaterialModelName = NormalMapping->CreateElement<Element>(TT("MaterialModelName"))->CreateComponent<TextInformation>();
			MaterialModelName->GetTransform()->SetSize(blockWidth, blockHeight);
			MaterialModelName->GetTransform()->SetLocalLocation(offsetX, NM_OffsetY);
			MaterialModelName->Name->SetText(TT("머터리얼 모델 : "));
			MaterialModelName->Contents->SetText(TT("NormalMapping"));
			NM_OffsetY += (blockHeight + gap);


			auto Color = NormalMapping->CreateElement<Element>(TT("Color"))->CreateComponent<JVector4Information>();
			Color->GetTransform()->SetSize(blockWidth, blockHeight * 2);
			Color->GetTransform()->SetLocalLocation(offsetX, NM_OffsetY);
			Color->Name->SetText(TT("Color"));
			Color->XName->SetText(TT(" R ")); Color->YName->SetText(TT(" G ")); Color->ZName->SetText(TT(" B ")); Color->WName->SetText(TT(" A "));
			NM_OffsetY += (blockHeight * 2 + gap);

			auto Albedo = NormalMapping->CreateElement<Element>(TT("Albedo"))->CreateComponent<TextureInformation>();
			Albedo->GetTransform()->SetSize(blockWidth, blockHeight * 4);
			Albedo->GetTransform()->SetLocalLocation(offsetX, NM_OffsetY);
			Albedo->Name->SetText(TT("Albedo"));
			NM_OffsetY += (blockHeight * 4 + gap);

			auto Normal = NormalMapping->CreateElement<Element>(TT("Normal"))->CreateComponent<TextureInformation>();
			Normal->GetTransform()->SetSize(blockWidth, blockHeight * 4);
			Normal->GetTransform()->SetLocalLocation(offsetX, NM_OffsetY);
			Normal->Name->SetText(TT("Normal"));
			NM_OffsetY += (blockHeight * 4 + gap);

			NormalMapping->SetActive(false);
		}
		ObjectInspector->SetActive(false);
	}
	{
		auto DLInspector = InspectorByType[TT("다이렉션얼 라이트")];
		DLInspector->CreateComponent<ShapeComponent>();
		DLInspector->CreateComponent<Border>();
		DLInspector->GetTransform()->SetSize(clientRect.Width(), clientRect.bottom - offsetY);
		DLInspector->GetTransform()->SetLocalLocation(0.0f, offsetY);
		DLInspector->SetActive(false);


		{
			float DI_OffsetY = 5.0f;
			_DLDetail.Dir = DLInspector->CreateElement<Element>(TT("Directional"))->CreateComponent<JVector3Information>();
			_DLDetail.Dir->GetTransform()->SetSize(blockWidth, blockHeight * 2);
			_DLDetail.Dir->GetTransform()->SetLocalLocation(offsetX, DI_OffsetY);
			_DLDetail.Dir->Name->SetText(TT("Direction"));
			_DLDetail.Dir->XName->SetText(TT(" X "));
			_DLDetail.Dir->YName->SetText(TT(" Y "));
			_DLDetail.Dir->ZName->SetText(TT(" Z "));
			DI_OffsetY += (blockHeight * 2 + gap);


			_DLDetail.Color = DLInspector->CreateElement<Element>(TT("Color"))->CreateComponent<JVector4Information>();
			_DLDetail.Color->GetTransform()->SetSize(blockWidth, blockHeight * 2);
			_DLDetail.Color->GetTransform()->SetLocalLocation(offsetX, DI_OffsetY);
			_DLDetail.Color->Name->SetText(TT("Color"));
			_DLDetail.Color->XName->SetText(TT(" R "));
			_DLDetail.Color->YName->SetText(TT(" G "));
			_DLDetail.Color->ZName->SetText(TT(" B "));
			_DLDetail.Color->WName->SetText(TT(" A "));
			DI_OffsetY += (blockHeight * 2 + gap);
		}



		auto PLInspector = InspectorByType[TT("포인트 라이트")];
		PLInspector->CreateComponent<ShapeComponent>();
		PLInspector->CreateComponent<Border>();
		PLInspector->GetTransform()->SetSize(clientRect.Width(), clientRect.bottom - offsetY);
		PLInspector->GetTransform()->SetLocalLocation(0.0f, offsetY);
		PLInspector->GetTransform()->SetActive(false);
		{
			float PI_OffsetY = 5.0f;
			_PLDetail.Location = PLInspector->CreateElement<Element>(TT("Point"))->CreateComponent<JVector3Information>();
			_PLDetail.Location->GetTransform()->SetSize(blockWidth, blockHeight * 2);
			_PLDetail.Location->GetTransform()->SetLocalLocation(offsetX, PI_OffsetY);
			_PLDetail.Location->Name->SetText(TT("Location"));
			_PLDetail.Location->XName->SetText(TT(" X "));
			_PLDetail.Location->YName->SetText(TT(" Y "));
			_PLDetail.Location->ZName->SetText(TT(" Z "));
			PI_OffsetY += (blockHeight * 2 + gap);


			_PLDetail.Color = PLInspector->CreateElement<Element>(TT("Color"))->CreateComponent<JVector4Information>();
			_PLDetail.Color->GetTransform()->SetSize(blockWidth, blockHeight * 2);
			_PLDetail.Color->GetTransform()->SetLocalLocation(offsetX, PI_OffsetY);
			_PLDetail.Color->Name->SetText(TT("Color"));
			_PLDetail.Color->XName->SetText(TT(" R "));
			_PLDetail.Color->YName->SetText(TT(" G "));
			_PLDetail.Color->ZName->SetText(TT(" B "));
			_PLDetail.Color->WName->SetText(TT(" A "));
			PI_OffsetY += (blockHeight * 2 + gap);


			_PLDetail.att0 = PLInspector->CreateElement<Element>(TT("ATT0"))->CreateComponent<FloatInformation>();
			_PLDetail.att0->GetTransform()->SetSize(blockWidth, blockHeight);
			_PLDetail.att0->GetTransform()->SetLocalLocation(offsetX, PI_OffsetY);
			_PLDetail.att0->Name->SetText(TT("att0"));
			_PLDetail.att0->FloatSlider->SetMinValue(0);
			_PLDetail.att0->FloatSlider->SetMaxValue(1);
			_PLDetail.att0->FloatSlider->SetValue(1.0f);
			PI_OffsetY += (blockHeight + gap);

			_PLDetail.att1 = PLInspector->CreateElement<Element>(TT("ATT1"))->CreateComponent<FloatInformation>();
			_PLDetail.att1->GetTransform()->SetSize(blockWidth, blockHeight);
			_PLDetail.att1->GetTransform()->SetLocalLocation(offsetX, PI_OffsetY);
			_PLDetail.att1->Name->SetText(TT("att1"));
			_PLDetail.att1->FloatSlider->SetMinValue(0);
			_PLDetail.att1->FloatSlider->SetMaxValue(0.01);
			_PLDetail.att1->FloatSlider->SetValue(0);
			PI_OffsetY += (blockHeight + gap);

			_PLDetail.att2 = PLInspector->CreateElement<Element>(TT("ATT2"))->CreateComponent<FloatInformation>();
			_PLDetail.att2->GetTransform()->SetSize(blockWidth, blockHeight);
			_PLDetail.att2->GetTransform()->SetLocalLocation(offsetX, PI_OffsetY);
			_PLDetail.att2->Name->SetText(TT("att2"));
			_PLDetail.att2->FloatSlider->SetMinValue(0);
			_PLDetail.att2->FloatSlider->SetMaxValue(0.0001);
			_PLDetail.att2->FloatSlider->SetValue(0);
			PI_OffsetY += (blockHeight + gap);



			_PLDetail.inRange = PLInspector->CreateElement<Element>(TT("inRange"))->CreateComponent<FloatInformation>();
			_PLDetail.inRange->GetTransform()->SetSize(blockWidth, blockHeight);
			_PLDetail.inRange->GetTransform()->SetLocalLocation(offsetX, PI_OffsetY);
			_PLDetail.inRange->Name->SetText(TT("inRange"));
			_PLDetail.inRange->FloatSlider->SetMinValue(0);
			_PLDetail.inRange->FloatSlider->SetMaxValue(1000);
			_PLDetail.inRange->FloatSlider->SetValue(80);
			PI_OffsetY += (blockHeight + gap);


			_PLDetail.outRange = PLInspector->CreateElement<Element>(TT("outRange"))->CreateComponent<FloatInformation>();
			_PLDetail.outRange->GetTransform()->SetSize(blockWidth, blockHeight);
			_PLDetail.outRange->GetTransform()->SetLocalLocation(offsetX, PI_OffsetY);
			_PLDetail.outRange->Name->SetText(TT("outRange"));
			_PLDetail.outRange->FloatSlider->SetMinValue(0);
			_PLDetail.outRange->FloatSlider->SetMaxValue(1000);
			_PLDetail.outRange->FloatSlider->SetValue(100);
			PI_OffsetY += (blockHeight + gap);


		}


		auto SLInspector = InspectorByType[TT("스팟 라이트")];
		SLInspector->CreateComponent<ShapeComponent>();
		SLInspector->CreateComponent<Border>();
		SLInspector->GetTransform()->SetSize(clientRect.Width(), clientRect.bottom - offsetY);
		SLInspector->GetTransform()->SetLocalLocation(0.0f, offsetY);
		SLInspector->GetTransform()->SetActive(false);
		{
			float SL_OffsetY = 5.0f;
			_SLDetail.Location = SLInspector->CreateElement<Element>(TT("Point"))->CreateComponent<JVector3Information>();
			_SLDetail.Location->GetTransform()->SetSize(blockWidth, blockHeight * 2);
			_SLDetail.Location->GetTransform()->SetLocalLocation(offsetX, SL_OffsetY);
			_SLDetail.Location->Name->SetText(TT("Location"));
			_SLDetail.Location->XName->SetText(TT(" X "));
			_SLDetail.Location->YName->SetText(TT(" Y "));
			_SLDetail.Location->ZName->SetText(TT(" Z "));
			SL_OffsetY += (blockHeight * 2 + gap);



			_SLDetail.Dir = SLInspector->CreateElement<Element>(TT("Dir"))->CreateComponent<JVector3Information>();
			_SLDetail.Dir->GetTransform()->SetSize(blockWidth, blockHeight * 2);
			_SLDetail.Dir->GetTransform()->SetLocalLocation(offsetX, SL_OffsetY);
			_SLDetail.Dir->Name->SetText(TT("Rotation"));
			_SLDetail.Dir->XName->SetText(TT(" X "));
			_SLDetail.Dir->YName->SetText(TT(" Y "));
			_SLDetail.Dir->ZName->SetText(TT(" Z "));
			SL_OffsetY += (blockHeight * 2 + gap);


			_SLDetail.Color = SLInspector->CreateElement<Element>(TT("Color"))->CreateComponent<JVector4Information>();
			_SLDetail.Color->GetTransform()->SetSize(blockWidth, blockHeight * 2);
			_SLDetail.Color->GetTransform()->SetLocalLocation(offsetX, SL_OffsetY);
			_SLDetail.Color->Name->SetText(TT("Color"));
			_SLDetail.Color->XName->SetText(TT(" R "));
			_SLDetail.Color->YName->SetText(TT(" G "));
			_SLDetail.Color->ZName->SetText(TT(" B "));
			_SLDetail.Color->WName->SetText(TT(" A "));
			SL_OffsetY += (blockHeight * 2 + gap);


			_SLDetail.att0 = SLInspector->CreateElement<Element>(TT("ATT0"))->CreateComponent<FloatInformation>();
			_SLDetail.att0->GetTransform()->SetSize(blockWidth, blockHeight);
			_SLDetail.att0->GetTransform()->SetLocalLocation(offsetX, SL_OffsetY);
			_SLDetail.att0->Name->SetText(TT("att0"));
			_SLDetail.att0->FloatSlider->SetMinValue(0);
			_SLDetail.att0->FloatSlider->SetMaxValue(1);
			_SLDetail.att0->FloatSlider->SetValue(1.0f);
			SL_OffsetY += (blockHeight + gap);

			_SLDetail.att1 = SLInspector->CreateElement<Element>(TT("ATT1"))->CreateComponent<FloatInformation>();
			_SLDetail.att1->GetTransform()->SetSize(blockWidth, blockHeight);
			_SLDetail.att1->GetTransform()->SetLocalLocation(offsetX, SL_OffsetY);
			_SLDetail.att1->Name->SetText(TT("att1"));
			_SLDetail.att1->FloatSlider->SetMinValue(0);
			_SLDetail.att1->FloatSlider->SetMaxValue(0.01);
			_SLDetail.att1->FloatSlider->SetValue(0);
			SL_OffsetY += (blockHeight + gap);

			_SLDetail.att2 = SLInspector->CreateElement<Element>(TT("ATT2"))->CreateComponent<FloatInformation>();
			_SLDetail.att2->GetTransform()->SetSize(blockWidth, blockHeight);
			_SLDetail.att2->GetTransform()->SetLocalLocation(offsetX, SL_OffsetY);
			_SLDetail.att2->Name->SetText(TT("att2"));
			_SLDetail.att2->FloatSlider->SetMinValue(0);
			_SLDetail.att2->FloatSlider->SetMaxValue(0.0001);
			_SLDetail.att2->FloatSlider->SetValue(0);
			SL_OffsetY += (blockHeight + gap);



			_SLDetail.inRange = SLInspector->CreateElement<Element>(TT("inRange"))->CreateComponent<FloatInformation>();
			_SLDetail.inRange->GetTransform()->SetSize(blockWidth, blockHeight);
			_SLDetail.inRange->GetTransform()->SetLocalLocation(offsetX, SL_OffsetY);
			_SLDetail.inRange->Name->SetText(TT("inRange"));
			_SLDetail.inRange->FloatSlider->SetMinValue(0);
			_SLDetail.inRange->FloatSlider->SetMaxValue(1000);
			_SLDetail.inRange->FloatSlider->SetValue(80);
			SL_OffsetY += (blockHeight + gap);


			_SLDetail.outRange = SLInspector->CreateElement<Element>(TT("outRange"))->CreateComponent<FloatInformation>();
			_SLDetail.outRange->GetTransform()->SetSize(blockWidth, blockHeight);
			_SLDetail.outRange->GetTransform()->SetLocalLocation(offsetX, SL_OffsetY);
			_SLDetail.outRange->Name->SetText(TT("outRange"));
			_SLDetail.outRange->FloatSlider->SetMinValue(0);
			_SLDetail.outRange->FloatSlider->SetMaxValue(1000);
			_SLDetail.outRange->FloatSlider->SetValue(100);
			SL_OffsetY += (blockHeight + gap);



			_SLDetail.inCone = SLInspector->CreateElement<Element>(TT("inCone"))->CreateComponent<FloatInformation>();
			_SLDetail.inCone->GetTransform()->SetSize(blockWidth, blockHeight);
			_SLDetail.inCone->GetTransform()->SetLocalLocation(offsetX, SL_OffsetY);
			_SLDetail.inCone->Name->SetText(TT("inCone"));
			_SLDetail.inCone->FloatSlider->SetMinValue(0);
			_SLDetail.inCone->FloatSlider->SetMaxValue(90);
			_SLDetail.inCone->FloatSlider->SetValue(15);
			SL_OffsetY += (blockHeight + gap);


			_SLDetail.outCone = SLInspector->CreateElement<Element>(TT("outCone"))->CreateComponent<FloatInformation>();
			_SLDetail.outCone->GetTransform()->SetSize(blockWidth, blockHeight);
			_SLDetail.outCone->GetTransform()->SetLocalLocation(offsetX, SL_OffsetY);
			_SLDetail.outCone->Name->SetText(TT("outRange"));
			_SLDetail.outCone->FloatSlider->SetMinValue(0);
			_SLDetail.outCone->FloatSlider->SetMaxValue(90);
			_SLDetail.outCone->FloatSlider->SetValue(30);
			SL_OffsetY += (blockHeight + gap);
		}





	}
	for (auto& i : InspectorByType)
	{
		i.second->SetActive(false);
	}
}

void Inspector::Tick()
{
	if (SelectedUnit)
	{

		// 트랜스폼 부분
		SelectedUnit->Location = _ObjectDetail.Location->GetValue();
		SelectedUnit->Rotation = _ObjectDetail.Rotation->GetValue();
		SelectedUnit->Scale = _ObjectDetail.Scale->GetValue();
		SelectedUnit->Dirty = true;
		// 머터리얼 부분
		int index = _ObjectDetail.MaterialSlot->Dropdown->SelectedItemIndex();
		if (index >= 0)
		{
			auto& m = SelectedUnit->materialRef[index];
			std::wstring materialName = m->GetName();
			if (index != PrevIndex)
			{
				for (auto& SlotPair : _ObjectDetail.MaterialSlotByModel)
				{
					if (SlotPair.first == m->GetName()) SlotPair.second->SetActive(true);
					else SlotPair.second->SetActive(false);
				}
				auto model = _ObjectDetail.MaterialSlotByModel[m->GetName()];

				auto color = model->GetTransform()->GetChild(TT("Color"));
				if (color)
				{
					JVector4 c;
					auto colorInfo = color->GetOwner()->FindComponent<JVector4Information>();
					SelectedUnit->sceneObjectRef[index]->GetMaterial()->GetVarAsFloat4("Color", c);
					colorInfo->SetValue(c);
				}

				if (materialName == TT("Default"))
				{
					
				}
				// Material By Data
				else if (materialName == TT("Texture") || materialName == TT("NormalMapping"))
				{
					auto textures = SelectedUnit->sceneObjectRef[index]->GetMaterial()->GetBindedTextures();
					for (size_t i = 0; i < textures.size(); ++i)
					{
						auto child = model->GetTransform()->GetChild(s2ws(textures[i].first));
						if (child)
						{
							auto t = child->GetOwner()->FindComponent< TextureInformation>();
							t->Texture->SetImage(textures[i].second);
						}
					}
				}
			}
			else
			{
				// 바끼지 않으면
				auto model = _ObjectDetail.MaterialSlotByModel[m->GetName()];
				auto color = model->GetTransform()->GetChild(TT("Color"));
				if (color)
				{
		
					auto colorInfo = color->GetOwner()->FindComponent<JVector4Information>();
					JVector4 c = colorInfo->GetValue();
					SelectedUnit->sceneObjectRef[index]->GetMaterial()->SetVarAsFloat4("Color", c);
	
				}
				if (materialName == TT("Default"))
				{

				}
				// Material By Data
				else if (materialName == TT("Texture") || materialName == TT("NormalMapping"))
				{
					auto textures = m->GetBindedTextures();
					for (size_t i = 0; i < textures.size(); ++i)
					{
						auto child = model->GetTransform()->GetChild(s2ws(textures[i].first));
						if (child)
						{
							auto t = child->GetOwner()->FindComponent< TextureInformation>();
							SelectedUnit->sceneObjectRef[index]->GetMaterial()->BindTexture(textures[i].first, t->Texture->GetTexture());
						}
					}
				}




			}
			PrevIndex = index;
		}
		else
		{
			for (auto& SlotPair : _ObjectDetail.MaterialSlotByModel)
			{
				SlotPair.second->SetActive(false);
			}
		}








	}


	if (SelectedLight)
	{
		auto type = SelectedLight->ptr->GetType();
		switch (type)
		{
		case GE::LightType::Directional:
		{
			GE::DirectionalLight* DLLight = (GE::DirectionalLight*)(SelectedLight->ptr);

			
			auto color = _DLDetail.Color->GetValue();
			SelectedLight->Color = JColor(color.x, color.y, color.z, color.w);
			SelectedLight->Rotation = _DLDetail.Dir->GetValue();


		}
			break;

		case GE::LightType::Point:
		{
			GE::PointLight* PLLight = (GE::PointLight*)(SelectedLight->ptr);
			auto color = _PLDetail.Color->GetValue();
			SelectedLight->Color = JColor(color.x, color.y, color.z, color.w);
			SelectedLight->Location = _PLDetail.Location->GetValue();
			SelectedLight->att0 = _PLDetail.att0->FloatSlider->GetValue();
			SelectedLight->att1 = _PLDetail.att1->FloatSlider->GetValue();
			SelectedLight->att2 = _PLDetail.att2->FloatSlider->GetValue();
			SelectedLight->inRange = _PLDetail.inRange->FloatSlider->GetValue();
			SelectedLight->outRange = _PLDetail.outRange->FloatSlider->GetValue();

		}


			break;

		case GE::LightType::Spot:
		{
			auto color = _SLDetail.Color->GetValue();
			SelectedLight->Color = JColor(color.x, color.y, color.z, color.w);
			SelectedLight->Location = _SLDetail.Location->GetValue();
			SelectedLight->Rotation = _SLDetail.Dir->GetValue();
			SelectedLight->att0 = _SLDetail.att0->FloatSlider->GetValue();
			SelectedLight->att1 = _SLDetail.att1->FloatSlider->GetValue();
			SelectedLight->att2 = _SLDetail.att2->FloatSlider->GetValue();
			SelectedLight->inRange = _SLDetail.inRange->FloatSlider->GetValue();
			SelectedLight->outRange = _SLDetail.outRange->FloatSlider->GetValue();
			SelectedLight->inCone = _SLDetail.inCone->FloatSlider->GetValue();
			SelectedLight->outCone = _SLDetail.outCone->FloatSlider->GetValue();

		}

			break;

		}
	}


}

void Inspector::Destroy()
{



}

void Inspector::SetUnitInspector(UnitTest::Unit* unit)
{
	
	if (unit == nullptr)
	{
		if (SelectedUnit)
		{
			PrevIndex = -1;
			SelectedUnit = nullptr;
			InspectorByType[TT("오브젝트")]->SetActive(false);
		}

		return;
	}
	PrevIndex = -1;
	SelectedUnit = unit;
	ObjectType->Contents->SetText(TT("오브젝트"));
	ObjectName->Contents->SetText(unit->Name);

	auto ObjectInspector = InspectorByType[TT("오브젝트")];
	ObjectInspector->SetActive(true);
	InspectorByType[TT("다이렉션얼 라이트")]->SetActive(false);
	InspectorByType[TT("포인트 라이트")]->SetActive(false);
	InspectorByType[TT("스팟 라이트")]->SetActive(false);





	_ObjectDetail.Location->SetValue(unit->Location);
	_ObjectDetail.Rotation->SetValue(unit->Rotation);
	_ObjectDetail.Scale->SetValue(unit->Scale);


	size_t slotCnt = unit->materialRef.size();
	_ObjectDetail.MaterialSlot->Dropdown->RemoveAllItem();
	for (size_t i = 0; i < slotCnt; ++i)
	{
		_ObjectDetail.MaterialSlot->Dropdown->AddItem(TT("머터리얼 슬롯 ") + to_wstring(i));
	}
	_ObjectDetail.MaterialSlot->Dropdown->SelectItem(0);
}
void Inspector::SetLightInspector(UnitTest::Light* light)
{
	if (light == nullptr)
	{
		if (SelectedLight)
		{
			auto type = SelectedLight->ptr->GetType();
			SelectedLight = nullptr;


			switch (type)
			{
			case GE::LightType::Directional:
				InspectorByType[TT("다이렉션얼 라이트")]->SetActive(false);
				break;
			case GE::LightType::Point:
				InspectorByType[TT("포인트 라이트")]->SetActive(false);
				break;
			case GE::LightType::Spot:
				InspectorByType[TT("스팟 라이트")]->SetActive(false);
				break;
			}
		}
		return;
	}
	SelectedLight = light;
	auto type = SelectedLight->ptr->GetType();
	std::wstring typeToString;
	switch (type)
	{
	case GE::LightType::Directional:
		typeToString = TT("다이렉션얼 라이트");
		break;
	case GE::LightType::Point:
		typeToString = TT("포인트 라이트");

		break;
	case GE::LightType::Spot:
		typeToString = TT("스팟 라이트");
		break;
	}
	ObjectType->Contents->SetText(typeToString);
	ObjectName->Contents->SetText(light->name);
	for (auto& inspectorPair : InspectorByType)
	{
		if (inspectorPair.first == typeToString) inspectorPair.second->SetActive(true);
		else inspectorPair.second->SetActive(false);
	}

	switch (type)
	{
	case GE::LightType::Directional:
		_DLDetail.Color->SetValue(light->Color);
		_DLDetail.Dir->SetValue(light->Rotation);


		break;
	case GE::LightType::Point:
		_PLDetail.Color->SetValue(light->Color);
		_PLDetail.Location->SetValue(light->Location);

		_PLDetail.att0->FloatSlider->SetValue(light->att0);
		_PLDetail.att1->FloatSlider->SetValue(light->att1);
		_PLDetail.att2->FloatSlider->SetValue(light->att2);
		_PLDetail.inRange->FloatSlider->SetValue(light->inRange);
		_PLDetail.outRange->FloatSlider->SetValue(light->outRange);

		break;
	case GE::LightType::Spot:
		_SLDetail.Color->SetValue(light->Color);
		_SLDetail.Dir->SetValue(light->Rotation);

		_SLDetail.att0->FloatSlider->SetValue(light->att0);
		_SLDetail.att1->FloatSlider->SetValue(light->att1);
		_SLDetail.att2->FloatSlider->SetValue(light->att2);
		_SLDetail.inRange->FloatSlider->SetValue(light->inRange);
		_SLDetail.outRange->FloatSlider->SetValue(light->outRange);
		_SLDetail.inCone->FloatSlider->SetValue(light->inCone);
		_SLDetail.outCone->FloatSlider->SetValue(light->outCone);

		break;
	}

}

void Spawner::Awake()
{
	GetTransform()->SetSize(400, 950);


}

void Spawner::Start()
{







	float yOffset = 0.0f;
	ObjectType = AddBlock(TT("오브젝트 유형"), CreateElement<Element>(TT("ObjectType")),
		Block_Dropdown, yOffset)->GetTransform()->GetChild(1)->GetOwner()->FindComponent<Dropdown>();

	UnitNameInput = AddBlock(TT("오브젝트 이름"), CreateElement<Element>(TT("ObjectName")),
		Block_InputText, yOffset)->GetTransform()->GetChild(1)->GetOwner()->FindComponent<InputText>();

	MeshPath = AddBlock(TT("메시"), CreateElement<Element>(TT("MeshPath")),
		Block_TextBox, yOffset)->GetTransform()->GetChild(1)->GetChild(0)->GetOwner()->FindComponent<TextMeshComponent>();
	MeshPath->SetText(TT("메시 패스"));
	

	MaterialPanelOffsetY = yOffset;


	auto meshPathScript = MeshPath->GetTransform()->GetParent()->GetOwner()->CreateComponent<NativeScriptComponent>();
	meshPathScript->OnDropItem = [&](const GUIDropItem& item)
	{
		size_t cnt = item.GetDropSourceCount();
		if (cnt > 1) return;

		auto source = item.GetDropSource<std::wstring>(0);
		if (MeshPath->GetText() == source->Data) return;
	
		fs::path meshPath(source->Data);
		std::wstring errCode;
		if (meshPath.extension() == ".fbx" || meshPath.extension() == ".FBX")
		{
			if (GlobalSharedData::GetAssetManager()->Import(meshPath.wstring(),
				TT("../Source/Resource/Test/Model/"), FBX_ImportFlag_ImportMesh, &errCode))
			{
				std::wstring fileName = meshPath.filename();
				fileName = fileName.substr(0, fileName.length() - 4) + TT(".mesh");
			
				meshPath = TT("../Source/Resource/Test/Model/") + fileName;

			}
			else
			{
				MeshPath->SetText(TT("메시 로드 실패 : ") + errCode);
			}

		}
		MeshPath->SetText(meshPath.wstring());
		if (meshPath.extension() == ".mesh")
		{
			MaterialInspectors.clear();
			MaterialDropdowns.clear();
			MaterialBackGround.clear();

			if (MaterialPanel)
			{
				DestroyElement(MaterialPanel);
				MaterialPanel = nullptr;
			}
			if (MaterialPanel == nullptr)
			{
				auto clientRect = GetClientRect();
				MaterialPanel = CreateElement<Element>(TT("Material Area"));
				MaterialPanel->GetTransform()->SetLocalLocation(clientRect.left, clientRect.top + MaterialPanelOffsetY);
				MaterialPanel->GetTransform()->SetSize(clientRect.Width(), clientRect.Height() - MaterialPanelOffsetY);
			}
			bool is_success = false;
			if (unitTest->MeshPool.find(meshPath.wstring()) == unitTest->MeshPool.end())
			{
				is_success = MeshGenerator::LoadFromFile(meshPath.wstring(), unitTest->MeshPool[meshPath.wstring()]);
			}
			else is_success = true;
			if (is_success)
			{
				auto meshCnt = unitTest->MeshPool[meshPath.wstring()].size();
				float yoffset = 0.0f;
				for (uint32_t i = 0; i < meshCnt; ++i)
				{
					auto slot = MaterialPanel->CreateElement<Element>(TT("Slot") + to_wstring(i));
					auto image = slot->CreateElement<Element>(TT("Select"))->CreateComponent<ImageComponent>();
					image->GetTransform()->SetAnchor(AnchorPreset::Full_Stretch);
					image->GetOwner()->CreateComponent<MaterialSlot>();
					image->SetColor(JColor(0.5f, 0.5f, 0.5f, 1.0f));
				    slot = AddBlock(TT("Slot") + to_wstring(i), slot, Block_Dropdown, yoffset);
					MaterialDropdowns.push_back(
						slot->GetTransform()->GetChild(2)->GetOwner()->FindComponent<Dropdown>());

					MaterialBackGround.push_back(image);
				}
				for (uint32_t i = 0; i < meshCnt; ++i)
				{
					// Material By Data
					MaterialDropdowns[i]->AddItem(TT("Default"));
					MaterialDropdowns[i]->AddItem(TT("Texture"));
					MaterialDropdowns[i]->AddItem(TT("NormalMapping"));
					MaterialInspectors.push_back(
						CreateUIByMaterialModel(MaterialDropdowns[i]->GetOwner(), TT("Slot") + to_wstring(i)));
				}
			}
		}

	};

	ObjectType->AddItem(TT("오브젝트"));
	ObjectType->AddItem(TT("다이렉션얼 라이트"));
	ObjectType->AddItem(TT("포인트 라이트"));
	ObjectType->AddItem(TT("스팟 라이트"));



	CreateBt = CreateElement<Element>(TT("생성버튼"))->CreateComponent<Button>();
	CreateBt->GetOwner()->CreateComponent<ImageComponent>();
	auto btTxt = CreateBt->GetOwner()->CreateElement<Element>(TT("텍스트"))->CreateComponent<TextMeshComponent>();
	btTxt->SetText(TT("생성"));
	btTxt->GetTransform()->SetSize(CreateBt->GetTransform()->GetSize());
	btTxt->GetTransform()->SetAnchor(AnchorPreset::Full_Stretch);
	btTxt->SetTextVAlign(TextVAlign_Center);
	btTxt->SetTextHAlign(TextHAlign_Center);
	CreateBt->GetTransform()->SetLocalLocation(330.0f, 31);
	CreateBt->GetTransform()->SetSize(60, 20);

	CreateBt->BindOnClick([&]()
	{
		auto objectType = ObjectType->SelectedItem();
		auto objectName = UnitNameInput->text->GetText();
		if (objectType == TT("오브젝트"))
		{
			std::vector<std::wstring> materialNames;

			for (auto& mdd : MaterialDropdowns)
			{
				auto mName = mdd->SelectedItem();
				if (mName.empty()) mName = TT("Default");
				materialNames.push_back(mName);
			}

			unitTest->CreateUnit(objectName, MeshPath->GetText(), materialNames);
			auto unit = unitTest->GetUnit(objectName);

			// 텍스쳐 적용
			auto cnt = unit->sceneObjectRef.size();
			for (size_t i = 0; i < cnt; ++i)
			{
				auto  m     = unit->sceneObjectRef[i]->GetMaterial();
				auto  mName = m->GetOwnerMaterial()->GetName();
				auto  dropdown = MaterialDropdowns[i];

				auto inspector = dropdown->GetTransform()->GetChild(TT("Material Inspector"));
				auto childCnt =inspector->GetTransform()->GetChildCount();
				for (uint32_t k = 0; k < childCnt; ++k)
				{
					auto child = inspector->GetTransform()->GetChild(k)->GetOwner();
					if (child->GetName() != dropdown->SelectedItem()) continue;
					// Material By Data 
					// 오브젝트 데이터 변경
					if (child->GetName() == TT("Texture"))
					{
						auto texture = child->GetTransform()->GetChild(TT("Albedo"))->GetChild(TT("Texture"))->GetOwner()->FindComponent<ImageComponent>();
						m->BindTexture("Albedo", texture->GetTexture());
					}
					else if (child->GetName() == TT("NormalMapping"))
					{
						auto texture = child->GetTransform()->GetChild(TT("Albedo"))->GetChild(TT("Texture"))->GetOwner()->FindComponent<ImageComponent>();
						m->BindTexture("Albedo", texture->GetTexture());

						texture = child->GetTransform()->GetChild(TT("Normal"))->GetChild(TT("Texture"))->GetOwner()->FindComponent<ImageComponent>();
						m->BindTexture("Normal", texture->GetTexture());
					}
				}
			}
			

		}
		else if (objectType == TT("포인트 라이트"))
		{
			unitTest->CreatePointLight(objectName);
		}
		else if (objectType == TT("스팟 라이트"))
		{
			unitTest->CreateSpotLight(objectName);
		}
		else if(objectType == TT("다이렉션얼 라이트"))
		{
			unitTest->CreateDirLight(objectName);
		}


	});
}

void Spawner::Tick()
{



	auto objectType = ObjectType->SelectedItem();
	if (objectType == TT("오브젝트"))
	{
		MeshPath->GetTransform()->GetParent()->GetParent()->GetOwner()->SetActive(true);
		if (MaterialPanel) MaterialPanel->SetActive(true);

		uint32_t cnt = MaterialBackGround.size();
		for (uint32_t i = 0; i < cnt; ++i)
		{
			if (JColor::Orange() == MaterialBackGround[i]->GetColor())
			{
				MaterialInspectors[i]->SetActive(true);
				auto selectItem = MaterialDropdowns[i]->SelectedItem();

				uint32_t cnt = MaterialInspectors[i]->GetTransform()->GetChildCount();
				for (uint32_t k = 0; k < cnt; ++k)
				{
					auto child = MaterialInspectors[i]->GetTransform()->GetChild(k)->GetOwner();
					auto name = child->GetName();
					if (name == selectItem) child->SetActive(true);
					else child->SetActive(false);
				}
			}
			else MaterialInspectors[i]->SetActive(false);
		}
	}
	else if (objectType == TT("포인트 라이트"))
	{
		MeshPath->GetTransform()->GetParent()->GetParent()->GetOwner()->SetActive(false);
		if (MaterialPanel) MaterialPanel->SetActive(false);
	}
	else if (objectType == TT("스팟 라이트"))
	{
		MeshPath->GetTransform()->GetParent()->GetParent()->GetOwner()->SetActive(false);
		if (MaterialPanel) MaterialPanel->SetActive(false);
	}
	else
	{
		MeshPath->GetTransform()->GetParent()->GetParent()->GetOwner()->SetActive(false);
		if (MaterialPanel) MaterialPanel->SetActive(false);
	}






	

}

void Spawner::Destroy()
{
}

void Spawner::OnKeyUp(KeyCode code)
{
	if (code == KeyCode::A)
	{
		DestroyElement(MaterialPanel);
		MaterialPanel = nullptr;
	}
}

Element* Spawner::AddBlock(const std::wstring& name, Element* element, int uiType, float& BlockOffsetY)
{
	auto  clientRect   = GetClientRect();
	float lineSpacing = 10.0f;
	float Spacing     = 10.0f;

	float x = clientRect.left + lineSpacing;
	float y = clientRect.top + BlockOffsetY + lineSpacing;


	JVector2 elementSize = { clientRect.Width() - (2 * Spacing), 20.0f };
	auto txt = element->CreateElement<Element>(TT("Text"))->CreateComponent<TextMeshComponent>();
	txt->GetTransform()->SetSize(elementSize.x * 0.4f, elementSize.y);
	txt->SetTextVAlign(TextVAlign_Center);
	txt->SetText(name);


	switch (uiType)
	{
	case Block_Dropdown:
	{
		element->GetTransform()->SetSize(elementSize);
		element->GetTransform()->SetLocalLocation(x, y);



		auto dropdown = element->CreateElement<Element>(TT("Dropdown"))->CreateComponent<Dropdown>();
		dropdown->GetTransform()->SetAnchor(JRect(0.4f, 0.0f, 0.4f,0.0f));
	}
		BlockOffsetY += (20.0f + lineSpacing);
		break;
	case Block_InputText:
	{
		element->GetTransform()->SetSize(elementSize);
		element->GetTransform()->SetLocalLocation(x, y);

		auto image = element->CreateElement<Element>(TT("Image"))->CreateComponent<ImageComponent>();
		image->GetOwner()->CreateComponent<InputText>();
		image->GetTransform()->SetSize(elementSize.x * 0.6f, elementSize.y);
		image->GetTransform()->SetAnchor(JRect(0.4f, 0.0f, 0.4f, 0.0f));
		image->SetColor(JColor(0.8f, 0.8f, 0.8f, 1.0f));

		auto text = image->GetOwner()->CreateElement<Element>(TT("text"))->CreateComponent<TextMeshComponent>();
		text->GetTransform()->SetSize(image->GetTransform()->GetSize());
		text->GetTransform()->SetAnchor(AnchorPreset::Full_Stretch);
		text->SetTextVAlign(TextVAlign_Center);
		text->SetColor(JColor::Black());
		text->SetText(TT("(None)"));

	}


		BlockOffsetY += (20.0f + lineSpacing);
		break;
	case Block_TextBox:
	{
		element->GetTransform()->SetSize(elementSize);
		element->GetTransform()->SetLocalLocation(x, y);


		auto image = element->CreateElement<Element>(TT("PathBackGround"))->CreateComponent<ImageComponent>();
		image->GetTransform()->SetSize(elementSize.x * 0.6f, elementSize.y * 4);
		image->GetTransform()->SetAnchor(JRect(0.4f, 0.0f, 0.4f, 0.0f));
		image->SetColor(JColor::White());
		auto text = image->GetOwner()->CreateElement<Element>(TT("text"))->CreateComponent<TextMeshComponent>();
		text->GetTransform()->SetSize(image->GetTransform()->GetSize());
		text->GetTransform()->SetAnchor(AnchorPreset::Full_Stretch);
		text->SetColor(JColor::Black());
		text->SetText(TT("(None)"));

	}



		BlockOffsetY += (80.0f + lineSpacing);
		break;
	case Block_Texture:
	{
		element->GetTransform()->SetSize(elementSize);
		element->GetTransform()->SetLocalLocation(x, y);


		auto image = element->CreateElement<Element>(TT("Texture"))->CreateComponent<ImageComponent>();
		image->GetOwner()->CreateComponent<TextureLoader>();
		image->GetTransform()->SetSize(elementSize.x * 0.4f, elementSize.x * 0.4f);
		image->GetTransform()->SetAnchor(JRect(0.4f, 0.0f, 0.4f, 0.0f));
		image->GetOwner()->CreateComponent<TextureLoader>();
		image->SetImage(nullptr);
	}
	BlockOffsetY += (elementSize.x * 0.4f + lineSpacing);
	break;

	
	}
	

	return element;
}

JGUI::Element* Spawner::CreateUIByMaterialModel(JGUI::Element* ownerElement, const std::wstring& slotName)
{
	auto cnt      = MaterialBackGround.size();
	auto location = MaterialBackGround[cnt - 1]->GetTransform()->GetLocation();
	auto size     = MaterialBackGround[cnt - 1]->GetTransform()->GetSize();


	auto clientRect = GetClientRect();
	auto e = ownerElement->CreateElement<Element>(TT("Material Inspector"))->CreateComponent<Border>();
	e->GetOwner()->CreateComponent<ShapeComponent>();
	e->GetTransform()->SetSize(clientRect.Width() - 20, clientRect.Height() - (location.y + size.y - clientRect.top) - 20.0f);
	e->GetTransform()->SetLocation(clientRect.left + 10.0f, location.y + size.y + 10.0f);


	// Material By Data

	// Default
	{
		
		auto SD_Default = e->GetOwner()->CreateElement<Element>(TT("Default"));
		SD_Default->SetActive(false);
	}
	// Texture
	{
		float offsetY = 10.0f;
		auto SD_Texture = e->GetOwner()->CreateElement<Element>(TT("Texture"));
		AddBlock(TT("Albedo"), SD_Texture->CreateElement<Element>(TT("Albedo")), Block_Texture, offsetY);
		SD_Texture->SetActive(false);
	}
	// Normal
	{
		float offsetY = 10.0f;
		auto SD_Normal = e->GetOwner()->CreateElement<Element>(TT("NormalMapping"));
		AddBlock(TT("Albedo"), SD_Normal->CreateElement< Element>(TT("Albedo")), Block_Texture, offsetY);
		AddBlock(TT("Normal"), SD_Normal->CreateElement< Element>(TT("Normal")), Block_Texture, offsetY);
		SD_Normal->SetActive(false);

	}

	e->SetActive(false);
	return e->GetOwner();
}
void WorldOutliner::Awake()
{
	GetTransform()->SetSize(400, 800);
}


void WorldOutliner::Start()
{
	Reset();
}
void WorldOutliner::Tick()
{
	
	auto item = WorldTreeView->GetSelectedItem();
	if(item == nullptr) return;
	if (item == Root || ObjectTreeItem == item ||
		PointLightItem == item || SpotLightTreeItem == item ||
		DirectionalLightTreeItem == item) return;


	if (item->GetTreeParent() == ObjectTreeItem)
	{
		SelectLight = nullptr;
		PrevSelectLight = nullptr;
		auto selectItem = item->GetOwner()->GetName();
		SelectUnit = unitTest->UnitPool[selectItem].get();
		if (SelectUnit != PrevUnit)
		{
			unitTest->SelectedUnit = SelectUnit;
			unitTest->SelectedLight = nullptr;
			if (unitTest->InspectorForm)
			{
				unitTest->InspectorForm->SetLightInspector(nullptr);
				unitTest->InspectorForm->SetUnitInspector(unitTest->SelectedUnit);
			}
			PrevUnit = SelectUnit;
		}
	}
	else
	{
		PrevUnit = nullptr;
		SelectUnit = nullptr;
		auto selectItem = item->GetOwner()->GetName();
		SelectLight = unitTest->LightPool[selectItem].get();
		if (SelectLight != PrevSelectLight)
		{
			unitTest->SelectedLight = SelectLight;
			unitTest->SelectedUnit = nullptr;
			if (unitTest->InspectorForm)
			{
				unitTest->InspectorForm->SetUnitInspector(nullptr);
				unitTest->InspectorForm->SetLightInspector(unitTest->SelectedLight);
			}

			PrevSelectLight = SelectLight;
		}
	}
}
void WorldOutliner::Destroy()
{

}
void WorldOutliner::Reset()
{
	if (WorldTreeView)
	{
		DestroyElement(WorldTreeView->GetOwner());
	}
	PrevUnit   = nullptr;
	SelectUnit = nullptr;
	SelectLight = nullptr;
	PrevSelectLight = nullptr;


	WorldTreeView = CreateElement<Element>(TT("WorldView"))->CreateComponent<TreeView>();
	WorldTreeView->GetTransform()->SetLocalLocation(11.0f, 31.0f);
	WorldTreeView->GetTransform()->SetSize(378, 758);
	TreeItemDesc desc;
	desc.name = TT("World");
	Root = WorldTreeView->CreateRoot(desc);

	desc.name = TT("Object");
	ObjectTreeItem = Root->AddItem(desc);

	desc.name = TT("PointLight");
	PointLightItem = Root->AddItem(desc);

	desc.name = TT("SpotLight");
	SpotLightTreeItem = Root->AddItem(desc);

	desc.name = TT("DirectionalLight");
	DirectionalLightTreeItem = Root->AddItem(desc);


	for (auto& item : unitTest->UnitPool)
	{
		TreeItemDesc desc;
		desc.name = item.first;
		ObjectTreeItem->AddItem(desc);
	}
	for (auto& item : unitTest->LightPool)
	{
		TreeItemDesc desc;
		desc.name = item.first;
		switch(item.second->ptr->GetType())
		{
		case GE::LightType::Directional:
			DirectionalLightTreeItem->AddItem(desc);
			break;
		case GE::LightType::Point:
			PointLightItem->AddItem(desc);
			break;
		case GE::LightType::Spot:
			SpotLightTreeItem->AddItem(desc);
			break;
		}

	}
}
void MapSaveLoad::Awake()
{
	GetTransform()->SetSize(300, 20);

	Input = GetOwner()->CreateElement<Element>(TT("Input"))->CreateComponent<InputTextField>();
	Input->GetTransform()->SetSize(240.0f, 20.0f);

	SaveBt = GetOwner()->CreateElement<Element>(TT("SaveButton"))->CreateComponent<Button>();
	SaveBt->GetOwner()->CreateComponent<ImageComponent>();
	SaveBt->GetTransform()->SetSize(50.0f , 20.0f);
	SaveBt->GetTransform()->SetLocalLocation(250.0f, 0.0f);

	auto btText = SaveBt->GetOwner()->CreateElement<Element>(TT("Text"))->CreateComponent<TextMeshComponent>();
	btText->GetTransform()->SetSize(50, 20);
	btText->GetTransform()->SetAnchor(AnchorPreset::Full_Stretch);
	btText->SetTextHAlign(TextHAlign_Center);
	btText->SetTextVAlign(TextVAlign_Center);
	btText->SetText(TT("저장"));

	SaveBt->BindOnClick([&]()
	{
		Save(TT("../Source/Resource/Test/Map/") + Input->Text->GetText());
	});


}
void MapSaveLoad::Load(const std::wstring& path)
{
	std::ifstream fin;

	fin.open(ws2s(path), ios::binary);
	if (!fin.is_open()) return;

	uint32_t objectCnt = 0;
	fin.read(reinterpret_cast<char*>(&objectCnt),
		sizeof(uint32_t));



	for (uint32_t i = 0; i < objectCnt; ++i)
	{
		JVector3 Location;
		JVector3 Rotation;
		JVector3 Scale;
		uint32_t slotCnt;
		wstring unitName;
		wstring meshPath;
		std::vector<wstring> materialNames;
	
		Read(fin, unitName);

		fin.read(reinterpret_cast<char*>(&Location),
			sizeof(JVector3));
		fin.read(reinterpret_cast<char*>(&Rotation),
			sizeof(JVector3));
		fin.read(reinterpret_cast<char*>(&Scale),
			sizeof(JVector3));
		// 메시 패스
		Read(fin, meshPath);

		//unitTest->CreateUnit(unitName, meshPath, )

		// 머터리얼 슬롯 갯수
		fin.read(reinterpret_cast<char*>(&slotCnt),
			sizeof(uint32_t));

		for (uint32_t i = 0; i < slotCnt; ++i)
		{
			// 머터리얼 이름
			std::wstring mName;
			Read(fin, mName);
			materialNames.push_back(mName);
		}
		unitTest->CreateUnit(unitName, meshPath, materialNames);
		auto unit = unitTest->GetUnit(unitName);

		if (unit)
		{
			for (uint32_t i = 0; i < slotCnt; ++i)
			{
				auto m = unit->sceneObjectRef[i]->GetMaterial();
				JVector4 color;
				fin.read(reinterpret_cast<char*>(&color),
					sizeof(JVector4));

				m->SetVarAsFloat4("Color", color);

				// Material By Data
				if (materialNames[i] == TT("Default"))
				{


				}
				else if (materialNames[i] == TT("Texture"))
				{
					std::wstring albedo;
					Read(fin, albedo);
					m->BindTexture("Albedo", GraphicsIF::LoadTexture(albedo).Get());

				}
				else if (materialNames[i] == TT("NormalMapping"))
				{
					std::wstring albedo;
					Read(fin, albedo);
					std::wstring normal;
					Read(fin, normal);


					m->BindTexture("Albedo", GraphicsIF::LoadTexture(albedo).Get());
					m->BindTexture("Normal", GraphicsIF::LoadTexture(normal).Get());
				}




			}

			unit->Location = Location;
			unit->Rotation = Rotation;
			unit->Scale    = Scale;
			unit->Dirty = true;
		}
	}


	uint32_t lightCnt = 0;
	fin.read(reinterpret_cast<char*>(&lightCnt),
		sizeof(uint32_t));
	// 라이트
	for (uint32_t i = 0; i < lightCnt; ++i)
	{
		// 타입
		GE::LightType type;
		fin.read(reinterpret_cast<char*>(&type),
			sizeof(GE::LightType));
		// 이름
		std::wstring name;
		Read(fin, name);
		// 정보

		UnitTest::LightInfo info;
		fin.read(reinterpret_cast<char*>(&info),
			sizeof(UnitTest::LightInfo));


		switch (type)
		{
		case GE::LightType::Directional:
			unitTest->CreateDirLight(name);
			break;
		case GE::LightType::Point:
			unitTest->CreatePointLight(name);
			break;
		case GE::LightType::Spot:
			unitTest->CreateSpotLight(name);
			break;
		}
		auto l = unitTest->GetLight(name);
		l->Color    = info.Color;
		l->Location = info.Location;
		l->Rotation = info.Rotation;
		l->att0     = info.att0;
		l->att1     = info.att1;
		l->att2     = info.att2;
		l->inRange  = info.inRange;
		l->outRange = info.outRange;
		l->inCone   = info.inCone;
		l->outCone  = info.outCone;


	}

	fin.close();
	Input->Text->SetText(fs::path(path).filename().wstring());

	//unitTest->



}
void MapSaveLoad::Save(const std::wstring& path)
{
	std::ofstream fout;


	fout.open(ws2s(path), ios::binary);
	if (!fout.is_open()) return;

	// 오브젝트 갯수
	uint32_t ObjectCount = unitTest->UnitPool.size();
	fout.write(
		reinterpret_cast<const char*>(&ObjectCount), 
		sizeof(uint32_t));

	for (auto& unitPair : unitTest->UnitPool)
	{
		auto unit = unitPair.second.get();

		// 이름
		Write(fout, unit->Name);

		fout.write(
			reinterpret_cast<const char*>(&unit->Location),
			sizeof(JVector3));
		fout.write(
			reinterpret_cast<const char*>(&unit->Rotation),
			sizeof(JVector3));
		fout.write(
			reinterpret_cast<const char*>(&unit->Scale),
			sizeof(JVector3));


		//meshPath
		Write(fout, unit->MeshName);

		// 머터리얼 슬롯 갯수
		uint32_t slotCount = unit->materialRef.size();
		fout.write(
			reinterpret_cast<const char*>(&slotCount),
			sizeof(uint32_t));

		for (uint32_t i = 0; i < slotCount; ++i)
		{
			auto& mName = unit->materialRef[i]->GetName();
			Write(fout, mName);
		}

		for (uint32_t i = 0; i < slotCount; ++i)
		{
			//// Material By Data
			auto& mName = unit->materialRef[i]->GetName();
			if (mName == TT("Default"))
			{
				// color
				JVector4 Color;
				unit->sceneObjectRef[i]->GetMaterial()->GetVarAsFloat4("Color", Color);
				fout.write(
					reinterpret_cast<const char*>(&Color),
					sizeof(JVector4));
			}
			else if (mName == TT("Texture"))
			{
				// color
				JVector4 Color;
				unit->sceneObjectRef[i]->GetMaterial()->GetVarAsFloat4("Color", Color);
				fout.write(
					reinterpret_cast<const char*>(&Color),
					sizeof(JVector4));
				auto bindedTexture =unit->sceneObjectRef[i]->GetMaterial()->GetBindedTextures();
				auto albedo = bindedTexture[0].second->GetInfo().path;
				Write(fout, albedo);
				// albedo -> texturePath
			}
			else if(mName == TT("NormalMapping"))
			{
				// color
				JVector4 Color;
				unit->sceneObjectRef[i]->GetMaterial()->GetVarAsFloat4("Color", Color);
				fout.write(
					reinterpret_cast<const char*>(&Color),
					sizeof(JVector4));
				auto bindedTexture = unit->sceneObjectRef[i]->GetMaterial()->GetBindedTextures();
				auto albedo = bindedTexture[0].second->GetInfo().path;
				auto normal = bindedTexture[1].second->GetInfo().path;
				Write(fout, albedo);
				Write(fout, normal);
			}
			
		}
	}


	uint32_t lightCount = unitTest->LightPool.size();
	// 라이트
	fout.write(
		reinterpret_cast<const char*>(&lightCount),
		sizeof(uint32_t));

	for (auto& light : unitTest->LightPool)
	{
		auto l = light.second.get();
		auto type = l->ptr->GetType();
		// 타입
		fout.write(
			reinterpret_cast<const char*>(&type),
			sizeof(GE::LightType));



		// 이름
		Write(fout, l->name);

		UnitTest::LightInfo info;
		info.Color = l->Color;
		info.Location = l->Location;
		info.Rotation = l->Rotation;
		info.att0 = l->att0;
		info.att1 = l->att1;
		info.att2 = l->att2;
		info.inRange = l->inRange;
		info.outRange = l->outRange;
		info.inCone = l->inCone;
		info.outCone = l->outCone;

		fout.write(
			reinterpret_cast<const char*>(&info),
			sizeof(UnitTest::LightInfo));
		// 정보
	}


	fout.close();


}
void MapSaveLoad::Write(std::ofstream& fout, const std::wstring& str)
{
	auto strLen = str.length();
	fout.write(
		reinterpret_cast<const char*>(&strLen),
		sizeof(size_t));
	fout.write(
		reinterpret_cast<const char*>(str.c_str()),
		strLen * sizeof(wchar_t));
}
void MapSaveLoad::Read(std::ifstream& fin, std::wstring& str)
{
	size_t strLen = 0;
	fin.read(
		reinterpret_cast<char*>(&strLen),
		sizeof(size_t));
	str.resize(strLen);
	fin.read(
		reinterpret_cast<char*>(&str[0]),
		strLen * sizeof(wchar_t));
}
void InputText::Start()
{
	image = GetOwner()->FindComponent<ImageComponent>();
	text = image->GetTransform()->GetChild(0)->GetOwner()->FindComponent<TextMeshComponent>();


	OnFocusEnter = [&]()
	{
		image->SetColor(JColor::White());

	};


	OnFocusExit = [&]()
	{
		image->SetColor(JColor(0.8f, 0.8f, 0.8f, 1.0f));
	};

	OnChar = [&](wchar_t c)
	{
		if (c == TT('\r') || c == TT('\n')) return;
		std::wstring t = text->GetText();
		size_t len = t.length();
		if (c == TT('\b') && text->GetText().length() != 0)
		{
			text->SetText(t.substr(0, len - 1));
		}
		else
		{
			text->SetText(t + c);
		}

	};


}

void TextureLoader::Start()
{
	image = GetOwner()->FindComponent<ImageComponent>();

	OnDropItem = [&](const GUIDropItem& item)
	{
		int cnt = item.GetDropSourceCount(); if (cnt > 1) return;

		fs::path p(item.GetDropSource<std::wstring>(0)->Data);
		image->SetImage(p.wstring());
	};

}

void MaterialSlot::Start()
{
	image = GetOwner()->FindComponent<ImageComponent>();
	OnMouseClick = [&](KeyCode code)
	{
		if (code == KeyCode::LeftMouseButton)
		{
			image->SetColor(JColor::Orange());
		}

		auto allSlot = GetOwner()->GetTransform()->GetParent()->GetTransform()->GetParent();
		auto slotCnt = allSlot->GetTransform()->GetChildCount();

		for (uint32_t i = 0; i < slotCnt; ++i)
		{
			auto slot = allSlot->GetChild(i)->GetChild(TT("Select"))->GetOwner()->FindComponent<MaterialSlot>();
			if (slot != this)
			{
				slot->image->SetColor(JColor(0.5f, 0.5f, 0.5f, 1.0f));
			}

		}

	};

}

void TextInformation::Awake()
{
	JVector2 OwnerSize = GetTransform()->GetSize();

	Name     = GetOwner()->CreateElement<Element>(TT("Name"))->CreateComponent<TextMeshComponent>();
	Contents = GetOwner()->CreateElement<Element>(TT("Contents"))->CreateComponent<TextMeshComponent>();
	JRect NameAnchor; 
	NameAnchor.left = 0.0f; NameAnchor.right  = 0.3f; 
	NameAnchor.top  = 0.0f; NameAnchor.bottom = 1.0f;
	JRect ContentsAnchor;
	ContentsAnchor.left = 0.3f; ContentsAnchor.right  = 1.0f;
	ContentsAnchor.top  = 0.0f; ContentsAnchor.bottom = 1.0f;
	


	Name->GetTransform()->SetSize(OwnerSize.x * NameAnchor.Width(), OwnerSize.y);
	Name->GetTransform()->SetAnchor(NameAnchor);

	Contents->GetTransform()->SetSize(OwnerSize.x * ContentsAnchor.Height(), OwnerSize.y);
	Contents->GetTransform()->SetAnchor(ContentsAnchor);
}

void FloatInformation::Awake()
{
	Name = GetOwner()->CreateElement<Element>(TT("Name"))->CreateComponent<TextMeshComponent>();
	FloatSlider = GetOwner()->CreateElement<Element>(TT("Name"))->CreateComponent<Slider>();
	FloatSlider->GetTransform()->SetSize(0, 0);
	Value = GetOwner()->CreateElement<Element>(TT("Value"))->CreateComponent<TextMeshComponent>();

	JRect NameAnchor;
	NameAnchor.left = 0.0f; NameAnchor.right = 0.2f; NameAnchor.top = 0.0f; NameAnchor.bottom = 1.0f;
	Name->GetTransform()->SetAnchor(NameAnchor);


	JRect FloatSliderAnchor;
	FloatSliderAnchor.left = 0.2f; FloatSliderAnchor.right = 0.7f; FloatSliderAnchor.top = 0.0f; FloatSliderAnchor.bottom = 1.0f;
	FloatSlider->GetTransform()->SetAnchor(FloatSliderAnchor);
	


	JRect ValueAnchor;
	ValueAnchor.left = 0.7f; ValueAnchor.right = 1.0f; ValueAnchor.top = 0.0f; ValueAnchor.bottom = 1.0f;
	Value->GetTransform()->SetAnchor(ValueAnchor);

}

void FloatInformation::Tick()
{
	Value->SetText(to_wstring(FloatSlider->GetValue()));

}

void JVector4Information::Awake()
{
	Name = GetOwner()->CreateElement<Element>(TT("Name"))->CreateComponent<TextMeshComponent>();
	XName = GetOwner()->CreateElement<Element>(TT("Name"))->CreateComponent<TextMeshComponent>();
	YName = GetOwner()->CreateElement<Element>(TT("Name"))->CreateComponent<TextMeshComponent>();
	WName = GetOwner()->CreateElement<Element>(TT("Name"))->CreateComponent<TextMeshComponent>();
	ZName = GetOwner()->CreateElement<Element>(TT("Name"))->CreateComponent<TextMeshComponent>();
	XInput = GetOwner()->CreateElement<Element>(TT("Name"))->CreateComponent<InputTextField>();
	YInput = GetOwner()->CreateElement<Element>(TT("Name"))->CreateComponent<InputTextField>();
	ZInput = GetOwner()->CreateElement<Element>(TT("Name"))->CreateComponent<InputTextField>();
	WInput = GetOwner()->CreateElement<Element>(TT("Name"))->CreateComponent<InputTextField>();
	XInput->Text->SetText(TT("0"));
	YInput->Text->SetText(TT("0"));
	ZInput->Text->SetText(TT("0"));
	WInput->Text->SetText(TT("0"));
	JRect NameAnchor;
	NameAnchor.left = 0.0f; NameAnchor.right = 1.0f; NameAnchor.top = 0.0f; NameAnchor.bottom = 0.5f;
	Name->GetTransform()->SetAnchor(NameAnchor);
	float xoffset = 0.0f;

	JRect XNameAnchor;
	XNameAnchor.left = xoffset; XNameAnchor.right = xoffset + 2.0f / 20.0f;
	XNameAnchor.top = 0.5f;    XNameAnchor.bottom = 1.0f;
	XName->GetTransform()->SetAnchor(XNameAnchor);
	xoffset += 2.0f / 20.0f;

	JRect XInputAnchor;
	XInputAnchor.left = xoffset; XInputAnchor.right = xoffset + 3.0f / 20.0f;
	XInputAnchor.top = 0.5f;    XInputAnchor.bottom = 1.0f;
	XInput->GetTransform()->SetAnchor(XInputAnchor);
	xoffset += 3.0f / 20.0f;



	JRect YNameAnchor;
	YNameAnchor.left = xoffset; YNameAnchor.right = xoffset + 2.0f / 20.0f;
	YNameAnchor.top = 0.5f;    YNameAnchor.bottom = 1.0f;
	YName->GetTransform()->SetAnchor(YNameAnchor);
	xoffset += 2.0f / 20.0f;

	JRect YInputAnchor;
	YInputAnchor.left = xoffset; YInputAnchor.right = xoffset + 3.0f / 20.0f;
	YInputAnchor.top = 0.5f;     YInputAnchor.bottom = 1.0f;
	YInput->GetTransform()->SetAnchor(YInputAnchor);
	xoffset += 3.0f / 20.0f;


	JRect ZNameAnchor;
	ZNameAnchor.left = xoffset; ZNameAnchor.right = xoffset + 2.0f / 20.0f;
	ZNameAnchor.top = 0.5f;     ZNameAnchor.bottom = 1.0f;
	ZName->GetTransform()->SetAnchor(ZNameAnchor);
	xoffset += 2.0f / 20.0f;

	JRect ZInputAnchor;
	ZInputAnchor.left = xoffset; ZInputAnchor.right = xoffset + 3.0f / 20.0f;
	ZInputAnchor.top = 0.5f;    ZInputAnchor.bottom = 1.0f;
	ZInput->GetTransform()->SetAnchor(ZInputAnchor);
	xoffset += 3.0f / 20.0f;



	JRect WNameAnchor;
	WNameAnchor.left = xoffset; WNameAnchor.right = xoffset + 2.0f / 20.0f;
	WNameAnchor.top = 0.5f;     WNameAnchor.bottom = 1.0f;
	WName->GetTransform()->SetAnchor(WNameAnchor);
	xoffset += 2.0f / 20.0f;

	JRect WInputAnchor;
	WInputAnchor.left = xoffset; WInputAnchor.right = 1.0F;
	WInputAnchor.top = 0.5f;    WInputAnchor.bottom = 1.0f;
	WInput->GetTransform()->SetAnchor(WInputAnchor);
	//xoffset += 3.0f / 20.0f;






	XInput->IsOnlyNumber = true;
	YInput->IsOnlyNumber = true;
	ZInput->IsOnlyNumber = true;
	WInput->IsOnlyNumber = true;
	XInput->GetOwner()->CreateComponent<NativeScriptComponent>()->OnFocusExit = [&]()
	{
		value.x = _wtof(XInput->Text->GetText().c_str());
	};
	YInput->GetOwner()->CreateComponent<NativeScriptComponent>()->OnFocusExit = [&]()
	{
		value.y = _wtof(YInput->Text->GetText().c_str());
	};
	ZInput->GetOwner()->CreateComponent<NativeScriptComponent>()->OnFocusExit = [&]()
	{
		value.z = _wtof(ZInput->Text->GetText().c_str());
	};
	WInput->GetOwner()->CreateComponent<NativeScriptComponent>()->OnFocusExit = [&]()
	{
		value.w = _wtof(WInput->Text->GetText().c_str());
	};
}
void JVector4Information::SetValue(const JVector4& v)
{
	value = v;
	XInput->Text->SetText(to_wstring(v.x));
	YInput->Text->SetText(to_wstring(v.y));
	ZInput->Text->SetText(to_wstring(v.z));
	WInput->Text->SetText(to_wstring(v.w));
}
const JVector4& JVector4Information::GetValue() const
{
	return value;
}

bool JVector4Information::IsRevising() const
{
	return XInput->IsFocusing || YInput->IsFocusing || ZInput->IsFocusing || WInput->IsFocusing ;
}

void JVector3Information::Awake()
{
	Name = GetOwner()->CreateElement<Element>(TT("Name"))->CreateComponent<TextMeshComponent>();
	XName = GetOwner()->CreateElement<Element>(TT("Name"))->CreateComponent<TextMeshComponent>();
	YName = GetOwner()->CreateElement<Element>(TT("Name"))->CreateComponent<TextMeshComponent>();
	ZName = GetOwner()->CreateElement<Element>(TT("Name"))->CreateComponent<TextMeshComponent>();
	XInput = GetOwner()->CreateElement<Element>(TT("Name"))->CreateComponent<InputTextField>();
	YInput = GetOwner()->CreateElement<Element>(TT("Name"))->CreateComponent<InputTextField>();
	ZInput = GetOwner()->CreateElement<Element>(TT("Name"))->CreateComponent<InputTextField>();
	XInput->Text->SetText(TT("0"));
	YInput->Text->SetText(TT("0"));
	ZInput->Text->SetText(TT("0"));
	JRect NameAnchor;
	NameAnchor.left = 0.0f; NameAnchor.right = 1.0f; NameAnchor.top = 0.0f; NameAnchor.bottom = 0.5f;
	Name->GetTransform()->SetAnchor(NameAnchor);
	float xoffset = 0.0f;

	JRect XNameAnchor;
	XNameAnchor.left = xoffset; XNameAnchor.right = xoffset + 1.0f / 15.0f;
	XNameAnchor.top  = 0.5f;    XNameAnchor.bottom = 1.0f;
	XName->GetTransform()->SetAnchor(XNameAnchor);
	xoffset += 1.0f / 15.0f;

	JRect XInputAnchor;
	XInputAnchor.left = xoffset; XInputAnchor.right  = xoffset + 4.0f / 15.0f;
	XInputAnchor.top  = 0.5f;    XInputAnchor.bottom = 1.0f;
	XInput->GetTransform()->SetAnchor(XInputAnchor);
	xoffset += 4.0f / 15.0f;



	JRect YNameAnchor;
	YNameAnchor.left = xoffset; YNameAnchor.right = xoffset + 1.0f / 15.0f;
	YNameAnchor.top  = 0.5f;    YNameAnchor.bottom = 1.0f;
	YName->GetTransform()->SetAnchor(YNameAnchor);
	xoffset += 1.0f / 15.0f;

	JRect YInputAnchor;
	YInputAnchor.left = xoffset; YInputAnchor.right = xoffset + 4.0f / 15.0f;
	YInputAnchor.top = 0.5f;     YInputAnchor.bottom = 1.0f;
	YInput->GetTransform()->SetAnchor(YInputAnchor);
	xoffset += 4.0f / 15.0f;


	JRect ZNameAnchor;
	ZNameAnchor.left = xoffset; ZNameAnchor.right = xoffset + 1.0f / 15.0f;
	ZNameAnchor.top = 0.5f;     ZNameAnchor.bottom = 1.0f;
	ZName->GetTransform()->SetAnchor(ZNameAnchor);
	xoffset += 1.0f / 15.0f;

	JRect ZInputAnchor;
	ZInputAnchor.left = xoffset; ZInputAnchor.right = 1.0F;
	ZInputAnchor.top = 0.5f;    ZInputAnchor.bottom = 1.0f;
	ZInput->GetTransform()->SetAnchor(ZInputAnchor);



	XInput->IsOnlyNumber = true;
	YInput->IsOnlyNumber = true;
	ZInput->IsOnlyNumber = true;

	XInput->GetOwner()->CreateComponent<NativeScriptComponent>()->OnFocusExit = [&]()
	{
		value.x = _wtof(XInput->Text->GetText().c_str());
	};
	YInput->GetOwner()->CreateComponent<NativeScriptComponent>()->OnFocusExit = [&]()
	{
		value.y = _wtof(YInput->Text->GetText().c_str());
	};
	ZInput->GetOwner()->CreateComponent<NativeScriptComponent>()->OnFocusExit = [&]()
	{
		value.z = _wtof(ZInput->Text->GetText().c_str());
	};
}	
void JVector3Information::SetValue(const JVector3& v)
{
	value = v;
	XInput->Text->SetText(to_wstring(v.x));
	YInput->Text->SetText(to_wstring(v.y));
	ZInput->Text->SetText(to_wstring(v.z));
}
const JVector3& JVector3Information::GetValue() const
{
	return value;
}
bool JVector3Information::IsRevising() const
{
	return XInput->IsFocusing || YInput->IsFocusing || ZInput->IsFocusing;
}
void TextureInformation::Awake()
{
	Name     = GetOwner()->CreateElement<Element>(TT("Name"))->CreateComponent<TextMeshComponent>();
	Texture  = GetOwner()->CreateElement<Element>(TT("Texture"))->CreateComponent<JGUI::ImageComponent>();
	Texture->SetImage(nullptr);
	JRect NameAnchor;
	NameAnchor.left = 0.0f;  NameAnchor.right  = 0.3f;
	NameAnchor.top  = 0.0f;  NameAnchor.bottom = 1.0f;
	Name->GetTransform()->SetAnchor(NameAnchor);

	JRect TextureAnchor;
	TextureAnchor.left = 0.3f;  TextureAnchor.right  = 0.3f;
	TextureAnchor.top  = 0.0f;  TextureAnchor.bottom = 0.0f;
	Texture->GetTransform()->SetAnchor(TextureAnchor);
	Texture->GetTransform()->SetSize(80, 80);


	auto script = Texture->GetOwner()->CreateComponent<NativeScriptComponent>();
	script->OnDropItem = [&](const GUIDropItem& item)
	{
		int cnt = item.GetDropSourceCount(); if (cnt > 1) return;

		fs::path p(item.GetDropSource<std::wstring>(0)->Data);
		Texture->SetImage(p.wstring());
	};
}

void InputTextField::Awake()
{
	BackGround = GetOwner()->CreateComponent<ImageComponent>();
	BackGround->SetColor(JColor(0.8f, 0.8f, 0.8f, 1.0f));
	Text       = GetOwner()->CreateElement<Element>(TT("Text"))->CreateComponent<TextMeshComponent>();
	Text->GetTransform()->SetSize(GetTransform()->GetSize());
	Text->GetTransform()->SetAnchor(JGUI::AnchorPreset::Full_Stretch);
	Text->SetColor(JColor::Black());
	OnFocusEnter = [&]()
	{
		BackGround->SetColor(JColor::White());
		IsFocusing = true;
	};
	OnFocusExit = [&]()
	{
		BackGround->SetColor(JColor(0.8f, 0.8f, 0.8f, 1.0f));
		IsFocusing = false;
	};
	OnMouseDoubleClick = [&](KeyCode code)
	{
		Text->SetText(TT(""));
	};
	OnChar = [&](wchar_t c)
	{
		if (c == TT('\r') || c == TT('\n')) return;
		std::wstring t = Text->GetText();
		size_t len = t.length();

		if (IsOnlyNumber && c != TT('\b'))
		{
			// 0 ~ 9 사이에서만
			if (c < TT('0') || c > TT('9'))
			{
				if (c == TT('-') && len == 0)
				{

				}
				// . 가 아니거나, . 여도 이미 .가 포합되어있다면 패쓰
				else if (c != TT('.') || t.find(TT('.')) != wstring::npos)
				{
					return;
				}
				
			}
		}



		if (c == TT('\b'))
		{
			if (len != 0)
			{
				if (len > 1) Text->SetText(t.substr(0, len - 1));
				else Text->SetText(TT(""));
			}
		}
		else 
		{
			Text->SetText(t + c);
		}


	};
}

void DropdownInformation::Awake()
{
	Name = GetOwner()->CreateElement<Element>(TT("Name"))->CreateComponent<TextMeshComponent>();
	Dropdown = GetOwner()->CreateElement<Element>(TT("Name"))->CreateComponent<JGUI::Dropdown>();
	Dropdown->GetTransform()->SetSize(0, 0);
	JRect NameAnchor;
	NameAnchor.left = 0.0f; NameAnchor.right = 0.3f;
	NameAnchor.top = 0.0f; NameAnchor.bottom = 1.0f;
	Name->GetTransform()->SetAnchor(NameAnchor);
	JRect DropdownAnchor;
	DropdownAnchor.left = 0.3f; DropdownAnchor.right = 1.0f;
	DropdownAnchor.top = 0.0f; DropdownAnchor.bottom = 1.0f;
	Dropdown->GetTransform()->SetAnchor(DropdownAnchor);
}


