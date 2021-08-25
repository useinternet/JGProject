#pragma once
#include "Type.h"
#include "Object.h"




namespace JG
{
	class IDragAndDropData : public IJGObject
	{
		JGCLASS
	};

	class DDDContentsFile : public IDragAndDropData
	{
		JGCLASS
	public:
		char FilePath[256] = {};
	};

	class DDDGameNode : public IDragAndDropData
	{
		JGCLASS
	public:
		class GameNode* GameNode = nullptr;
	};

}
