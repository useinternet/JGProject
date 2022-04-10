#pragma once

#include "GameComponent.h"


namespace JG
{
	class Light : public GameComponent
	{
		JGCLASS
	private:
#ifdef JG_EDITOR
		SharedPtr<Graphics::PaperObject> mTargetObject;
		SharedPtr<ScheduleHandle>	     mPushDebugHandle;
		Asset<ITexture>* mIcon = nullptr;
		bool mIsEditorMode = true;
#endif // DEBUG
	public:
		virtual ~Light() = default;
	protected:
		virtual void Awake() override;
		virtual void Start() override;
		virtual void Update() override;
		virtual void FixedUpdate() override;
		virtual void Destory() override;
	public:
		virtual void MakeJson(SharedPtr<JsonData> jsonData)   const override;
		virtual void LoadJson(SharedPtr<JsonData> jsonData) override;
	protected:
		void CalcAtt(f32 attRange, f32* att0, f32 *att1, f32 *att2);
		virtual void PushLightItem();
	private:
#ifdef JG_EDITOR
		void PushDebugRenderItem();
#endif
	};




}