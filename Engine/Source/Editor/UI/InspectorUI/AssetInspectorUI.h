#pragma once
#include "InspectorUI.h"
#include "Class/Asset/Asset.h"

namespace JG
{
	template<class T>
	class Asset;



	class MaterialAssetInspectorUI : public InspectorUI<Asset<class IMaterial>> { virtual void OnGUI_Impl(Asset<IMaterial>* obj) override; };

}