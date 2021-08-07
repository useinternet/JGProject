#pragma once
#include "Class/SystemLayer.h"





namespace JG
{
	class ImGuiSystemLayer : public ISystemLayer
	{
	public:
		virtual ~ImGuiSystemLayer() {}
	public:
		virtual void OnAttach() override;
		virtual void OnDetach() override;




		virtual void Begin() override;
		virtual void Destroy() override {}

		virtual void OnEvent(IEvent& e) override;

		virtual String GetLayerName() override;


		EScheduleResult Update();
	private:
		bool Resize(AppResizeEvent& e);
	};
}