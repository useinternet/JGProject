#pragma once
#include "GameComponent.h"
#include "Class/Asset/Asset.h"
#include "Light.h"

namespace JG
{
	namespace Graphics
	{
		class DirectionalLight;
		class PaperObject;
	}
	class DirectionalLight : public Light
	{
		JGCLASS

	private:
		JVector3 mColor = Color::White();
		JVector3 mDirection = JVector3(0, 0, 1);
		f32 mIntensity = 1.0f;
	public:
		virtual ~DirectionalLight() = default;
	protected:
		virtual void Awake() override;
		virtual void Start() override;
		virtual void Update() override;
		virtual void FixedUpdate() override;
		virtual void Destory() override;
	public:
		virtual void MakeJson(SharedPtr<JsonData> jsonData)   const override;
		virtual void LoadJson(SharedPtr<JsonData> jsonData) override;
	public:
		void SetIntensity(f32 intensity);
		f32  GetIntensity() const;
		void SetColor(const Color& color);
		const JVector3& GetDirection() const;
		Color    GetColor() const;
	private:
		virtual void PushLightItem() override;
	};
}