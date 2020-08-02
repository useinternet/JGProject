#pragma once





#ifdef GUI_EXPORT

#define GUI_API __declspec(dllexport)

#else

#define GUI_API __declspec(dllimport)

#endif

#include "CommonCore.h"
#include "GraphicsIF.h"


namespace GUI
{
	REGISTER_SYSTEM_CORE(GUI, 0, 0, 0)




	class GUI_API GUI : public SystemCore
	{
	public:
		GUI(const SystemCoreDesc& desc) : SystemCore(desc) {}
		virtual ~GUI() {}
		virtual void Awake() override;
		virtual void Start() override;
		virtual void Update() override;
		virtual void Destroy() override;
	public:
		virtual void ReceiveMessage(const Message& msg) override;
	private:


	};

	/*

	1. Transform 검증
	2. ImageComponent Window, 고려하기




	
	*/
}