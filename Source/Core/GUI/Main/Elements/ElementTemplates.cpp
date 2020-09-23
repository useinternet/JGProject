
#include "pch.h"
#include "Element.h"
#include "Components/Transform.h"
#include "Components/NativeScriptComponent.h"
#include "Components/DrawComponent.h"
#include "Components/LayerComponent.h"
#include "Components/Script/Button.h"
#include "Components/Script/Border.h"
#include "Components/Script/ResizeGrip.h"
#include "ElementTemplates.h"





namespace JGUI
{
	namespace ElementTemplate
	{
		void Template_TitleBar(Element* emptyElement)
		{
			auto transform = emptyElement->GetTransform();
			auto layer = emptyElement->CreateComponent<LayerComponent>(TT("Layer"));
			auto image = emptyElement->CreateComponent<ImageComponent>(TT("Image"));


			auto textElement = emptyElement->CreateElement<Element>(TT("TitleName"));
			auto textMesh = textElement->CreateComponent<TextMeshComponent>(TT("TextMesh"));

			layer->SetLayer(GUIDraw_Priority_TitleBar);
			
			auto& style = GUIIF::GetStyle();

			image->SetColor(style.GUIColors[GUIStyle_TitleBar]);
			image->SetInteractionActive(false);

	
			textMesh->SetTextVAlign(TextVAlign_Center);
			textMesh->SetColor(style.GUIColors[GUIStyle_TitleText]);



			textElement->GetTransform()->SetLocalLocation(style.WindowPadding.x, 0.0f);
			textElement->GetTransform()->SetSize(transform->GetSize());
			textElement->GetTransform()->SetPivot(0.0f, 0.0f);
			textElement->GetTransform()->SetAnchor(AnchorPreset::Full_Stretch);
		}
		void Template_Button(Element* emptyElement)
		{
			emptyElement->CreateComponent<ImageComponent>(TT("Image"));
			emptyElement->CreateComponent<Button>(TT("Button"));

		}
		void Template_ResizeGrip(Element* emptyElement)
		{
			emptyElement->CreateComponent<ShapeComponent>(TT("Image"));
			emptyElement->CreateComponent<ResizeGrip>(TT("ResizeGrip"));
		}
		void Template_Border(Element* emptyElement)
		{
			emptyElement->CreateComponent<ShapeComponent>(TT("Image"));
			emptyElement->CreateComponent<Border>(TT("Border"));
		}
		void Template_Text(Element* emptyElement)
		{
			emptyElement->CreateComponent<TextMeshComponent>(TT("TextMesh"));
		}
		void Template_Image(Element* emptyElement)
		{
			emptyElement->CreateComponent<ImageComponent>(TT("Image"));
		}
	}
}