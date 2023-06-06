#include "PCH/PCH.h"
#include "WBorder.h"
#include "Builder/GUIBuilder.h"
#include "External/Classes/ImGuiBuildUtility.h"
#include "External/imgui/imgui.h"

WBorder::WBorder(const WBorder::HArguments& args)
{
	Construct(args);
}

void WBorder::Construct(const HArguments& args)
{
	_onMouseLeftClick  = args.OnMouseLeftClick;
	_onMouseRightClick = args.OnMouseRightClick;
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
	inBuilder.PushGenerateNativeGUI(SharedWrap(this), HOnGenerateNativeGUI::Create(SharedWrap(this), [&](const HWidgetContext& widgetContext)
		{
			//ImGui::PushStyleColor(ImGuiCol_ChildBg, LinearColorToImVec4(_backGroundColor.GetValue()));
			//    colors[ImGuiCol_Header]                 = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
			//    colors[ImGuiCol_HeaderActive]           = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
			ImGui::BeginGroup();

			ImGui::RenderFrame(
				Vector2ToImVec2(_cacheBorderRect.Min()),
				Vector2ToImVec2(_cacheBorderRect.Max()),
				LinearColorToImU32(_backGroundColor.GetValue()));
			//	ImGui::push

			widgetContext.PushContent(HOnGUIContent::Create(SharedWrap(this), [this](HGUIBuilder& inBuilder)
				{
					OnContent(inBuilder);
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
			auto test1 = ImGui::GetItemRectMin();
			auto test2 = ImGui::GetItemRectMax();
			_cacheBorderRect = HRect(HVector2(test1.x, test1.y), HVector2(test2.x, test2.y));

			//_cacheBorderRect;
		//	ImGui::PopStyleColor()

			

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

		}));
}
