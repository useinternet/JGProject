#pragma once
#include "BaseRenderer.h"

namespace JG
{
	class SpriteRenderer : public BaseRenderer
	{
		JGCLASS
	private:
		Color mColor = Color::White();
		JVector2 mSpriteSize;
		Asset<ITexture>* mSprite = nullptr;
		SharedPtr<ScheduleHandle> mRenderSH = nullptr;
	public:
		virtual ~SpriteRenderer() = default;
	protected:
		virtual void Awake() override;
		virtual void Start() override;
		virtual void Destory() override;
	protected:

		virtual void Update() override;
		virtual void LateUpdate() override;
		virtual void FixedUpdate() override;
	public:
		virtual void MakeJson(SharedPtr<JsonData> jsonData)   const override;
		virtual void LoadJson(SharedPtr<JsonData> jsonData) override;
	public:
		void SetSprite(const String& path);
		void SetColor(const Color& color);

		const Color& GetColor() const;
		const Asset<ITexture>* GetSprite() const;
	public:
		virtual void OnChange(const ChangeData& data) override;
	private:
		void PushRenderSceneObject();
	};

}