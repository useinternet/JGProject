#include "pch.h"
#include "Dev.h"
#include "Application.h"

namespace JG
{
	namespace Dev
	{
		void DevComponent::Start()
		{
			GameComponent::Start();
		}
		void DevComponent::Update()
		{
			GameComponent::Update();
			f32 tick = Application::GetInstance().GetAppTimer()->GetTick();
			static f32 accTime = 0.0f;
			accTime += tick;


			if (accTime > 1.0f)
			{
				JG_CORE_INFO("Test Simulate Update!!");
				accTime = 0.0f;
			}


		}
		void DevComponent::Destory()
		{
			GameComponent::Destory();
		}
		void DevComponent::MakeJson(SharedPtr<JsonData> jsonData) const
		{
			GameComponent::MakeJson(jsonData);
		}
		void DevComponent::LoadJson(SharedPtr<JsonData> jsonData)
		{
			GameComponent::LoadJson(jsonData);
		}
	}
}
