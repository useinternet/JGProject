#pragma once
#include "JGCore.h"
#include "UI/UIView.h"
#include "Class/Asset/Asset.h"

namespace JG
{
	namespace Graphics
	{
		class Scene;
		class StaticRenderObject;

	}
	class AnimationClipView : public UIView
	{
		JGCLASS
	public:
		AnimationClipView();
		virtual ~AnimationClipView() = default;
	protected:
		virtual void Load() override;
		virtual void Initialize() override;
		virtual void OnGUI() override;
		virtual void PreOnGUI() override;
		virtual void LateOnGUI() override;
		virtual void Destroy() override;
		virtual void OnEvent(IEvent& e) override;
		virtual void MakeJson(SharedPtr<JsonData> jsonData) const override { }
		virtual void LoadJson(SharedPtr<JsonData> jsonData) override { }
	private:
	};
}