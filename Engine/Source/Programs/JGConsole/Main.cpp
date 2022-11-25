#include "PCH/PCH.h"




#include <iostream>
#include "Core.h"
#include "TestClass/TestClassA.h"


using namespace std;

int main()
{
	GCoreSystem::Create();
	
	PSharedPtr<JGObject> Object = Allocate<JGObject>();
	int32 testEnum = 0;



	GScheduleGlobalSystem::GetInstance().ScheduleOnce(Object, EMainThreadExecutionOrder::Begin, 
		[&](const PTaskArguments& Args)
	{
		for (int i = 0; i < 45; ++i)
		{
			GScheduleGlobalSystem::GetInstance().ScheduleAsync(ENamedThread::RemainThread, Object,
				[&](const PTaskArguments& args)
			{
				auto s = std::hash<std::thread::id>()(std::this_thread::get_id());
				JG_LOG(TEST, ELogLevel::Info, "Test Log %u", s);
			});
		}

	});



	while (true)
	{
		GCoreSystem::Update();
	}

	GCoreSystem::Destroy();
	return 0;
}


