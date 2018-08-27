#pragma once
#include"../../EngineStatics/Engine.h"



class ENGINE_EXPORT JGShaderData
{
	const size_t MaxSize = 5;
	std::vector<std::wstring> DataNames;
	void* Data_1;
	void* Data_2;
	void* Data_3;
	void* Data_4;
	void* Data_5;
	UINT Slot = 0;
public:
	JGShaderData();
	JGShaderData(const JGShaderData& copy) = delete;
	void InsertData(const std::wstring& wstring, void* data);
	void* GetData(const std::wstring& DataName);
};