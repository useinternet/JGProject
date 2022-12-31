#pragma once
#include "Core.h"

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
};
