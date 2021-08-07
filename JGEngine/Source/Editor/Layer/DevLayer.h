#pragma once
#include "JGCore.h"
#include "Class/SystemLayer.h"


namespace JG
{
	class DevLayer : public ISystemLayer

	{
	private:
	public:
		virtual ~DevLayer() = default;
	public:
		virtual void OnAttach() override;
		virtual void OnDetach()  override;
		virtual void Begin() override;
		virtual void Destroy()  override;
		virtual void OnEvent(IEvent& e)  override;
		virtual String GetLayerName()  override;
	};
}