#pragma once


#include "GFWCore.h"
#ifdef GAMEFRAMEWORK_EXPORT

#define GFW_API __declspec(dllexport)

#else

#define GFW_API __declspec(dllimport)

#endif

extern "C" __declspec(dllexport)
void _TestFunc()
{

}


//REGISTER_SYSTEM_CORE(GameFrameWork, 0, -100, 0)




//class GFW_API GameFrameWork 
//{
//public:
//	GameFrameWork(const SystemCoreDesc& desc) : SystemCore(desc) {}
//	virtual void Awake() override;
//	virtual void Start() override;
//	virtual void Update() override;
//	virtual void Destroy() override;
//	virtual ~GameFrameWork() {}
//public:
//	virtual void ReceiveMessage(const Message& msg) override;
//
//
//private:
//
//
//
//};