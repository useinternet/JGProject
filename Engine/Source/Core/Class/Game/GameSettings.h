#pragma once


#include "JGCore.h"



namespace JG
{
	class GameSettings
	{
	private:
		static JVector2 Resolution;
		static u32      UnitSize;
	public:
		static const JVector2& GetResolution() {
			return Resolution;
		}
		static const u32 GetUnitSize() {
			return UnitSize;
		}
	};
}