#include "PCH/PCH.h"
#include "WDetailsView.h"
#include "Properties/WProperty_Object.h"
#include "WidgetComponents/WBorder.h"

void WDetailsView::Construct(const HArguments& inArgs)
{
	_detailsObjects = inArgs.DetailsObjects;
}

void WDetailsView::OnGUIBuild(HGUIBuilder& inBuilder)
{
	JG_SUPER::OnGUIBuild(inBuilder);

	inBuilder.BeginVertical();
	//inBuilder.PushWidgetComponent();
	for (PWeakPtr<JGObject> weakObject : _detailsObjects)
	{
		// 콜파즈 헤더 트리
		
		//ImGui::CollapsingHeader()
		if (weakObject.IsValid() == false)
		{
			continue;
		}

	}
	inBuilder.EndVertical();
}