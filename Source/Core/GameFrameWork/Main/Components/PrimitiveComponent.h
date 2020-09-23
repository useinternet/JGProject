#pragma once
#include "HierarchicalComponent.h"



namespace GFW
{
	class PrimitiveComponent : public HierarchicalComponent
	{


	public:
		virtual void Awake()   override;
		virtual void Start()   override;
		virtual void Tick()    override;
		virtual void Destroy() override;



	};
}