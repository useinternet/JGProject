#pragma once
#include "JGCore.h"
#include "UI/UIView.h"
#include "Class/Asset/Asset.h"

namespace JG
{
	class EditorUIScene;
	class ParticleSystemView : public UIView
	{
		JGCLASS
	private:
		UniquePtr<EditorUIScene>    mEditorUIScene;
	public:
		ParticleSystemView();
		virtual ~ParticleSystemView() = default;
	protected:
		virtual void Load() override;
		virtual void Initialize() override;
		virtual void OnGUI()   override;
		virtual void Destroy() override;
		virtual void OnEvent(IEvent& e) override;
		virtual void MakeJson(SharedPtr<JsonData> jsonData) const override { }
		virtual void LoadJson(SharedPtr<JsonData> jsonData) override { }
	};
}