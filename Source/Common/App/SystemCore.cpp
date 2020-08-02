#include "pch.h"
#include "SystemCore.h"
using namespace std;


SystemCore::SystemCore(const SystemCoreDesc& desc) noexcept
{
	m_Desc = desc;
	m_ID = GlobalSharedData::GetMessageBus()->IssueID(desc.dllPath, this);
}