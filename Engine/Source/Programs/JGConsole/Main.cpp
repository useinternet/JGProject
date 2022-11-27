#include "PCH/PCH.h"




#include <iostream>
#include "Core.h"
#include "TestClass/TestClassA.h"


using namespace std;

void test()
{

}
int main()
{
	GCoreSystem::Create();
	
	PSharedPtr<JGObject> Object = Allocate<JGObject>();
	int32 testEnum = 0;



	GScheduleGlobalSystem::GetInstance().Schedule(Object, EMainThreadExecutionOrder::Begin, 
		[&](const PTaskArguments& Args)
	{
		GScheduleGlobalSystem::GetInstance().ScheduleAsync(ENamedThread::RemainThread, Object,
			[&](const PTaskArguments& args)
		{
			auto s1 = std::hash<std::thread::id>()(std::this_thread::get_id());
			JG_LOG(TEST, ELogLevel::Info, "Test Log1 %u", s1);
			GScheduleGlobalSystem::GetInstance().ScheduleAsync(ENamedThread::RemainThread, Object,
				[&](const PTaskArguments& args)
			{
				auto s2 = std::hash<std::thread::id>()(std::this_thread::get_id());
				JG_LOG(TEST, ELogLevel::Info, "Test Log2 %u", s2);
				GScheduleGlobalSystem::GetInstance().ScheduleAsync(ENamedThread::RemainThread, Object,
					[&](const PTaskArguments& args)
				{
					auto s3 = std::hash<std::thread::id>()(std::this_thread::get_id());
					JG_LOG(TEST, ELogLevel::Info, "Test Log3 %u", s3);
					GScheduleGlobalSystem::GetInstance().ScheduleAsync(ENamedThread::RemainThread, Object,
						[&](const PTaskArguments& args)
					{
						auto s4 = std::hash<std::thread::id>()(std::this_thread::get_id());
						JG_LOG(TEST, ELogLevel::Info, "Test Log4 %u", s2);
						GScheduleGlobalSystem::GetInstance().ScheduleAsync(ENamedThread::RemainThread, Object,
							[&](const PTaskArguments& args)
						{
							auto s5 = std::hash<std::thread::id>()(std::this_thread::get_id());
							JG_LOG(TEST, ELogLevel::Info, "Test Log5%u", s5);
							GScheduleGlobalSystem::GetInstance().ScheduleAsync(ENamedThread::RemainThread, Object,
								[&](const PTaskArguments& args)
							{
								auto s6 = std::hash<std::thread::id>()(std::this_thread::get_id());
								JG_LOG(TEST, ELogLevel::Info, "Test Log6 %u", s6);
							});
						});
					});
				});
			});
		});

	});



	while (true)
	{
		GCoreSystem::Update();
	}

	GCoreSystem::Destroy();
	return 0;
}


