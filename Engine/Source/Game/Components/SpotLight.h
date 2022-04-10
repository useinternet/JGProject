#pragma once
#include "GameComponent.h"
#include "Class/Asset/Asset.h"
#include "Light.h"

namespace JG
{
	namespace Graphics
	{
		class SpotLight;
		class PaperObject;
	}
	class SpotLight : public Light
	{
		JGCLASS
	private:
		JVector3 mColor     = Color::White();
		JVector3 mDirection = JVector3(0, 0, 1);
		f32	mRange = 0.0f;
		f32 mOutFallOff = 0.0f;
		f32 mFallOff    = 0.0f;
		f32 mIntensity  = 1.0f;
		f32 mAttRange = 0.0f;

		f32 mAtt0;
		f32 mAtt1;
		f32 mAtt2;
	public:
		virtual ~SpotLight() = default;
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
		void SetRange(f32 range);
		f32 GetRange() const;

		void SetOutFallOff(f32 angle);
		f32 GetOutFallOff() const;

		void SetFallOff(f32 angle);
		f32 GetFallOff() const;


		void SetIntensity(f32 intensity);
		f32  GetIntensity() const;

		void  SetColor(const Color& color);
		Color GetColor() const;



		void SetAttRange(f32 attRange, bool auto_calc_att = true);
		f32 GetAttRange() const;

		void SetAtt0(f32 att0);
		f32 GetAtt0() const;

		void SetAtt1(f32 att1);
		f32 GetAtt1() const;

		void SetAtt2(f32 att2);
		f32 GetAtt2() const;

		const JVector3& GetDirection() const;

	private:
		virtual void PushLightItem() override;
	};
}