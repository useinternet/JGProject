#pragma once
#include "JGCore.h"
#include "UI/UIView.h"


namespace JG
{
	class MaterialView : public UIView
	{
		JGCLASS
	public:
		MaterialView();
		virtual ~MaterialView() = default;
	protected:
		virtual void Load() override;
		virtual void Initialize() override;
		virtual void OnGUI() override;
		virtual void Destroy() override;
		virtual void OnEvent(IEvent& e) override;
	};

}