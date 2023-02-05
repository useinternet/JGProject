#pragma once
#include "WidgetComponents/WidgetComponent.h"
#include "Builder/GUIBuilder.h"
#include "External/imgui/imgui.h"

template<class T>
class WInputScalar : public WWidgetComponent
{
private:
	T _value;
	T _step;

public:
	WInputScalar() : WWidgetComponent() {}
	virtual ~WInputScalar() = default;

	const T& GetInputValue() const 
	{
		return _value;
	}
protected:
	virtual void OnGUIBuild(HGUIBuilder& inBuilder) override
	{
		inBuilder.PushGenerateNativeGUI<WInputScalar<T>>(SharedWrap(this), POnGenerateNativeGUI::Create<WText>(SharedWrap(this), [&](const HWidgetContext& widgetContext)
		{
			bool bIsDirty = false;
			T	 value = _value;

			JGType valueType = JGTYPE(T);
			if (valueType == JGTYPE(float32) ||
				valueType == JGTYPE(float64))
			{
				bIsDirty = ImGui::InputFloat("##", &value, _step);
			}
			else if (valueType == JGTYPE(HVector2))
			{
				bIsDirty = ImGui::InputFloat2("##", (float*)&value);
			}
			else if (valueType == JGTYPE(HVector3))
			{
				bIsDirty = ImGui::InputFloat3("##", (float*)&value);
			}
			else if (valueType == JGTYPE(HVector4))
			{
				bIsDirty = ImGui::InputFloat4("##", (float*)&value);
			}
			else if (valueType == JGTYPE(HVector2Int) ||
				valueType == JGTYPE(HVector2Uint))
			{
				bIsDirty = ImGui::InputInt2("##", (int32*)&value, _step);
			}
			else if (valueType == JGTYPE(HVector3Int) ||
				valueType == JGTYPE(HVector3Uint))
			{
				bIsDirty = ImGui::InputInt3("##", (int32*)&value, _step);
			}
			else if (valueType == JGTYPE(HVector4Int) ||
				valueType == JGTYPE(HVector4Uint))
			{
				bIsDirty = ImGui::InputInt4("##", (int32*)&value, _step);
			}
			else if (valueType == JGTYPE(int32) ||
				valueType == JGTYPE(int64) ||
				valueType == JGTYPE(uint32) ||
				valueType == JGTYPE(uint64))
			{
				bIsDirty = ImGui::InputInt2("##", &value, _step);
			}

			if (bIsDirty == true)
			{
				_value = value;
			}
		}));
	}
};