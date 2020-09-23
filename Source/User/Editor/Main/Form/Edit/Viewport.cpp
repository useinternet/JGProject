#include "pch.h"
#include "Viewport.h"
#include "Components/DrawComponent.h"


using namespace std;
using namespace JGUI;

void Viewport::Awake()
{
	//
	SetBackGroundColor(JColor::Black());

	auto LoadWorldEvent = GlobalSharedData::GetEventManager()->EventCall<void, const std::wstring&>(TT("GFW::LoadWorld"));
	if (LoadWorldEvent.IsValid())
	{
		LoadWorldEvent(TT("TestWorld"));
	}
	auto GetWorldCameraEvent = GlobalSharedData::GetEventManager()->EventCall<GE::Scene*>(TT("GFW::GetWorldCamera"));
	if (GetWorldCameraEvent.IsValid())
	{
		m_WorldScene = GetWorldCameraEvent();
	}


	GlobalSharedData::GetEventManager()->EventNotify<void, const std::wstring&, JVector3>(
		TT("D::GFW::SpawnObject(Test)"), TT("../Source/Resource/Test/Model/Shape_Sphere.mesh"), { 0,0,200 });
	GlobalSharedData::GetEventManager()->EventNotify<void, const std::wstring&, JVector3>(
		TT("D::GFW::SpawnObject(Test)"), TT("../Source/Resource/Test/Model/Shape_Sphere.mesh"), { 150,0,200 });
	GlobalSharedData::GetEventManager()->EventNotify<void, const std::wstring&, JVector3>(
		TT("D::GFW::SpawnObject(Test)"), TT("../Source/Resource/Test/Model/Shape_Sphere.mesh"), { -150,0,200 });
}

void Viewport::Start()
{
	
	auto clientRect = GetClientRect();
	m_ViewportImage = CreateElement<Element>(TT("ViewportScene"))->CreateComponent<ImageComponent>();
	m_ViewportImage->SetImage(m_WorldScene->GetSceneTexture());
	m_ViewportImage->GetTransform()->SetSize(clientRect.Width(), clientRect.Height());
	m_ViewportImage->GetTransform()->SetLocalLocation(clientRect.left, clientRect.top);
	m_ViewportImage->GetTransform()->SetAnchor(AnchorPreset::Full_Stretch);



}

void Viewport::Tick()
{
}

void Viewport::Destroy()
{
}
