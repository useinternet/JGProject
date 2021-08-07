#pragma once
#include "Class/Game/GameComponent.h"
#include "Class/Asset/Asset.h"
#include "Class/Asset/AssetManager.h"




namespace JG
{
	class BaseRenderer : public GameComponent
	{
	public:
		virtual ~BaseRenderer() = default;
	};


}