#pragma once
#include "CommonCore.h"
#include "GlobalSharedData.h"
#include <string>

class Message;

class SystemCoreDesc
{
public:
	std::string dllPath;
	int start_priority = 0;
	int update_priority = 0;
	int destroy_priority = 0;
};

class __declspec(dllexport) SystemCore
{
public:
	SystemCore() noexcept {}
	SystemCore(const SystemCoreDesc& desc) noexcept;
	virtual ~SystemCore() {}
	virtual void Awake() = 0;
	virtual void Start() = 0;
	virtual void Update() = 0;
	virtual void Destroy() = 0;
public:
	virtual void ReceiveMessage(const Message& msg) = 0;
public:
	const SystemCoreDesc& GetDesc() const {
		return m_Desc;
	}
	uint64_t GetID() const {
		return m_ID;
	}
public:
	template<class CoreType>
	static CoreType* CreateSystemCore(const SystemCoreDesc& desc) {
		return new CoreType(desc);
	}

	static void DestroySystemCore(SystemCore* core) {
		delete core;
		core = nullptr;
	}
private:
	SystemCoreDesc m_Desc;
	uint64_t m_ID = -1;
};

using SystemCoreLoadFunc = SystemCore * (*)();





#define REGISTER_SYSTEM_CORE(className, startpriority, updatepriority, destroypriority) \
REGISTER_GLOBAL_SHARED_DATA \
class className; \
extern "C" __declspec(dllexport) \
className* CreateSystemCore_##className() \
{\
    \
	SystemCoreDesc desc; \
	desc.dllPath = #className; \
	desc.start_priority = startpriority; \
	desc.update_priority = updatepriority; \
	desc.destroy_priority = destroypriority; \
    return SystemCore::CreateSystemCore<##className>(desc); \
}\

