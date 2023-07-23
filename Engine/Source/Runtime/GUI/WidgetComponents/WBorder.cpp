#include "PCH/PCH.h"
#include "WBorder.h"
#include "Builder/GUIBuilder.h"
#include "GUIStyles/GenericGUIStyle.h"
#include "External/Classes/ImGuiBuildUtility.h"
#include "External/imgui/imgui.h"

WBorder::HArguments::HArguments()
{
	StretchMode = EStretchMode::None;
	BackGroundColor.SetValue(JGGenericGUIStyle::Get().GetStyleColor(JGGenericGUIStyle::Border));
}

void WBorder::Construct(const HArguments& args)
{
	_bHover = false;
	_onContent = args.OnContent;
	_onMouseLeftClick  = args.OnMouseLeftClick;
	_onMouseRightClick = args.OnMouseRightClick;
	_onMouseHovered	   = args.OnMouseHovered;
	_onMouseEnter = args.OnMouseEnter;
	_onMouseLeave = args.OnMouseLeave;
	_backGroundColor   = args.BackGroundColor;
	_stretchMode = args.StretchMode;
}

void WBorder::SetBackGroundColor(const HAttribute<HLinearColor>& inColor)
{
	_backGroundColor = inColor;
}

HLinearColor WBorder::GetBackGroundColor() const
{
	return _backGroundColor.GetValue();
}

void WBorder::OnGUIBuild(HGUIBuilder& inBuilder)
{
	inBuilder.PushGenerateNativeGUI(SharedWrap(this), HOnGenerateNativeGUI::CreateLambda([&](const HWidgetContext& widgetContext)
		{
			ImGui::BeginGroup();

			ImGui::RenderFrame(
				Vector2ToImVec2(_cacheBorderRect.Min()),
				Vector2ToImVec2(_cacheBorderRect.Max()),
				LinearColorToImU32(_backGroundColor.GetValue()));

			widgetContext.PushContent(HOnGUIContent::CreateLambda([this](HGUIBuilder& inBuilder)
				{
					inBuilder.PushWidgetComponent(_onContent.ExecuteIfBound());
				}));

			switch (_stretchMode)
			{
			case EStretchMode::Horizontal:
				ImGui::SameLine(); ImGui::Dummy(ImVec2(widgetContext.ContentSize.x, 0.0f));
				break;

			case EStretchMode::Vertical:
				ImGui::SameLine(); ImGui::Dummy(ImVec2(0.0f, widgetContext.ContentSize.y));
				break;
			}

			ImGui::EndGroup();
			ImVec2 ItemRectMin = ImGui::GetItemRectMin();
			ImVec2 ItemRectMax = ImGui::GetItemRectMax();
			_cacheBorderRect = HRect(HVector2(ItemRectMin.x, ItemRectMin.y), HVector2(ItemRectMax.x, ItemRectMax.y));

			if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
			{
				JG_LOG(GUI, ELogLevel::Info, "Left Click (%d)", GetGuid().GetHashCode());
				_onMouseLeftClick.ExecuteIfBound();
			}

			if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
			{
				JG_LOG(GUI, ELogLevel::Info, "Right Click (%d)", GetGuid().GetHashCode());
				_onMouseRightClick.ExecuteIfBound();
			}
		
			if (ImGui::IsItemHovered())
			{
				if (_bHover == false)
				{
					_bHover = true;
					_onMouseEnter.ExecuteIfBound();
				}

				_onMouseHovered.ExecuteIfBound();
			}
			else
			{
				if (_bHover)
				{
					_bHover = false;
					_onMouseLeave.ExecuteIfBound();
				}
			}

		}));
}
