#include "pch.h"
#include "MeshComponent.h"
#include "GFWIF.h"

using namespace std;

namespace GFW
{



	void MeshComponent::Awake()
	{
		PrimitiveComponent::Awake();
		GFWIF::RegisterInstanceWithGraphicsEngine(this);
	}

	void MeshComponent::Start()
	{
		PrimitiveComponent::Start();

		

	}

	void MeshComponent::Tick()
	{
		PrimitiveComponent::Tick();
		



	}

	void MeshComponent::Destroy()
	{
		PrimitiveComponent::Destroy();
		GFWIF::RegisterInstanceWithGraphicsEngine(this);
	}

}