#pragma once

#include "GameComponent.h"


namespace JG
{
	class Light : public GameComponent
	{
		JGCLASS
	public:
		virtual ~Light() = default;
	protected:
		virtual void Start() override;
		virtual void Destory() override;
	public:
		virtual void MakeJson(SharedPtr<JsonData> jsonData)   const override;
		virtual void LoadJson(SharedPtr<JsonData> jsonData) override;
	};




}