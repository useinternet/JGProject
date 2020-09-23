#pragma once
#include "HierarchicalComponent.h"




namespace GFW
{
	class CameraComponent : public HierarchicalComponent
	{
	public:
		virtual void Awake()   override;
		virtual void Start()   override;
		virtual void Tick()    override;
		virtual void Destroy() override;
	private:
		GE::SceneCamera m_SceneCamera;
	};
}