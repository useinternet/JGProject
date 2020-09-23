#include "pch.h"

#include "DrawComponent.h"
#include "Elements/Element.h"
#include "Windows/Window.h"
#include "LayerComponent.h"
using namespace std;


namespace JGUI
{
	void DrawComponent::Awake()
	{
		MeshGenerator::Quad(0.0F, 0.0F, 0.0f, m_V, m_I);
		m_SceneObjectRef = GraphicsIF::RequestSceneObject();
		m_SceneObjectRef->SetMeshType(GE::MeshType::Dynamic);
		m_SceneObjectRef->BindInstanceData(&m_Instance, 1);
		SetColor(JColor::White());
	}
	void DrawComponent::Start()
	{
		
	}

	void DrawComponent::Tick()
	{
		if (GetOwner()->GetOwnerWindow() == nullptr) return;
		



		if (m_V.size() != m_PrevVertexCount || m_I.size() != m_PrevIndexCount)
		{
			m_PrevVertexCount = m_V.size();
			m_PrevIndexCount  = m_I.size();
			m_SceneObjectRef->BindMesh(m_V.data(), m_V.size(), m_I.data(), m_I.size());
		}
		

		auto pivot = GetOwner()->GetTransform()->GetPivot();
		auto size  = GetOwner()->GetTransform()->GetSize();

		if (IsDirty())
		{
			float pivot_x = size.x * pivot.x;
			float pivot_y = size.y * pivot.y;

			JRect localRect(0.0f, 0.0f, size.x, size.y);
			localRect.left   -= pivot_x;
			localRect.right  -= pivot_x;
			localRect.top    -= pivot_y;
			localRect.bottom -= pivot_y;

			localRect.top    = -localRect.top;
			localRect.bottom = -localRect.bottom;
			ConvertVertexCoordinate(localRect);
			ClearDirty();
		}

		m_Instance.world = GetOwner()->GetTransform()->GetWorldMatrix();
		if (GetOwner()->GetFlags() & ElementFlag_TopMost)
		{
			m_DrawPriority = GUIDraw_Priority_TopMostElement;
		}
		else if (m_IsAutoPriority)
		{
			if (GetOwner()->m_LayerComponent) m_DrawPriority = GetOwner()->m_LayerComponent->IssueLayer();
			else m_DrawPriority = 0;
		}

		GUIDrawItem item;
		item.object   = m_SceneObjectRef.Get();
		item.priority = m_DrawPriority;
		GetOwner()->GetOwnerWindow()->PushDrawItem(item);
		if (m_IsInteractionActive)
		{
			GUIInteractionItem ITitem;
			ITitem.draw_priority = item.priority;
			ITitem.interactionFunc = [&]() -> bool
			{
				return InteractionActive();
			};
			ITitem.ownerElement = GetOwner();
			GetOwnerWindow()->RegisterInteractionItem(ITitem);
		}
		else GetOwnerWindow()->UnRegisterInteractionItem(GetOwner());
	}

	void DrawComponent::Destroy()
	{

		if(GetOwnerWindow())
			GetOwnerWindow()->UnRegisterInteractionItem(GetOwner());
		
		//m_SceneObjectRef.Reset();
	}
	void DrawComponent::SetColor(const JColor& color)
	{
		m_Color = color;
		if (m_SceneObjectRef.IsValid())
		{
			if (m_SceneObjectRef->GetMaterial())
			{
				auto mat = m_SceneObjectRef->GetMaterial();
				mat->SetVarAsFloat4(GUI_MATERIAL_DATA_COLOR, JVector4(color.r, color.g, color.b, color.a));
			}
		}
	}

	void DrawComponent::ConvertVertexCoordinate(const JRect& localRect)
	{
		m_V[0].position = { localRect.left,  localRect.top, 0.0f };
		m_V[1].position = { localRect.right, localRect.top, 0.0f };
		m_V[2].position = { localRect.right, localRect.bottom, 0.0f };
		m_V[3].position = { localRect.left,  localRect.bottom, 0.0f };
	}

	bool DrawComponent::InteractionActive()
	{
		if (GetOwner()->GetOwnerWindow() == nullptr) return false;
		else return GetOwner()->GetTransform()->GetRect().Contain(GetOwnerWindow()->GetMousePos());
	}

	bool DrawComponent::IsDirty()
	{
		return m_PrevPivot != GetOwner()->GetTransform()->GetPivot() || m_PrevSize != GetOwner()->GetTransform()->GetSize();
	}

	void DrawComponent::ClearDirty()
	{
		m_PrevPivot = GetOwner()->GetTransform()->GetPivot();
		m_PrevSize = GetOwner()->GetTransform()->GetSize();
	}





	void ImageComponent::Awake()
	{
		DrawComponent::Awake();
		SetImage(TT("none"));
	}

	void ImageComponent::SetImage(GE::Texture* t)
	{
		m_ImageSource = to_wstring((size_t)t);
		m_SceneObjectRef->BindMaterial(GUIIF::GetGUIMaterialRef(GUI_PreGenerated_Material_Image).Get());
		m_SceneObjectRef->GetMaterial()->BindTexture(GUI_MATERIAL_DATA_IMAGE, t);
	}

	void ImageComponent::SetImage(const std::wstring& imagePath)
	{
		auto prevSource = m_ImageSource;
		m_ImageSource = imagePath;
		if (imagePath == TT("none"))
		{
			m_SceneObjectRef->BindMaterial(GUIIF::GetGUIMaterialRef(GUI_PreGenerated_Material_Default).Get());
		}
		else
		{
			m_SceneObjectRef->BindMaterial(GUIIF::GetGUIMaterialRef(GUI_PreGenerated_Material_Image).Get());
			m_SceneObjectRef->GetMaterial()->BindTexture(GUI_MATERIAL_DATA_IMAGE, GraphicsIF::LoadTexture(m_ImageSource).Get());
		}
		if (prevSource == TT("none") && imagePath != TT("none") || prevSource != TT("none") && imagePath == TT("none"))
		{
			SetColor(m_Color);
		}
	}

	GE::Texture* ImageComponent::GetTexture() const
	{
		if (m_ImageSource == TT("none")) return nullptr;
		auto bindTextures = m_SceneObjectRef->GetMaterial()->GetBindedTextures();
		for (auto& t_pair : bindTextures)
		{
			if (t_pair.first == GUI_MATERIAL_DATA_IMAGE) return t_pair.second;
		}

		return nullptr;
	}




	void ShapeComponent::Awake()
	{
		DrawComponent::Awake();
		m_SceneObjectRef->BindMaterial(GUIIF::GetGUIMaterialRef(GUI_PreGenerated_Material_Default).Get());
	}
	void ShapeComponent::ConvertVertexCoordinate(const JRect& localRect)
	{
		if (m_ConvertVertexCoordinate)
		{
			m_ConvertVertexCoordinate(localRect, m_V, m_I);
		}
	}
	bool ShapeComponent::InteractionActive()
	{
		if (m_InteractionFunc) return m_InteractionFunc(GetOwner()->GetTransform()->GetRect());
		else return false;
	}

	void ShapeComponent::ClearDirty()
	{
		if (m_ConvertVertexCoordinate)
		{
			DrawComponent::ClearDirty();
		}
	}




	void TextMeshComponent::Awake()
	{
		DrawComponent::Awake();
		SetInteractionActive(false);
		m_SceneObjectRef->BindMaterial(GUIIF::GetGUIMaterialRef(GUI_PreGenerated_Material_Text).Get());
		SetFont(m_FontPath);
	
	}
	void TextMeshComponent::Tick()
	{
		if (m_Text.empty()) return;
		DrawComponent::Tick();

	}
	void TextMeshComponent::ConvertVertexCoordinate(const JRect& localRect)
	{
		if (m_UpdateItem.State == UpdateItemState::Waiting)
		{
			if (m_Text.empty())
			{
				m_UpdateItem.State = UpdateItemState::Compelete;
			}
			else
			{
				m_UpdateItem.Font = m_Font;
				m_UpdateItem.Text = m_Text;
				m_UpdateItem.TextSize = m_TextSize;
				m_UpdateItem.localRect = localRect;
				m_UpdateItem.hAlign = m_HAlign;
				m_UpdateItem.vAlign = m_VAlign;
				m_UpdateItem.lineSpacing = m_LineSpacing;
				m_UpdateItem.tabSize = m_TabSize;
				m_UpdateItem.State = UpdateItemState::Run;
				m_UpdateTextTask.run([&]()
				{
					UpdateTextMesh(m_UpdateItem);
				});
			}

			m_IsTextDirty = false;

		}
	}
	bool TextMeshComponent::IsDirty()
	{
		if (m_UpdateItem.State == UpdateItemState::Compelete)
		{
			m_V = move(m_UpdateItem.v);
			m_I = move(m_UpdateItem.i);
			m_UpdateItem.State = UpdateItemState::Waiting;
			m_SceneObjectRef->BindMesh(m_V.data(), m_V.size(), m_I.data(), m_I.size());
		}
		return DrawComponent::IsDirty() || m_IsTextDirty;
	}
	void TextMeshComponent::ClearDirty()
	{
		DrawComponent::ClearDirty();
	}
	void TextMeshComponent::SetText(const std::wstring& str)
	{
		m_IsTextDirty = true;
		m_Text = str;
	}
	void TextMeshComponent::SetFont(const std::wstring& path)
	{
		m_IsTextDirty = true;
		m_FontPath = path;
		m_Font = GUIIF::LoadFont(path);
		if (m_Font == nullptr)
		{
			GUILOG_ERROR("%s 폰트 로드에 실패하였습니다.", path.c_str()); return;
		}
		
		auto&  fileInfo = m_Font->GetFileInfo();
		if (m_SceneObjectRef.IsValid())
		{
			m_SceneObjectRef->GetMaterial()->BindTexture(
				GUI_MATERIAL_DATA_IMAGE, 
				GraphicsIF::LoadTexture(fileInfo.page.at(0)).Get());
		}
	}
	void TextMeshComponent::SetTextHAlign(TextHAlign align)
	{
		m_IsTextDirty = true;
		m_HAlign = align;
	}
	void TextMeshComponent::SetTextVAlign(TextVAlign align)
	{
		m_IsTextDirty = true;
		m_VAlign = align;
	}
	void TextMeshComponent::SetLineSpacing(float spacing)
	{
		m_IsTextDirty = true;
		m_LineSpacing = spacing;
	}
	void TextMeshComponent::SetTabSize(float tabSize)
	{
		m_IsTextDirty = true;
		m_TabSize = tabSize;
	}
	void TextMeshComponent::SetTextSize(float size)
	{
		m_IsTextDirty = true;
		m_TextSize = size;
	}
	void TextMeshComponent::UpdateTextMesh(UpdateItem& item)
	{
		struct lineInfo
		{
			float    width;
			uint32_t pos;
		};
		map<int, lineInfo> WidthByLine;
		int   line = 0;
		float lineWidth = 0.0f;

		float xStart = item.localRect.left;
		float yStart  = item.localRect.top;

		float xOff = xStart;
		float yOff = yStart;

		uint32_t iOff = 0;
		uint32_t len = (uint32_t)item.Text.length();
		auto& fileInfo = item.Font->GetFileInfo();
		float size_ratio = (float)item.TextSize / (float)fileInfo.default_font_size;
		float lineHeight = size_ratio * fileInfo.lineHeight;
		for (uint32_t i = 0; i < len; ++i)
		{
			
			if (item.Text[i] == '\n')
			{
				// 라인 초기화
				WidthByLine[line].width = lineWidth;
				WidthByLine[line].pos = i;
				++line; lineWidth = 0.0f;
				

				// yOffset
				yOff -= (item.lineSpacing + lineHeight);
				xOff = xStart;
				continue;
			}
			if (item.Text[i] == '\t')
			{
				xOff      += item.tabSize;
				lineWidth += item.tabSize;
				continue;
			}
			auto& charInfo = item.Font->GetCharInfo(item.Text[i]);
			if (xOff + (float)charInfo.xadvance * size_ratio > item.localRect.right)
			{

				// 라인 초기화
				WidthByLine[line].width = lineWidth;
				WidthByLine[line].pos = i;
				++line; lineWidth = 0.0f;


				// yOffset
				yOff -= (item.lineSpacing + lineHeight);
				xOff = xStart;
			}



		
			float xoffset = charInfo.xoffset * size_ratio;
			float yoffset = charInfo.yoffset * size_ratio;

			float w = charInfo.width * size_ratio;
			float h = charInfo.height * size_ratio;
			float hw = w * 0.5f;
			float hh = h * 0.5f;
			float tleft = charInfo.x / fileInfo.scaleW;
			float ttop = charInfo.y / fileInfo.scaleH;
			float tright = tleft + (charInfo.width / fileInfo.scaleW);
			float tbottom = ttop + (charInfo.height / fileInfo.scaleH);

			float xpos = xOff + xoffset;
			float ypos = yOff - yoffset;

			std::vector<PaperVertex> vv(4);

			vv[0].position = { xpos     ,ypos - h ,  0.0f };
			vv[1].position = { xpos     ,ypos     ,  0.0f };
			vv[2].position = { xpos + w ,ypos     ,  0.0f };
			vv[3].position = { xpos + w ,ypos - h ,  0.0f };


			vv[0].texcoord = { tleft , tbottom };
			vv[1].texcoord = { tleft , ttop };
			vv[2].texcoord = { tright, ttop };
			vv[3].texcoord = { tright, tbottom };

			vector<uint32_t> ii(6);

			ii[0] = 0 + iOff; ii[1] = 1 + iOff; ii[2] = 2 + iOff;
			ii[3] = 0 + iOff; ii[4] = 2 + iOff; ii[5] = 3 + iOff;

			iOff += 4;
			xOff += (float)charInfo.xadvance * size_ratio;
	
	
			lineWidth += (float)charInfo.xadvance * size_ratio;
			item.v.insert(item.v.end(), vv.begin(), vv.end());
			item.i.insert(item.i.end(), ii.begin(), ii.end());
		}
		WidthByLine[line].width = lineWidth;
		WidthByLine[line].pos   = len;
		++line;



		int textPos = 0;
		int idx_off = 0;
		float totalHeight = (lineHeight * line) + (item.lineSpacing * (line -1));
		float yoff = item.localRect.Height() - totalHeight;
		float hyoff = yoff * 0.5f;
		for (auto& line_pair : WidthByLine)
		{
			auto& line      = line_pair.first;
			auto& line_Info = line_pair.second;
			float xoff = item.localRect.Width() - line_Info.width;
			float hxoff = xoff * 0.5f;
			for (int i = textPos; i < line_Info.pos; ++i)
			{
				if (item.Text[i] == '\t') {
					++idx_off; continue;
				}
				int idx = (i - idx_off) * 4;
				switch (item.hAlign)
				{
				case TextHAlign_Right:
					item.v[idx].position.x     += xoff;
					item.v[idx + 1].position.x += xoff;
					item.v[idx + 2].position.x += xoff;
					item.v[idx + 3].position.x += xoff; break;
				case TextHAlign_Center:
					item.v[idx].position.x     += hxoff;
					item.v[idx + 1].position.x += hxoff;
					item.v[idx + 2].position.x += hxoff;
					item.v[idx + 3].position.x += hxoff; break;
				}
				switch (item.vAlign)
				{
				case TextVAlign_Bottom:
					item.v[idx].position.y     -= yoff;
					item.v[idx + 1].position.y -= yoff;
					item.v[idx + 2].position.y -= yoff;
					item.v[idx + 3].position.y -= yoff; break;
				case TextVAlign_Center:
					item.v[idx].position.y     -= hyoff;
					item.v[idx + 1].position.y -= hyoff;
					item.v[idx + 2].position.y -= hyoff;
					item.v[idx + 3].position.y -= hyoff; break;
				}
			}
			int idx = line_Info.pos;
			if (item.Text[idx] == '\n')
			{
				++idx; ++idx_off;
			}
			textPos += idx;
		}

		item.State = UpdateItemState::Compelete;
	}
}