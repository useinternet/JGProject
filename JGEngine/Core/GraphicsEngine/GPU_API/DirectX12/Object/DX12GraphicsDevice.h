#pragma once


#include "GECore.h"
#include "Core.h"




namespace DX12
{
	class DX12GraphicsDevice : public GE::GraphicsDevice
	{
	public:
		virtual void Init()   override;
		virtual bool Load() override;
		virtual void Update() override;
		virtual void Destroy() override;
	};

}




