#pragma once
#include "BaseRenderer.h"
#include "Class/Asset/Asset.h"
#include "Class/Asset/AssetManager.h"
namespace JG
{
	class SpriteRenderer : public BaseRenderer
	{
		JGCLASS
	private:
		SharedPtr<Standard2DRenderItem> mSpriteRI;
		JVector2 mSpriteSize;


		Asset<ITexture>* mSprite;
	public:
		virtual ~SpriteRenderer() = default;
	public:
		SpriteRenderer();
	protected:
		virtual void Awake() override;
		virtual void Start() override;
		virtual void Destory() override;
	protected:
		virtual void Update() override;
		virtual void LateUpdate() override;
	public:
		virtual void MakeJson(SharedPtr<JsonData> jsonData)   const override;
		virtual void LoadJson(SharedPtr<JsonData> jsonData) override;
	public:
		void SetSprite(const String& path);
	public:
		virtual SharedPtr<IRenderItem> PushRenderItem() override;
	public:
		virtual void OnChange(const ChangeData& data) override;
		virtual void OnInspectorGUI() override;
	};

}