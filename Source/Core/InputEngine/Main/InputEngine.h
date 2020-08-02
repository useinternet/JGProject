#pragma once




#ifdef INPUTENGINE_EXPORT

#define INPUTENGINE_API __declspec(dllexport)

#else

#define INPUTENGINE_API __declspec(dllimport)

#endif
#include "CommonCore.h"
namespace IE
{
	REGISTER_SYSTEM_CORE(InputEngine, 0, -100, 0)

	class INPUTENGINE_API InputEngine : public SystemCore
	{
	public:
		InputEngine(const SystemCoreDesc& desc) : SystemCore(desc) {}
		virtual void Awake() override;
		virtual void Start() override;
		virtual void Update() override;
		virtual void Destroy() override;
	public:
		virtual void ReceiveMessage(const Message& msg) override;


	private:



	};
}
