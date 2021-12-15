#pragma once
#include "JGCore.h"
#include "UI/UIView.h"


namespace JG
{
	class DevelopView : public UIView
	{
		JGCLASS

	private:
		bool	 mOpenGUI = true;
	public:
		DevelopView();
		virtual ~DevelopView() = default;
	protected:
		virtual void Load() override;
		virtual void Initialize() override;
		virtual void OnGUI() override;
		virtual void Destroy() override;
		virtual void OnEvent(IEvent& e) override;
	private:
		void Statistics_OnGUI();
		void Dev_OnGUI();
	};



}