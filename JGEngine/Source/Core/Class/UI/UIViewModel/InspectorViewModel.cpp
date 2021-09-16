#include "pch.h"
#include "InspectorViewModel.h"
#include "Class/Asset/Asset.h"
#include "Class/Game/GameObjectFactory.h"
#include "Class/Game/GameNode.h"
#include "Class/UI/UIModel/InspectorModel.h"
namespace JG
{
	void InspectorViewModel::Initialize()
	{
	}
	void InspectorViewModel::Destroy()
	{
		UIViewModel::Destroy();
	}
}