#pragma once
#include"Common/JGRCCommon.h"
#include"Common/JGRCGlobalType.h"
#include"DirectXCommon.h"

namespace JGRC
{
	class CORE_EXPORT JGRCObject
	{
		static unsigned long long Count;
	public:
		JGRCObject() { Count++; }
		virtual ~JGRCObject() { Count--; }
	};
}
