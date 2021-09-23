#pragma once
#include "Class/Game/GameComponent.h"
#include "Class/Asset/Asset.h"
#include "Light.h"

namespace JG
{
	class PointLight : public Light
	{
		JGCLASS
		SharedPtr<PointLightItem> mRI;




#ifdef JG_EDITOR
		SharedPtr<Standard2DRenderItem> mTargetRI;
		Asset<ITexture>* mIcon = nullptr;
#endif // DEBUG

	
	protected:
		virtual void Start() override;
		virtual void Destory() override;
	public:
		virtual void MakeJson(SharedPtr<JsonData> jsonData)   const override;
		virtual void LoadJson(SharedPtr<JsonData> jsonData) override;



	private:
		virtual void OnInspectorGUI() override;
#ifdef JG_EDITOR
		virtual SharedPtr<IRenderItem> PushRenderItem() override;
#endif
		virtual SharedPtr<ILightItem> PushLightItem() override;
	};
}