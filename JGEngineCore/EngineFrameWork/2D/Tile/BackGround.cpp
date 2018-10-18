#include"BackGround.h"
#include"../../../EngineStatics/JGConstructHelper.h"
#include"../../Components/StaticMesh2DComponent.h"
#include"../../../RenderSystem/JGViewportD.h"
#include"../../World/World.h"
#include"../../Components/Camera2DComponent.h"
#include"../../../EngineStatics/JMath/JGMath.h"
using namespace std;
BackGround::BackGround()
{
	RegisterComponentID(typeid(this));
	UseDefaultViewMatrix();
	for (int i = 0; i < 2; ++i)
	{
		m_BackGround[i] = make_unique<StaticMesh2DComponent>();
		//m_BackGround[i]->UseDefaultViewMatrix();
	}
	m_ScaleSpeed.Set(0.0f, 0.0f);
}
BackGround::~BackGround()
{

}
void BackGround::BeginComponent(World* world)
{
	Motivated2DComponent::BeginComponent(world);
	float Width = m_BackGround[0]->GetTextureWdith();
	float Height = m_BackGround[0]->GetTextureHeight();
	m_bHorizon ?
		m_BackGround[1]->SetComponentLocation(Width * GetComponentScale().X(), 0.0f) :
		m_BackGround[1]->SetComponentLocation(0.0f, Height * GetComponentScale().Y());
	for (int i = 0; i < 2; ++i)
	{
		m_BackGround[i]->BeginComponent(world);
		m_BackGround[i]->SetOwnerObject(GetOwnerObject());
	}
}
void BackGround::Tick(const float DeltaTime)
{
	Motivated2DComponent::Tick(DeltaTime);
	if (m_CenterVector == nullptr)
	{
		DeltaCameraX = 0.0f;
		DeltaCameraY = 0.0f;
	}
	else
	{
		DeltaCameraX = m_CenterVector->X() - ((GetViewport()->GetWidth() / 2));
		DeltaCameraY = m_CenterVector->Y() - (GetViewport()->GetHeight() / 2);
	}
	for (int i = 0; i < 2; ++i)
	{
		if (IsChangeScale())
		{
			m_BackGround[i]->SetComponentScale(GetComponentScale());
		}
		
	}
	for (int i = 0; i < 2; ++i)
	{
		m_BackGround[i]->SetZOrder(GetZOrder());
		if (IsGetOffWidthDisplay(m_BackGround[i].get()))
		{
			JGVector2D Location = m_BackGround[i]->GetComponentLocalLocation();
			float width = m_BackGround[i]->GetTextureWdith() * GetComponentScale().X();
		
			if (i == 0)
			{
				float ReverseX = m_BackGround[1]->GetComponentLocalLocation().X();
				m_BackGround[i]->SetComponentLocation(width + ReverseX, 0.0f);
			}
			else
			{
				float ReverseX = m_BackGround[0]->GetComponentLocalLocation().X();
				m_BackGround[i]->SetComponentLocation(width + ReverseX, 0.0f);
			}
		}
		if (IsGetOffHeightDisplay(m_BackGround[i].get()))
		{
			JGVector2D Location = m_BackGround[i]->GetComponentLocalLocation();
			float height = m_BackGround[i]->GetTextureHeight() * GetComponentScale().Y();
			m_BackGround[i]->SetComponentLocation(0.0f, (-height) + DeltaCameraY);
		}
	}

	for (int i = 0; i < 2; ++i)
	{
		m_BackGround[i]->AddComponentLocation(
			m_MoveSpeed.X() * m_ScaleSpeed.X(),
			m_MoveSpeed.Y() * m_ScaleSpeed.Y());
	}
}
void BackGround::CreateBackGround(const std::wstring& BackGroundPath, const JGVector2D& MoveSpeed, const bool Horizon)
{
	m_MoveSpeed.Set(MoveSpeed);
	m_MoveSpeed.SetNegativeX();
	m_bHorizon = Horizon;
	m_BackGround[0]->RegisterName(GetComponentName() + TT("_1"));
	m_BackGround[1]->RegisterName(GetComponentName() + TT("_2"));
	JGConstructHelper::StaticMesh2D BackGround_Mesh(m_BackGround[0]->GetComponentName(), EPivot::MiddleMiddle, BackGroundPath);
	if (BackGround_Mesh.Success)
	{
	
		m_BackGround[0]->SetConstructObject(BackGround_Mesh.Object);
		AddChild(m_BackGround[0].get());
	}
	JGConstructHelper::StaticMesh2D BackGround_ReverseMesh(m_BackGround[1]->GetComponentName(), EPivot::MiddleMiddle, BackGroundPath,EReverse::RL);
	if (BackGround_ReverseMesh.Success)
	{
		m_BackGround[1]->SetConstructObject(BackGround_ReverseMesh.Object);
		AddChild(m_BackGround[1].get());
	}
	SetComponentLocation(GetViewport()->GetWidth() / 2, GetViewport()->GetHeight() / 2);
}

void BackGround::Move(const float ScaleX, const float ScaleY)
{
	m_ScaleSpeed.Set(ScaleX, ScaleY);
}

bool BackGround::IsGetOffWidthDisplay(StaticMesh2DComponent* mesh)
{
	JGVector2D Location = mesh->GetComponentLocalLocation();
	if (Location.X() <= -(mesh->GetTextureWdith()*GetComponentScale().X()) + DeltaCameraX)
	{
		return true;
	}
	return false;
}

bool BackGround::IsGetOffHeightDisplay(StaticMesh2DComponent * mesh)
{
	JGVector2D Location = mesh->GetComponentLocalLocation();

	if (Location.Y() >= (mesh->GetTextureHeight() * GetComponentScale().Y()) + DeltaCameraY)
	{
		return true;
	}
	return false;
}

void BackGround::RegistorCenter(JGVector2D* vec)
{
	m_CenterVector = vec;
}
