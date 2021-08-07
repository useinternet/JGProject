#pragma once


#include "Event/Events.h"
#include "Command.h"


namespace JG
{
	class ISystemLayer : public ISubscriber
	{
	public:
		virtual ~ISystemLayer() {}
	public:
		virtual void OnAttach() = 0;
		virtual void OnDetach() = 0;

		virtual void Begin()   = 0;
		virtual void Destroy() = 0;

		virtual void OnEvent(IEvent& e) = 0;

		virtual String GetLayerName() = 0;
	};

	
}