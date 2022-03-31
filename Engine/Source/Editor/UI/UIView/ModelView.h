#pragma once
#include "JGCore.h"
#include "UI/UIView.h"


namespace JG
{
	class ModelView : public UIView
	{
		JGCLASS
	public:
		ModelView();
		virtual ~ModelView() = default;
	protected:
		virtual void Load() override;
		virtual void Initialize() override;
		virtual void OnGUI() override;
		virtual void Destroy() override;
		virtual void OnEvent(IEvent& e) override;
	};



}