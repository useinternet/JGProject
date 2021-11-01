#pragma once
#include "Class/Game/GameComponent.h"


namespace JG
{
	class Collision : public GameComponent
	{
		JGCLASS
	protected:

	protected:
		virtual void Awake() override;
		virtual void Start() override;
		virtual void Destory() override;
	protected:
		virtual void Update() override;
		virtual void LateUpdate() override;
		virtual void MakeJson(SharedPtr<JsonData> jsonData) const override;
		virtual void LoadJson(SharedPtr<JsonData> jsonData) override;
		virtual void OnInspectorGUI() override;
	private:
		virtual void OnChange(const ChangeData& data) override;
	};


}