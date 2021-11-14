#pragma once
#include "GameComponent.h"


namespace JG
{
	class Collision : public GameComponent
	{
		JGCLASS
	public:
		virtual ~Collision() = default;
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