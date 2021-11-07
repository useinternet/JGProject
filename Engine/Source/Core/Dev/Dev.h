#pragma once
#include <string>
#include "Class/Game/GameComponent.h"


namespace JG
{
	namespace Dev
	{
		class DevComponent : public GameComponent
		{
			JGCLASS
		private:

		public:
			virtual ~DevComponent() = default;

			
		protected:
			virtual void Start() override;
			virtual void Update() override;
			virtual void Destory() override;
		public:
			virtual void MakeJson(SharedPtr<JsonData> jsonData)   const override;
			virtual void LoadJson(SharedPtr<JsonData> jsonData) override;


		};


	}
}
