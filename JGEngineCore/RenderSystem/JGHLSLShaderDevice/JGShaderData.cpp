#include "JGShaderData.h"
#include"../../EngineStatics/JGLog.h"
using namespace std;

JGShaderData::JGShaderData()
{
}
void JGShaderData::InsertData(const std::wstring& BufferName, void * data)
{

	switch (Slot)
	{
	case 0:
		Data_1 = data;
		break;
	case 1:
		Data_2 = data;
		break;
	case 2:
		Data_3 = data;
		break;
	case 3:
		Data_4 = data;
		break;
	case 4:
		Data_5 = data;
		break;
	default:
		JGLog::Write(ELogLevel::Warning, TT("maximum index(%d) of JGShaderData is exceed ( %d )"), MaxSize, Slot);
	}
	DataNames.push_back(BufferName);
	Slot++;
}
void* JGShaderData::GetData(const wstring& DataName)
{
	size_t Count = 0;
	for (Count = 0; Count < DataNames.size() + 1; ++Count)
	{
		if (DataNames[Count] == DataName)
		{
			break;
		}
	}
	switch (Count)
	{
	case 0:
		return Data_1;
	case 1:
		return Data_2;
	case 2:
		return Data_3;
	case 3:
		return Data_4;
	case 4:
		return Data_5;
	default:
		JGLog::Write(ELogLevel::Warning, TT("maximum index(%d) of JGShaderData is exceed ( %d )"), MaxSize, Count);
		return &Data_1;
	}
}

