#pragma once
#include "Class/Game/GameComponent.h"


namespace JG
{
	template<class T>
	class Asset;
	class ITexture;
	class IMesh;




	class SkyDome : public GameComponent
	{
		JGCLASS

		//Asset<IMesh>* mMesh = nullptr;

		SharedPtr<IMesh> mMesh = nullptr;
		SharedPtr<IMaterial> mMaterial = nullptr;
	public:
		virtual ~SkyDome() = default;
	protected:
		virtual void Awake() override;
		virtual void Start() override;
		virtual void Destory() override;

		virtual SharedPtr<IRenderItem> PushRenderItem();
	protected:
		virtual void Update() override;
		virtual void LateUpdate() override;
		virtual void MakeJson(SharedPtr<JsonData> jsonData) const override;
		virtual void LoadJson(SharedPtr<JsonData> jsonData) override;
		virtual void OnInspectorGUI() override;
	private:
		void CreateGeometry();
	};
}