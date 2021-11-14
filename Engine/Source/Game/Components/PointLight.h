#pragma once
#include "GameComponent.h"
#include "Class/Asset/Asset.h"
#include "Light.h"

namespace JG
{
	namespace Graphics
	{
		class PointLight;
		class PaperObject;
	}
	class PointLight : public Light
	{
		JGCLASS
	    JVector3 mColor = Color::White();
		f32 mIntensity = 1.0f;
		f32 mRange = 0.0f;
		f32 mAtt0 = 0.0f;
		f32 mAtt1 = 0.0f;
		f32 mAtt2 = 0.0f;

		SharedPtr<ScheduleHandle> mPushLightScheduleHandle;


#ifdef JG_EDITOR
		SharedPtr<Graphics::PaperObject> mTargetObject;
		SharedPtr<ScheduleHandle> mPushDebugHandle;
		Asset<ITexture>* mIcon = nullptr;
#endif // DEBUG
	public:
		virtual ~PointLight() = default;
	
	protected:
		virtual void Awake() override;
		virtual void Start() override;
		virtual void Destory() override;
	public:
		virtual void MakeJson(SharedPtr<JsonData> jsonData)   const override;
		virtual void LoadJson(SharedPtr<JsonData> jsonData) override;


	public:
		void SetColor(const Color& color);
		void SetIntensity(f32 intensity);
		void SetRange(f32 range);
		void SetAtt0(f32 att0);
		void SetAtt1(f32 att1);
		void SetAtt2(f32 att2);

		Color GetColor() const;
		f32 GetIntensity() const;
		f32 GetRange() const;
		f32 GetAtt0() const;
		f32 GetAtt1() const;
		f32 GetAtt2() const;


	private:
		virtual void OnInspectorGUI() override;
#ifdef JG_EDITOR
		EScheduleResult PushDebugRenderItem();
#endif
		EScheduleResult PushLightItem();
	};
}