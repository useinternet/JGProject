#include"Progressbar.h"
#include"../../Components/StaticMesh2DComponent.h"
#include"../../../EngineStatics/JGConstructHelper.h"
#include"../../../EngineStatics/JTimerManager.h"
using namespace std;

map<wstring, ConstructObject*> Progressbar::m_mBarPath;
Progressbar::Progressbar()
{
	RegisterComponentID(typeid(this));
}
Progressbar::~Progressbar()
{

}

void Progressbar::BeginComponent(World* world)
{
	Widget::BeginComponent(world);

	m_FullBar->SetOwnerObject(GetOwnerObject());
	m_EmptyBar->SetOwnerObject(GetOwnerObject());
}

void Progressbar::Tick(const float DeltaTime)
{
	Widget::Tick(DeltaTime);


	if (IsChangeScale())
	{
		m_FullBar->SetComponentScale(GetComponentScale());
		m_EmptyBar->SetComponentScale(GetComponentScale());
	}

}

void Progressbar::CreateProgressBar(const std::wstring& FullBarPath, const std::wstring& EmptyBarPath,EPivot pivot)
{
	m_FullBar = make_unique<StaticMesh2DComponent>();
	m_EmptyBar = make_unique<StaticMesh2DComponent>();

	AddChild(m_FullBar.get());
	AddChild(m_EmptyBar.get());


	m_FullBar->RegisterName(GetComponentName() + TT("FullBar"));
	m_EmptyBar->RegisterName(GetComponentName() + TT("EmptyBar"));

	CreateBarResource(m_FullBar.get(), FullBarPath, pivot);
	CreateBarResource(m_EmptyBar.get(), EmptyBarPath, pivot);

	m_FullBar->SetStartDrawTex(0.0f, 0.0f);
	m_FullBar->SetEndDrawTex(m_Aspect, 1.0f);

	m_EmptyBar->SetStartDrawTex(m_Aspect, 0.0f);
	m_EmptyBar->SetEndDrawTex(1.0f, 1.0f);
}


void Progressbar::SetBindingData(const float Aspect)
{
	m_Aspect = Aspect;

	m_FullBar->SetStartDrawTex(0.0f, 0.0f);
	m_FullBar->SetEndDrawTex(m_Aspect, 1.0f);

	m_EmptyBar->SetStartDrawTex(Aspect, 0.0f);
	m_EmptyBar->SetEndDrawTex(1.0f, 1.0f);
}
void Progressbar::CreateBarResource(StaticMesh2DComponent* Bar, const std::wstring& path, EPivot pivot)
{
	if (ResourceCheck(path))
	{
		Bar->SetConstructObject(m_mBarPath[path]);
	}
	else
	{
		JGConstructHelper::StaticMesh2D ConstructMesh(Bar->GetComponentName(), pivot, path);
		if (ConstructMesh.Success)
		{
			Bar->SetConstructObject(ConstructMesh.Object);
			m_mBarPath.insert(pair<wstring, ConstructObject*>(path, ConstructMesh.Object));
		}
	}
}

bool Progressbar::ResourceCheck(const wstring& CheckPath)
{
	auto iter = m_mBarPath.find(CheckPath);
	if (iter != m_mBarPath.end())
	{
		return true;
	}
	return false;
}
