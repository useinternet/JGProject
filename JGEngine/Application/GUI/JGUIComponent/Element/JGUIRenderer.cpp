#include "pch.h"
#include "JGUIRenderer.h"

#include "GUI/JGUIObject/JGUIWindow.h"
#include "GUI/JGUIObject/JGUIElement.h"


#include "GUI/JGUIComponent/JGUICamera.h"
#include "GUI/JGUIComponent/Element/JGUICanvas.h"




#include "GUI/JGUIFont/JGUIFontManager.h"




#include "RenderEngine.h"
#include "Object/ReObject/ReMaterial.h"
#include "Object/ReObject/RenderItem.h"
#include "Object/ReObject/ReMesh.h"
#include "Object/DxObject/Resource.h"
#include "Object/Shader/ShaderModule.h"


void JGUIRenderer::Awake()
{
	FindCamera();
}

void JGUIRenderer::Start()
{


}

void JGUIRenderer::Destroy()
{
	DestroyRI();
}

void JGUIRenderer::Tick(float tick)
{
	if (m_OwnerCanvas == nullptr)   FindCanvas();
	if(m_OwnerCamera == nullptr) 	FindCamera();

	if (m_RenderItem == nullptr)    return;
	if (m_OwnerCanvas == nullptr)   return;
	if (m_OwnerCamera == nullptr)   return;
	JGUITransform* transform = nullptr;
	if (GetOwner())
	{
		transform = GetOwner()->GetTransform();
	}

	LayerProcess();

	JVector2 camera_size = m_OwnerCamera->GetSize();

	if (transform->IsDirty() ||
		(camera_size != m_PrevCameraSize))
	{
		m_PrevCameraSize = camera_size;
		JVector2 pos = transform->GetPosition();






		pos += m_Offset;

		auto angle = transform->GetAngle();
		auto scale = transform->GetScale();
		pos = m_OwnerCamera->ConvertScreenToWorldPos(pos);

	

		auto pivot = transform->GetPivot();
		pivot.x = 1.0f - (pivot.x + 0.5f);
		pivot.y -= 0.5f;

		auto shape_size = transform->GetSize();
		pos.x += (shape_size.x * pivot.x);
		pos.y += (shape_size.y * pivot.y);


		if (m_OwnerCanvas->GetCanvasFlags() & JGUI_CanvasFlag_Overlay)
		{
			auto view_pos = GetOwner()->GetOwnerWindow()->GetCamera()->GetPosition();
			pos.x += view_pos.x;
			pos.y += view_pos.y;
		}
		// 매트릭스계산
		auto t = JMatrix::Translation(JVector3(pos.x, pos.y, 0.0f));
		auto r = JMatrix::Rotation(JQuaternion::ToQuaternion({ 0.0f,0.0f, angle }));
		auto s = JMatrix::Scaling(JVector3(scale.x, scale.y, 1.0f));

		auto m = s * r * t;
		if (m_Instance)
		{
			m_Instance->Set("World", JMatrix::Transpose(m));
		}
		transform->Flush();


		if (m_RenderItem && m_RenderItem->GetMaterial())
		{

			auto mat      = GetRI()->GetMaterial();
			float width   = m_ClipRect.width();
			float height  = m_ClipRect.height();
			JGUIRect rect = m_ClipRect;

			JVector2 lt = { rect.left, rect.top };
			JVector2 rb = { rect.right, rect.bottom };
	
			//lt += transform->GetPosition();
			//rb += transform->GetPosition();
			//lt += m_Offset;
			//rb += m_Offset;

			lt = m_OwnerCamera->ConvertScreenToWorldPos(lt);
			rb = m_OwnerCamera->ConvertScreenToWorldPos(rb);

			//lt.x += (shape_size.x * pivot.x);
			//lt.y += (shape_size.y * pivot.y);
			//rb.x += (shape_size.x * pivot.x);
			//rb.y += (shape_size.y * pivot.y);
			if (m_OwnerCanvas->GetCanvasFlags() & JGUI_CanvasFlag_Overlay)
			{
				auto view_pos = GetOwner()->GetOwnerWindow()->GetCamera()->GetPosition();
				lt.x += view_pos.x;
				lt.y += view_pos.y;

				rb.x += view_pos.x;
				rb.y += view_pos.y;
			}
			mat->SetValueAsFloat2("ClipX", { lt.x, rb.x });
			mat->SetValueAsFloat2("ClipY", { lt.y, rb.y });
			mat->SetValueAsInt("Clip", (int)m_ClipFlags);
		}
	}
}

void JGUIRenderer::ChangeParent(JGUIObject* prev_parent)
{
	FindCanvas();
	FindCamera();
}

void JGUIRenderer::LayerProcess()
{
	if (GetOwner())
	{
		m_RenderItem->SetLayer(m_OwnerCanvas->GetLayer());
		m_RenderItem->SetPriority(GetOwner()->GetRISortingOrder());
	}
}

void JGUIRenderer::SetActive(bool active)
{
	JGUIComponent::SetActive(active);
	if (m_RenderItem)  m_RenderItem->SetActive(IsActive());
}


void JGUIRenderer::SetClipFlags(EJGUI_Clip_Flags flags)
{
	m_ClipFlags = flags;
}

void JGUIRenderer::DestroyRI()
{
	if (m_RenderItem)
	{
		m_Instance = nullptr;
		RE::RenderEngine::DestroyRenderItem(m_RegisterID, m_RenderItem);
		m_RenderItem = nullptr;
	}
}

void JGUIRenderer::CreateRI()
{
	if (m_OwnerCamera == nullptr) return;
	if (m_RenderItem)
	{
		DestroyRI();
	}
	m_RegisterID = m_OwnerCamera->GetOwnerID();
	m_RenderItem = RE::RenderEngine::CreateRenderItem(m_RegisterID,
		RE::ERenderItemUsage::GUI, GetName() + "_RI");
	if (m_RenderItem)
	{
		if(GetOwner()) m_RenderItem->SetPriority(GetOwner()->GetRISortingOrder());
		if(m_OwnerCanvas) m_RenderItem->SetLayer(m_OwnerCanvas->GetLayer());
		m_Instance = m_RenderItem->AddInstance();
		m_RenderItem->SetActive(IsActive());
	}
}

void JGUIRenderer::FindCanvas()
{
	if (GetOwner())
	{
		m_OwnerCanvas = GetOwner()->FindComponent<JGUICanvas>();

		if (m_OwnerCanvas == nullptr)
		{
			auto parent = GetOwner()->GetParent();
			while (parent != nullptr)
			{
				m_OwnerCanvas = parent->FindComponent<JGUICanvas>();
				if (m_OwnerCanvas)
				{
					break;
				}
				parent = parent->GetParent();
			}
		}
		else
		{
			ENGINE_LOG_ERROR("{0} is not exsit in Canvas", GetName());
		}
	}
}

void JGUIRenderer::FindCamera()
{
	auto prev_camera = m_OwnerCamera;
	if (GetOwner())
	{
		m_OwnerCamera = GetOwner()->FindComponent<JGUICamera>();

		if (m_OwnerCamera == nullptr)
		{
			auto parent = GetOwner()->GetParent();
			while (parent != nullptr)
			{
				m_OwnerCamera = parent->FindComponent<JGUICamera>();
				if (m_OwnerCamera)
				{
					break;
				}
				parent = parent->GetParent();
			}
		}

		if (m_OwnerCamera == nullptr)
		{
			m_OwnerCamera = GetOwner()->GetOwnerWindow()->FindComponent<JGUICamera>();
			if (m_OwnerCamera == nullptr)
			{
				ENGINE_LOG_ERROR("{0} is not exsit in Camera", GetName());
			}

		}
	}

	if (prev_camera != m_OwnerCamera)
	{
		if (m_RenderItem)
		{
			DestroyRI();
		}
		CreateRI();
	}
}
void JGUIShapeRenderer::Awake()
{
	JGUIRenderer::Awake();
}
void JGUIShapeRenderer::Destroy()
{
	JGUIRenderer::Destroy();
}
void JGUIShapeRenderer::Tick(float tick)
{
	if (m_RenderItem)
	{
		JVector2 size;
		if (GetOwner())
		{
			size = GetOwner()->GetTransform()->GetSize();
		}
		if (m_PrevSize != size)
		{
			switch (m_ShapeType)
			{
			case Rectangle:
				m_RenderItem->SetMesh(RE::ReGuiMesh::CreateFillRect(size.x, size.y));
				break;
			case EmptyRectangle:
				m_RenderItem->SetMesh(RE::ReGuiMesh::CreateEmptyRect(size.x, size.y, m_Thick));
				break;
			case Circle:
				m_RenderItem->SetMesh(RE::ReGuiMesh::CreateFillRect(size.x, size.y));
				break;
			}
			m_PrevSize = size;
		}
		if (m_RenderItem->GetMaterial() == nullptr) Init();
	}
	JGUIRenderer::Tick(tick);
}
void JGUIShapeRenderer::ChangeParent(JGUIObject* prev_parent)
{
	JGUIRenderer::ChangeParent(prev_parent);
	Init();
}
void JGUIShapeRenderer::Init()
{
	JVector2 size;
	if (GetOwner())
	{
		if (m_RenderItem) m_RenderItem->SetPriority(GetOwner()->GetRISortingOrder());
		size = GetOwner()->GetTransform()->GetSize();
	}
	if (m_RenderItem)
	{
		m_RenderItem->SetMaterial(RE_GUI_DefaultMaterial);
		m_RenderItem->GetMaterial()->SetValueAsFloat4("Color", m_Color);
		switch (m_ShapeType)
		{
		case Rectangle:
			m_RenderItem->SetMesh(RE::ReGuiMesh::CreateFillRect(size.x, size.y));
			break;
		case EmptyRectangle:
			m_RenderItem->SetMesh(RE::ReGuiMesh::CreateEmptyRect(size.x, size.y, m_Thick));
			break;
		case Circle:
			m_RenderItem->SetMesh(RE::ReGuiMesh::CreateFillRect(size.x, size.y));
			break;
		}
	}
}
void JGUIShapeRenderer::SetColor(const JColor& color)
{
	m_Color = color;
	if (m_RenderItem && m_RenderItem->GetMaterial())
	{
		m_RenderItem->GetMaterial()->SetValueAsFloat4("Color", m_Color);
	}
}
void JGUIShapeRenderer::SetColor(float r, float g, float b, float a)
{
	SetColor({ r,g,b,a });
}

void JGUIShapeRenderer::CreateRectangle()
{
	m_ShapeType = Rectangle;
	if (m_RenderItem)
	{
		JVector2 size = GetOwner()->GetTransform()->GetSize();
		m_RenderItem->SetMesh(RE::ReGuiMesh::CreateFillRect(size.x, size.y));
	}
}

void JGUIShapeRenderer::CreateEmptyRectangle(float thick)
{
	m_ShapeType = EmptyRectangle;
	m_Thick     = thick;
	if (m_RenderItem)
	{
		JVector2 size = GetOwner()->GetTransform()->GetSize();
		m_RenderItem->SetMesh(RE::ReGuiMesh::CreateEmptyRect(size.x, size.y, m_Thick));
	}
}

const JColor& JGUIShapeRenderer::GetColor() const
{
	return m_Color;
}


void JGUIImageRenderer::Awake()
{
	JGUIRenderer::Awake();
}

void JGUIImageRenderer::Destroy()
{
	JGUIRenderer::Destroy();
}

void JGUIImageRenderer::Tick(float tick)
{
	if (m_RenderItem)
	{
		JVector2 size;
		if (GetOwner())
		{
			size = GetOwner()->GetTransform()->GetSize();
		}
		if (m_PrevSize != size)
		{
			m_RenderItem->SetMesh(RE::ReGuiMesh::CreateFillRect(size.x, size.y));
			m_PrevSize = size;
		}
		if (m_RenderItem->GetMaterial() == nullptr) Init();
	}
	JGUIRenderer::Tick(tick);
}

void JGUIImageRenderer::ChangeParent(JGUIObject* prev_parent)
{
	JGUIRenderer::ChangeParent(prev_parent);
	Init();
}

void JGUIImageRenderer::Init()
{
	JVector2 size;
	if (GetOwner())
	{
		if (m_RenderItem) m_RenderItem->SetPriority(GetOwner()->GetRISortingOrder());
		size = GetOwner()->GetTransform()->GetSize();
	}
	if (m_RenderItem)
	{
		if (m_Image == "none")
		{
			m_RenderItem->SetMaterial(RE_GUI_DefaultMaterial);
			m_RenderItem->GetMaterial()->SetValueAsFloat4("Color", m_Color);
		}
		else
		{
			m_RenderItem->SetMaterial(RE_GUI_OneTextureDefault);
			m_RenderItem->GetMaterial()->SetValueAsFloat4("Color", m_Color);
			m_RenderItem->GetMaterial()->SetTexture("Image", m_Image);
		}
		m_RenderItem->SetMesh(RE::ReGuiMesh::CreateFillRect(size.x, size.y));
	}

}


void JGUIImageRenderer::SetColor(const JColor& color)
{
	m_Color = color;
	if (m_RenderItem && m_RenderItem->GetMaterial())
	{
		m_RenderItem->GetMaterial()->SetValueAsFloat4("Color", m_Color);
	}
}
void JGUIImageRenderer::SetColor(float r, float g, float b, float a)
{
	SetColor({ r,g,b,a });
}
void JGUIImageRenderer::SetImage(const std::string& name)
{
	m_Image = name;
	Init();
}

const JColor& JGUIImageRenderer::GetColor() const
{
	return m_Color;
}


void JGUIWindowRenderer::Init()
{
	JVector2 size;
	if (GetOwner())
	{
		if(m_RenderItem) m_RenderItem->SetPriority(0);
		
		size = GetOwner()->GetTransform()->GetSize();
	}
	if (m_RenderItem)
	{
		if (m_Image == "none")
		{
			m_RenderItem->SetMaterial(RE_GUI_DefaultMaterial);
			m_RenderItem->GetMaterial()->SetValueAsFloat4("Color", m_Color);
		}
		else
		{
			m_RenderItem->SetMaterial(RE_GUI_OneTextureDefault);
			m_RenderItem->GetMaterial()->SetValueAsFloat4("Color", m_Color);
			m_RenderItem->GetMaterial()->SetTexture("Image", m_Image);
		}
		m_RenderItem->SetMesh(RE::ReGuiMesh::CreateFillRect(size.x, size.y));
	}


}
void JGUIWindowRenderer::Tick(float tick)
{
	if (GetOwner()->GetOwnerWindow()->GetParent() == nullptr)
	{
		DestroyRI();
		m_OwnerCamera = nullptr;
		m_PrevParent  = nullptr;
	}
	else
	{
		if (m_PrevParent != GetOwner()->GetOwnerWindow()->GetParent())
		{
			FindCamera();
			Init();
			m_PrevParent = GetOwner()->GetOwnerWindow()->GetParent();
		}
	}


	JGUIImageRenderer::Tick(tick);

}
void JGUIWindowRenderer::LayerProcess()
{
	if (GetOwner())
	{
		m_RenderItem->SetLayer(GetOwner()->GetOwnerWindow()->GetPriority());
	}
}

void JGUIWindowRenderer::FindCamera()
{
	auto prev_camera = m_OwnerCamera;
	if (GetOwner()->GetOwnerWindow()->GetParent())
	{
		m_OwnerCamera = GetOwner()->GetOwnerWindow()->GetParent()->FindComponent<JGUICamera>();
		m_PrevParent = GetOwner()->GetOwnerWindow()->GetParent();
	}
	else
	{
		if (m_RenderItem)
		{
			m_OwnerCamera = nullptr;
			DestroyRI();
		}
	}

	if (m_OwnerCamera == nullptr && GetOwnerWindow()->GetParent())
	{
		ENGINE_LOG_ERROR("{0} is not exsit in Camera", GetName());
	}
	if (prev_camera != m_OwnerCamera)
	{
		if (m_RenderItem)
		{
			DestroyRI();
		}
		CreateRI();
	}
}

