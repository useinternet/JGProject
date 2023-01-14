#pragma once
#include "Core.h"
#include "Widget.h"
// MenuBuilder
// GUIBuilder

class GGUIGlobalSystem : public GGlobalSystemInstance<GGUIGlobalSystem>
{
	// Icon 包府
	// GUIBuilder 包府
	// MenuBuilder 包府
	// ContextBuilfer 包府



protected:
	virtual void Start() override;
	virtual void Update() override;
	virtual void Destroy() override;

public:
	template<class T>
	PSharedPtr<T> OpenWidget()
	{
		PSharedPtr<T> Widget = Allocate<T>();
		PSharedPtr<WWidget> BaseWidget = Widget;
		BaseWidget->OnConstruct();

		return Widget;
	}
};
