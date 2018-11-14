#pragma once
#include"Common/JGRCCommon.h"
#include"Common/JGRCGlobalType.h"
#include"DirectXCommon.h"

namespace JGRC
{
	class CORE_EXPORT JGRCObject
	{
		static unsigned long long Count;
		static class DirectX* m_Dx;
	public:
		JGRCObject();
		virtual ~JGRCObject() { Count--; }

	protected:
		DirectX* GetDx();
	public:
		static unsigned long long GetCount() { return Count; }
	};
}
