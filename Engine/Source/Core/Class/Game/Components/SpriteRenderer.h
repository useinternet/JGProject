#pragma once
#include "BaseRenderer.h"

namespace JG
{
	class SpriteRenderer : public BaseRenderer
	{
		JGCLASS
	private:
		//SharedPtr<Standard2DRenderItem> mSpriteRI;

		Color mColor = Color::White();
		JVector2 mSpriteSize;
		Asset<ITexture>* mSprite = nullptr;
		SharedPtr<ScheduleHandle> mPushRenderSceneObjectScheduleHandle = nullptr;
	public:
		virtual ~SpriteRenderer() = default;
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
		void SetColor(const Color& color);

		const Color& GetColor() const;
	public:
		virtual void OnChange(const ChangeData& data) override;
		virtual void OnInspectorGUI() override;

	private:
		EScheduleResult PushRenderSceneObject();
	};

}