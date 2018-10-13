#include"BTT_MoveTo.h"
#include"EnemyUnit/AI/Board/BTIB_EnemyBoard.h"
#include"EngineStatics/JGLog.h"
BTT_MoveTo::BTT_MoveTo()
{
}

BTT_MoveTo::~BTT_MoveTo()
{

}

bool BTT_MoveTo::ExcuteTask(const float DeltaTime)
{
	static float count = 3.0f;
	count += DeltaTime;
	if (count >= 3.0f)
	{
		count = 0.0f;
		JGLog::Write(ELogLevel::Default, TT("Task Working!!"));
		if (GetBoard())
		{
			JGLog::Write(ELogLevel::Default, TT("Binding Complete!!"));
			BTIB_EnemyBoard* board = dynamic_cast<BTIB_EnemyBoard*>(GetBoard());
			if (board->EnemyObject)
			{
				JGLog::Write(ELogLevel::Default, TT("Binding Complete!!"));
			}
		}
	}
	return false;
}
