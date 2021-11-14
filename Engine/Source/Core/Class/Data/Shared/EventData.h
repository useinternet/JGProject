#pragma once
#include "JGCore.h"


namespace JG
{
	namespace Shared
	{
		namespace Editor
		{
			struct ProgressBarOpenData
			{
				String Title;
				String Contents;
				f32    Ratio = 0.0f;
			};

			struct ProgressBarDisplayData
			{
				String Contents;
				f32    Ratio = 0.0f;
			};

			struct MessageBoxOpenData
			{
				String Title = "None";
				String Contents;
				int    MsgType = 0;
				String ParamName[2] = { "OK", "Cancel" };
				std::function<void()> ParamAction[2] = { nullptr, };
			};

		}

		namespace Game
		{

		}
	}

}


