#pragma once

#include "GFWCore.h"


namespace GFW
{
	REGISTER_SYSTEM_CORE(GameFrameWork, 0, 0, 0)

	class GAMEFRAMEWORK_API GameFrameWork : public SystemCore
	{
	public:
		GameFrameWork(const SystemCoreDesc& desc) : SystemCore(desc) {}
		virtual void Awake() override;
		virtual void Start() override;
		virtual void Update() override;
		virtual void Destroy() override;
		virtual ~GameFrameWork() {}
	public:
		virtual void ReceiveMessage(const Message& msg) override;


	private:
	};
}

